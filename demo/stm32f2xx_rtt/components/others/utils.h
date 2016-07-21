/*
 * utils.h
 *
 *  Created on: 2013-11-26
 *      Author: BX-Zuohao
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <stm32f2xx_conf.h>
#include <rtthread.h>

/* system status */
typedef enum{
    SYSTEM_STATUS_INIT,
    SYSTEM_STATUS_RUN,
    SYSTEM_STATUS_FAULT,
    SYSTEM_STATUS_SLEEP,
}SystemStatus;

uint8_t AvoidTimeout(uint32_t TimeOfTimeout,uint32_t Period,uint8_t (*DetectCondition)(),uint8_t ConditionValue);
void Delay(vu32 nCount);
void system_go_to_fault_status(void);
void set_system_status(SystemStatus status);
SystemStatus get_system_status(void);

#endif /* UTILS_H_ */
