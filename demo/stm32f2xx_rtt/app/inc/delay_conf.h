/*********************************************************************************************************/
// Filename      : delay_config.h
// Version       : V1.00
// Programmer(s) : Liuqiuhu
// funcion		 : This file is used to configure the delay time
/*********************************************************************************************************/
#ifndef __DELAY_CONF_H__
#define __DELAY_CONF_H__

#include"rtconfig.h"

#if RT_TICK_PER_SECOND == 1
#define DELAY_1S			(RT_TICK_PER_SECOND)		
#define DELAY_S(X) 			(X*DELAY_1S)

#elif RT_TICK_PER_SECOND == 10				
#define DELAY_100MS(X)		(X)
#define DELAY_S(X)			(X*10)

#elif RT_TICK_PER_SECOND == 100		
#define	DELAY_10MS(X)		(X)
#define	DELAY_100MS(X)		(X*10)
#define DELAY_S(X)			(X*100)

#elif (RT_TICK_PER_SECOND == 1000)

#define DELAY_1MS           (RT_TICK_PER_SECOND/1000)		
#define DELAY_MS(X)         (X*DELAY_1MS)
#define DELAY_S(X)		    (X*1000*DELAY_1MS)

#elif (RT_TICK_PER_SECOND == 10000)
#define DELAY_100US(X)      (X*RT_TICK_PER_SECOND/10000)
#define DELAY_1MS           (RT_TICK_PER_SECOND/1000)		
#define DELAY_MS(X)         (X*DELAY_1MS)
#define DELAY_S(X)		    (X*1000*DELAY_1MS)

#endif

#define DELAY_SYS_INIT_LED_ON       DELAY_MS(90)   //系统初始化指示灯亮延时
#define DELAY_SYS_INIT_LED_OFF      DELAY_MS(10)   //系统初始化指示灯灭延时
#define DELAY_SYS_RUN_LED_ON        DELAY_MS(960) //系统正常运行指示灯亮延时
#define DELAY_SYS_RUN_LED_OFF       DELAY_MS(40)  //系统正常运行指示灯灭延时
#define DELAY_SYS_FAULT_LED_ON      DELAY_MS(40)   //系统故障运行指示灯亮延时
#define DELAY_SYS_FAULT_LED_OFF     DELAY_MS(960)  //系统故障运行指示灯灭延时

#define DELAY_SYS_SLEEP_LED			DELAY_MS(1000) //系统睡眠指示灯延时1s
#define POLL_CONNECT_CLOUD			DELAY_MS(100)  //轮询连接到白象云服务器的周期时间
#define TIMEOUT_CONNECT_CLOUD		DELAY_S(30)    //连接到白象云服务器的超时时间


#endif  

