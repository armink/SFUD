/*
 * user_finsh_cmd.c
 *
 *  Created on: 2013Äê12ÔÂ7ÈÕ
 *      Author: Armink
 */
#include <rthw.h>
#include <rtthread.h>
#include <stm32f2xx_conf.h>
#include <finsh.h>
#include <cpuusage.h>
#include <sfud.h>

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

static void sf(uint8_t argc, char **argv) {

#define CMD_SETECT_INDEX     0
#define CMD_READ_INDEX       1
#define CMD_WRITE_INDEX      2
#define CMD_ERASE_INDEX      3
#define CMD_BENCH_INDEX      4

    sfud_err result = SFUD_SUCCESS;
    const static sfud_flash *flash = NULL;

    const char* sf_help_info[] = {
            [CMD_SETECT_INDEX] = "sf select [index]             - select a flash chip with device's index",
            [CMD_READ_INDEX]   = "sf read addr size             - read 'size' bytes starting at 'addr'",
            [CMD_WRITE_INDEX]  = "sf write addr data1 ... dataN - write some bytes 'data' to flash starting at 'addr'",
            [CMD_ERASE_INDEX]  = "sf erase addr size            - erase 'size' bytes starting at 'addr'",
            [CMD_BENCH_INDEX]  = "sf bench                      - full chip benchmark test",
    };

    if (argc < 2) {
        rt_kprintf("Usage:\n");
        for (size_t i = 0; i < sizeof(sf_help_info) / sizeof(char*); i++) {
            rt_kprintf("%s\n", sf_help_info[i]);
        }
        rt_kprintf("\n");
    } else {
        const char *operator = argv[1];
        uint32_t addr, size;

        if (!strcmp(operator, "select")) {
            if (argc < 3) {
                rt_kprintf("Usage:%s.\n", sf_help_info[CMD_SETECT_INDEX]);
                if(sfud_get_device_num() > 0) {
                    for (size_t i = 0; i < sfud_get_device_num(); i++) {
                        if (sfud_get_device_table()[i].init_ok) {
                            rt_kprintf("The index %d flash device name is %s, ", i, sfud_get_device_table()[i].name);
                            if (sfud_get_device_table()[i].chip.capacity < 1024 * 1024) {
                                rt_kprintf("total is %d KB", sfud_get_device_table()[i].chip.capacity / 1024);
                            } else {
                                rt_kprintf("total is %d MB", sfud_get_device_table()[i].chip.capacity / 1024 / 1024);
                            }
                            if (sfud_get_device_table()[i].chip.name != NULL) {
                                rt_kprintf(", type is %d KB", sfud_get_device_table()[i].chip.name);
                            }
                            rt_kprintf(".\n");
                        }
                    }
                } else {
                    rt_kprintf("There is no flash device in device table.\n");
                }
            } else {
                size_t device_index = atol(argv[2]);
                if (device_index >= sfud_get_device_num()) {
                    rt_kprintf("Flash device index out bound[0:%d].\n", sfud_get_device_num());
                    return;
                }
                if (!sfud_get_device_table()[device_index].init_ok) {
                    rt_kprintf("Flash %s isn't initialize OK.\n", sfud_get_device_table()[device_index].name);
                    return;
                }
                flash = sfud_get_device_table() + device_index;
                if (flash->chip.capacity < 1024 * 1024) {
                    rt_kprintf("%d KB %s is current selected device.\n", flash->chip.capacity / 1024, flash->name);
                } else {
                    rt_kprintf("%d MB %s is current selected device.\n", flash->chip.capacity / 1024 / 1024, flash->name);
                }
            }
        } else {
            if (!flash) {
                rt_kprintf("No flash device selected. Please run 'sf select'.\n");
                return;
            }
            if (!strcmp(operator, "read")) {
                if (argc < 4) {
                    rt_kprintf("Usage:%s.\n", sf_help_info[CMD_READ_INDEX]);
                    return;
                } else {
                    addr = atol(argv[2]);
                    size = atol(argv[3]);
                    uint8_t *data = rt_malloc(size);
                    if (data) {
                        result = sfud_read(flash, addr, size, data);
                        if (result == SFUD_SUCCESS) {
                            rt_kprintf("Read the %s flash data success. Start from 0x%08X, size is %ld. The data is:\n",
                                    flash->name, addr, size);
                            rt_kprintf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
                            for (size_t i = 0; i < size; i++) {
                                if (i % 16 == 0) {
                                    rt_kprintf("[%08X] ", addr + i);
                                }
                                rt_kprintf("%02X ", data[i]);
                                if (((i + 1) % 16 == 0) || i == size - 1) {
                                    rt_kprintf("\n");
                                }
                            }
                            rt_kprintf("\n");
                        }
                        rt_free(data);
                    } else {
                        rt_kprintf("Low memory!\n");
                    }
                }
            } else if (!strcmp(operator, "write")) {
                if (argc < 4) {
                    rt_kprintf("Usage:%s.\n", sf_help_info[CMD_WRITE_INDEX]);
                    return;
                } else {
                    addr = atol(argv[2]);
                    size = argc - 3;
                    uint8_t *data = rt_malloc(size);
                    if (data) {
                        for (size_t i = 0; i < size; i++) {
                            data[i] = atoi(argv[3 + i]);
                        }
                        result = sfud_write(flash, addr, size, data);
                        if (result == SFUD_SUCCESS) {
                            rt_kprintf("Write the %s flash data success. Start from 0x%08X, size is %ld.\n",
                                    flash->name, addr, size);
                            rt_kprintf("Write data: ");
                            for (size_t i = 0; i < size; i++) {
                                rt_kprintf("%d ", data[i]);
                            }
                            rt_kprintf(".\n");
                        }
                        rt_free(data);
                    } else {
                        rt_kprintf("Low memory!\n");
                    }
                }
            } else if (!strcmp(operator, "erase")) {
                if (argc < 4) {
                    rt_kprintf("Usage:%s.\n", sf_help_info[CMD_ERASE_INDEX]);
                    return;
                } else {
                    addr = atol(argv[2]);
                    size = atol(argv[3]);
                    result = sfud_erase(flash, addr, size);
                    if (result == SFUD_SUCCESS) {
                        rt_kprintf("Erase the %s flash data success. Start from 0x%08X, size is %ld.\n", flash->name,
                                addr, size);
                    }
                }
            } else if (!strcmp(operator, "bench")) {
                /* full chip benchmark test */
                addr = 0;
                size = flash->chip.capacity;
                uint32_t start_time, time_cast;
                rt_uint32_t total_mem, used_mem, max_uesd_mem;
                rt_memory_info(&total_mem, &used_mem, &max_uesd_mem);
                size_t write_size = SFUD_WRITE_MAX_PAGE_SIZE, read_size;
                if ((total_mem - used_mem) / 2 < size) {
                    read_size = (total_mem - used_mem) / 2;
                } else {
                    read_size = size;
                }
                uint8_t *write_data = rt_malloc(write_size), *read_data = rt_malloc(read_size);

                if (write_data && read_data) {
                    memset(write_data, 0x55, write_size);
                    /* benchmark testing */
                    rt_kprintf("Erasing the %s %ld bytes data, waiting...\n", flash->name, size);
                    start_time = rt_tick_get();
                    result = sfud_erase(flash, addr, size);
                    if (result == SFUD_SUCCESS) {
                        time_cast = rt_tick_get() - start_time;
                        rt_kprintf("Erase benchmark success, total time: %d.%03dS.\n", time_cast / RT_TICK_PER_SECOND,
                                time_cast % RT_TICK_PER_SECOND / (RT_TICK_PER_SECOND / 1000));
                    } else {
                        rt_kprintf("Erase benchmark has an error. Error code: %d.\n", result);
                    }
                    /* write test */
                    rt_kprintf("Writing the %s %ld bytes data, waiting...\n", flash->name, size);
                    start_time = rt_tick_get();
                    for (size_t i = 0; i < size; i += write_size) {
                        result = sfud_write(flash, addr + i, write_size, write_data);
                        if (result != SFUD_SUCCESS) {
                            break;
                        }
                    }
                    if (result == SFUD_SUCCESS) {
                        time_cast = rt_tick_get() - start_time;
                        rt_kprintf("Write benchmark success, total time: %d.%03dS.\n", time_cast / RT_TICK_PER_SECOND,
                                time_cast % RT_TICK_PER_SECOND / (RT_TICK_PER_SECOND / 1000));
                    } else {
                        rt_kprintf("Write benchmark has an error. Error code: %d.\n", result);
                    }
                    /* read test */
                    rt_kprintf("Reading the %s %ld bytes data, waiting...\n", flash->name, size);
                    start_time = rt_tick_get();
                    for (size_t i = 0; i < size; i += read_size) {
                        if (i + read_size <= size) {
                            result = sfud_read(flash, addr + i, read_size, read_data);
                        } else {
                            result = sfud_read(flash, addr + i, size - i, read_data);
                        }
                        if (result != SFUD_SUCCESS) {
                            break;
                        }
                    }
                    if (result == SFUD_SUCCESS) {
                        time_cast = rt_tick_get() - start_time;
                        rt_kprintf("Read benchmark success, total time: %d.%03dS.\n", time_cast / RT_TICK_PER_SECOND,
                                time_cast % RT_TICK_PER_SECOND / (RT_TICK_PER_SECOND / 1000));
                    } else {
                        rt_kprintf("Read benchmark has an error. Error code: %d.\n", result);
                    }
                } else {
                    rt_kprintf("Low memory!\n");
                }
                rt_free(write_data);
                rt_free(read_data);
            } else {
                rt_kprintf("Usage:\n");
                for (size_t i = 0; i < sizeof(sf_help_info) / sizeof(char*); i++) {
                    rt_kprintf("%s\n", sf_help_info[i]);
                }
                rt_kprintf("\n");
                return;
            }
            if (result != SFUD_SUCCESS) {
                rt_kprintf("This flash operate has an error. Error code: %d.\n", result);
            }
        }
    }
}
MSH_CMD_EXPORT(sf, SPI Flash operate);

