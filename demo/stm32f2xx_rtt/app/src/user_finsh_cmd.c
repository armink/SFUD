/*
 * user_finsh_cmd.c
 *
 *  Created on: 2013年12月7日
 *      Author: Armink
 */
#include <rthw.h>
#include <rtthread.h>
#include <stm32f2xx_conf.h>
#include <finsh.h>
#include <cpuusage.h>
#include <sfud.h>
#include <ymodem.h>
#include <shell.h>
#include <spi_flash.h>

static void reboot(uint8_t argc, char **argv) {
    NVIC_SystemReset();
}
MSH_CMD_EXPORT(reboot, Reboot System);

static void get_cpuusage(void) {
    uint8_t cpu_usage_major, cpu_usage_minor;

    cpu_usage_get(&cpu_usage_major, &cpu_usage_minor);
    rt_kprintf("The CPU usage is %d.%d% now.\n", cpu_usage_major, cpu_usage_minor);
}
MSH_CMD_EXPORT(get_cpuusage, Get control board cpu usage);

#define FILE_FLASH_ADDR                     0 /* 传输的 Flash 目标地址 */

extern rt_spi_flash_device_t w25q128;
static uint32_t ymodem_file_total_size, ymodem_file_cur_size;
static enum rym_code ymodem_on_begin(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len) {
    char *file_name, *file_size;

    /* calculate and store file size */
    file_name = (char *) &buf[0];
    file_size = (char *) &buf[rt_strlen(file_name) + 1];
    ymodem_file_total_size = atol(file_size);
    ymodem_file_cur_size = 0;

    /* erase flash */
    if (sfud_erase((sfud_flash_t)(w25q128->user_data), FILE_FLASH_ADDR, ymodem_file_total_size)
            != SFUD_SUCCESS) {
        /* if erase fail then quit this session */
        return RYM_CODE_CAN;
    }

    return RYM_CODE_ACK;
}

static enum rym_code ymodem_on_data(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len) {
    /* write file to flash */
    if (sfud_write((sfud_flash_t)(w25q128->user_data), FILE_FLASH_ADDR + ymodem_file_cur_size, len, buf)
            != SFUD_SUCCESS) {
        /* if write fail then quit this session */
        return RYM_CODE_CAN;
    }
    ymodem_file_cur_size += len;
    return RYM_CODE_ACK;
}

static void ymodem(uint8_t argc, char **argv) {
    struct rym_ctx rctx;

    rt_kprintf("Please select a file and use Ymodem to send.\r\n");
    /* close finsh echo */
    finsh_set_echo(false);
    if (!rym_recv_on_device(&rctx, rt_device_find(RT_CONSOLE_DEVICE_NAME), RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
            ymodem_on_begin, ymodem_on_data, NULL, RT_TICK_PER_SECOND)) {
        /* wait some time for terminal response finish */
        rt_thread_delay(RT_TICK_PER_SECOND);
        rt_kprintf("Write file to flash success.\n");
    } else {
        /* wait some time for terminal response finish */
        rt_thread_delay(RT_TICK_PER_SECOND);
        rt_kprintf("Write file to flash failed.\n");
    }
    /* reopen finsh echo */
    finsh_set_echo(true);
}
MSH_CMD_EXPORT(ymodem, save file to flash by ymodem)
