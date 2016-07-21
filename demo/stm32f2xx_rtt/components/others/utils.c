/*
 * utils.c
 *
 *  Created on: 2013-11-26
 *      Author: BX-Zuohao
 */

#include "utils.h"
#include <stdlib.h>

#define SW_VER_LENGTH        3
#define HW_VER_LENGTH        2

/* 当前系统状态 */
static SystemStatus cur_system_status = SYSTEM_STATUS_INIT;

//软件版本号定义
const char sw_ver_name[] @ ".version" = "swver";
const char sw_ver_value[] @ ".version" = "0.06.23";
//硬件版本号定义
const char hw_ver_name[] @ ".version" = "hwver";
const char hw_ver_value[] @ ".version" = "1.0";
//软硬件版本，根据上面的定义，确定位数
static uint8_t software_version[SW_VER_LENGTH] = { 0 };
static uint8_t hardware_version[HW_VER_LENGTH] = { 0 };

/**
 * System go to fault status.
 */
void system_go_to_fault_status(void){
    cur_system_status = SYSTEM_STATUS_FAULT;
}

/**
 * Set current system status.
 *
 * @param status system status
 */
void set_system_status(SystemStatus status){
    cur_system_status = status;
}

/**
 * Get current system status.
 *
 * @return current system status
 */
SystemStatus get_system_status(void){
    return cur_system_status;
}

/**
 * Get hardware version
 *
 * @return hardware version
 */
char const *get_hardware_version(void){
    return hw_ver_value;
}

/**
 * Get software version
 *
 * @return software version
 */
char const *get_software_version(void){
    return sw_ver_value;
}
/**
 * Calc software version's Integer value
 */
void calc_software_version(void) {
    char const *start_addr = NULL;
    char *match_addr = NULL;
    char c_value[4] = { 0 };
    uint8_t c_length = 0, i = 0;

    start_addr = get_software_version();
    match_addr = rt_strstr((char *) start_addr, ".");
    while (match_addr != NULL) {
        c_length = match_addr - start_addr;
        rt_memcpy(c_value, start_addr, c_length);
        software_version[i++] = atoi(c_value);
        match_addr++;
        start_addr = match_addr;
        match_addr = rt_strstr((char *) start_addr, ".");
    }
    c_length = rt_strlen(start_addr);
    rt_memcpy(c_value, start_addr, c_length);
    software_version[i] = atoi(c_value);
}
/**
 * Calc hardware version's Integer value
 */
void calc_hardware_version(void) {
    char const *start_addr = NULL;
    char *match_addr = NULL;
    char c_value[4] = { 0 };
    uint8_t c_length = 0, i = 0;

    start_addr = get_hardware_version();
    match_addr = rt_strstr((char *) start_addr, ".");
    while (match_addr != NULL) {
        c_length = match_addr - start_addr;
        rt_memcpy(c_value, start_addr, c_length);
        hardware_version[i++] = atoi(c_value);
        match_addr++;
        start_addr = match_addr;
        match_addr = rt_strstr((char *) start_addr, ".");
    }
    c_length = rt_strlen(start_addr);
    rt_memcpy(c_value, start_addr, c_length);
    hardware_version[i] = atoi(c_value);
}
/**
 * @param software version length
 *
 * @return  integer array of software version number pointer
 */
uint8_t *get_sw_ver_value(void) {
    return software_version;
}
/**
 * @param hardware version length
 *
 * @return  integer array of hardware version number pointer
 */
uint8_t *get_hw_ver_value(void) {
    return hardware_version;
}