static void flash_status(uint8_t argc, char **argv) {
    if(argc < 2) {
        rt_kprintf("Please input flash_status <read|write> <device_index> [<1:volatile|0:non-volatile> <status>].\n");
    } else {
        const char *operator = argv[1];
        size_t device_index = atol(argv[2]);
        if (device_index >= sfud_get_device_num()) {
            rt_kprintf("Flash device index out bound[0:%d].\n", sfud_get_device_num());
            return;
        }
        const sfud_flash *flash = sfud_get_device_table() + device_index;

        if (!strcmp(operator, "read")) {
            uint8_t status;
            if (sfud_read_status(flash, &status) == SFUD_SUCCESS) {
                rt_kprintf("The %s flash status register value is 0x%02X.\n", flash->name, status);
            } else {
                rt_kprintf("Read the %s flash status register has an error!", flash->name);
            }
        } else if (!strcmp(operator, "write")) {
            if (argc == 5) {
                bool is_volatile = atoi(argv[3]);
                uint8_t status = atoi(argv[4]);
                if (sfud_write_status(flash, is_volatile, status) == SFUD_SUCCESS) {
                    rt_kprintf("Write the %s flash status register to 0x%02X success.\n", flash->name, status);
                } else {
                    rt_kprintf("Write the %s flash status register to 0x%02X has an error!", status, flash->name);
                }
            } else {
                rt_kprintf("Please input flash_status <read|write> <device_index> [<1:volatile|0:non-volatile> <status>].\n");
            }
        } else {
            rt_kprintf("Please input flash_status <read|write> <device_index> [<1:volatile|0:non-volatile> <status>].\n");
        }
    }
}
MSH_CMD_EXPORT(flash_status, Flash status <read|write> <device_index> [<1:volatile|0:non-volatile> <status>]);
