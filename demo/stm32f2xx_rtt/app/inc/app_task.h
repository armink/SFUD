/*********************************************************************************************************
//                             NCLUDE FILES
*********************************************************************************************************/
#ifndef APP_TASK_H
#define APP_TASK_H

#include <rthw.h>	
#include <rtthread.h>
#include <stm32f2xx_conf.h>

void rtthread_startup(void);

extern void cpu_usage_get(rt_uint8_t *major, rt_uint8_t *minor);
extern void cpu_usage_init(void);

#endif
