/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-EVAL Evaluation Board
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : STM32F103X RT-Thread 0.3.1 USB-CDC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#define  BSP_MODULE

#include <bsp.h>
/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/
#define DUTY_CYCLE 0
#define ADC1_DR_Address    ((u32)0x4001244C)
/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
uint16_t ADC1ConvertedValue[12];//单次AD转换值
/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/** This function will initial STM32 board**/
void rt_hw_board_init()
{
	BSP_Init();
	rt_components_board_init();
	rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
}

//******************************时钟配置函数***************************************
//函数定义: static void RCC_Configuration(void)
//函数功能：配置所有功能模块的时钟
//入口参数：无
//出口参数：无
//备    注：Editor：Zuohao 2013-08-29    Company: BXXJS
//**********************************************************************
static void RCC_Configuration(void)
{
    RCC_ClocksTypeDef rcc_clocks;

	RCC_GetClocksFreq(&rcc_clocks);
	/* 确定晶振完全起振 */
	RT_ASSERT(rcc_clocks.HCLK_Frequency == 120000000);

	//下面是给各模块开启时钟
    //启动GPIO
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | \
                           RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | \
                           RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG,
                           ENABLE);

	//启动USART1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	//启动USART2时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	/* Enable WWDG clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
}

//********************************NVIC配置函数***************************************
//函数定义: void NVIC_Configuration(void)
//函数功能：NVIC参数配置
//入口参数：无
//出口参数：无
//备    注：Editor：Zuohao 2013-08-29    Company: BXXJS
//**********************************************************************************
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

#if  defined(VECT_TAB_RAM)
    // Set the Vector Table base location at 0x20000000
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x00);
#elif  defined(VECT_TAB_FLASH)
    // Set the Vector Table base location at 0x08000000
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);
#elif defined(VECT_TAB_USER)
    // Set the Vector Table base location by user
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, USER_VECTOR_TABLE);
#endif

	//设置NVIC优先级分组为Group2：0-3抢占式优先级，0-3的响应式优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	//窗口看门狗中断配置
	NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
//******************************GPIO配置函数***************************************
//函数定义: static void GPIO_Configuration(void)
//函数功能：配置所有GPIO引脚功能
//入口参数：无
//出口参数：无
//备    注：Editor：Zuohao 2013-08-29    Company: BXXJS
//**********************************************************************
static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* PA13:SWDIO PA14:SWCLK for debug, can't set to output mode */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & (~(GPIO_Pin_13 | GPIO_Pin_14));
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    GPIO_Init(GPIOG, &GPIO_InitStructure);

    /******************系统运行LED指示灯配置*******************/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

//*******************初始化独立看门狗*************************************
//函数定义: void IWDG_Configuration(void) 
//描    述：初始化独立看门狗
//入口参数：无
//出口参数：无
//备    注：分频因子=4*2^prer.但最大值只能是256!时间计算(大概):Tout=40K/((4*2^prer)*rlr)值	 3S超时
//Editor：liuqh 2013-1-16  Company: BXXJS
//*******************************************************************
static void IWDG_Configuration(void) 
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//使能对IWDG->PR和IWDG->RLR的写
	IWDG_SetPrescaler(IWDG_Prescaler_64);//64分频
	IWDG_SetReload(1875);
	IWDG_ReloadCounter();
	IWDG_Enable();
}
//*******************喂独立看门狗*************************************
//函数定义: void IWDG_Feed(void)
//描    述：初始化独立看门狗
//入口参数：无
//出口参数：prer:分频数:0~7(只有低3位有效!)，rlr:重装载寄存器值:低11位有效.
//备    注：分频因子=4*2^prer.但最大值只能是256!时间计算(大概):Tout=40K/((4*2^prer)*rlr)值
//Editor：liuqh 2013-1-16  Company: BXXJS
//*******************************************************************
void IWDG_Feed(void)
{
	IWDG_ReloadCounter();//reload
}


/*******************************************************************************
 * Function Name  : SysTick_Configuration
 * Description    : Configures the SysTick for OS tick.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void  SysTick_Configuration(void)
{
	RCC_ClocksTypeDef  rcc_clocks;
	rt_uint32_t         cnts;

	RCC_GetClocksFreq(&rcc_clocks);

	cnts = (rt_uint32_t)rcc_clocks.HCLK_Frequency / RT_TICK_PER_SECOND;

	SysTick_Config(cnts);
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}
/**
 * This is the timer interrupt service routine.
 *
 */
void rt_hw_timer_handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

void assert_failed(u8* file, u32 line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* Infinite loop */
	rt_kprintf("assert failed at %s:%d \n", file, line);
	while (1) {
	}
}
/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*********************************************************************************************************
*/

void  BSP_Init (void)
{
	RCC_Configuration();
	NVIC_Configuration();
	SysTick_Configuration();
	GPIO_Configuration();
//	TODO  方便调试，暂时注释看门狗，正式发布时需要打开
// 	IWDG_Configuration();
}

