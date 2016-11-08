#include <app_task.h>
#include <stdlib.h>
#include <stdio.h>
#include <bsp.h>
#include <delay_conf.h>
#include <cpuusage.h>
#include <utils.h>
#include <shell.h>
#include <finsh.h>
#include <components.h>
#include <sfud.h>
#include <spi_flash.h>
#include <spi_flash_sfud.h>

#ifdef __CC_ARM
extern int Image$$RW_IRAM1$$ZI$$Limit;
#elif __ICCARM__
#pragma section="HEAP"
#else
extern int __bss_end;
#endif

#define THREAD_SYS_MONITOR_PRIO                30

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t thread_sys_monitor_stack[512];

struct rt_thread thread_sys_monitor;
uint8_t cpu_usage_major, cpu_usage_minor;

rt_spi_flash_device_t w25q64, w25q128;

/**
 * System monitor thread.
 *
 * @param parameter parameter
 */
void thread_entry_sys_monitor(void* parameter)
{
    extern void cpu_usage_get(rt_uint8_t *major, rt_uint8_t *minor);
    while (1)
    {
        if(get_system_status() == SYSTEM_STATUS_RUN){
            cpu_usage_get(&cpu_usage_major, &cpu_usage_minor);
            LED_RUN_ON;
            rt_thread_delay(DELAY_SYS_RUN_LED_ON);
            LED_RUN_OFF;
            rt_thread_delay(DELAY_SYS_RUN_LED_OFF);
        } else if (get_system_status() == SYSTEM_STATUS_INIT){
            LED_RUN_ON;
            rt_thread_delay(DELAY_SYS_INIT_LED_ON);
            LED_RUN_OFF;
            rt_thread_delay(DELAY_SYS_INIT_LED_OFF);
        } else if (get_system_status() == SYSTEM_STATUS_FAULT){
            LED_RUN_ON;
            rt_thread_delay(DELAY_SYS_FAULT_LED_ON);
            LED_RUN_OFF;
            rt_thread_delay(DELAY_SYS_FAULT_LED_OFF);
        }
        IWDG_Feed();
    }
}


/**
 * System initialization thread.
 *
 * @param parameter parameter
 */
void sys_init_thread(void* parameter){
    set_system_status(SYSTEM_STATUS_INIT);

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_init();
#endif

#ifdef  RT_USING_FINSH
    finsh_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

    /* Add CPU usage to system */
    cpu_usage_init();

    /* SFUD initialize */
    w25q64 = rt_sfud_flash_probe("W25Q64", "spi10");
    w25q128 = rt_sfud_flash_probe("W25Q128", "spi30");
    
    /* initialize OK and switch to running status */
    set_system_status(SYSTEM_STATUS_RUN);

    rt_thread_delete(rt_thread_self());
}



int rt_application_init(void)
{
    rt_thread_t init_thread = NULL;
    rt_thread_init(&thread_sys_monitor,
                   "sys_monitor",
                   thread_entry_sys_monitor,
                   RT_NULL,
                   thread_sys_monitor_stack,
                   sizeof(thread_sys_monitor_stack),
                   THREAD_SYS_MONITOR_PRIO, 5);
    rt_thread_startup(&thread_sys_monitor);

    init_thread = rt_thread_create("sys init", sys_init_thread,NULL, 2048, 10, 10);
    if (init_thread != NULL) {
        rt_thread_startup(init_thread);
    }

    return 0;
}

/**
 * This function will startup RT-Thread RTOS.
 */
void rtthread_startup(void)
{
    /* init board */
    rt_hw_board_init();

    /* show version */
    rt_show_version();

    /* init tick */
    rt_system_tick_init();

    /* init kernel object */
    rt_system_object_init();

    /* init timer system */
    rt_system_timer_init();

#ifdef RT_USING_HEAP
#ifdef __CC_ARM
    rt_system_heap_init((void*)&Image$$RW_IRAM1$$ZI$$Limit, (void*)STM32_SRAM_END);
#elif __ICCARM__
    rt_system_heap_init(__segment_end("HEAP"), (void*)STM32_SRAM_END);
#else
    /* init memory system */
    rt_system_heap_init((void*)&__bss_end, (void*)STM32_SRAM_END);
#endif
#endif

    /* init scheduler system */
    rt_system_scheduler_init();

    /* initialize timer */
    rt_system_timer_init();

    /* init timer thread */
    rt_system_timer_thread_init();

    /* init application */
    rt_application_init();

    /* init idle thread */
    rt_thread_idle_init();

    /* start scheduler */
    rt_system_scheduler_start();

    /* never reach here */
    return;
}
