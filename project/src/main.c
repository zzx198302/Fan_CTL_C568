/* add user code begin Header */
/**
  **************************************************************************
  * @file     main.c
  * @brief    main program
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */
/* add user code end Header */

/* Includes ------------------------------------------------------------------*/
#include "at32f403a_407_wk_config.h"
#include "wk_adc.h"
#include "wk_debug.h"
#include "wk_i2c.h"
#include "wk_rtc.h"
#include "wk_tmr.h"
#include "wk_usart.h"
#include "wk_wdt.h"
#include "wk_dma.h"
#include "wk_gpio.h"
#include "wk_system.h"

/* private includes ----------------------------------------------------------*/
/* add user code begin private includes */
#include "User_head.h"
#include "led_dsp.h"
/* add user code end private includes */

/* private typedef -----------------------------------------------------------*/
/* add user code begin private typedef */
/* add user code end private typedef */

/* private define ------------------------------------------------------------*/
/* add user code begin private define */
#define TASK_PRIOR_Led    4
#define TASK_PRIOR_Key    5
#define TASK_PRIOR_Adc    6
#define TASK_PRIOR_Lcd    7
#define TASK_PRIOR_Usart1 8
#define TASK_PRIOR_Usart6 9
/* add user code end private define */

/* private macro -------------------------------------------------------------*/
/* add user code begin private macro */

/* add user code end private macro */

/* private variables ---------------------------------------------------------*/
/* add user code begin private variables */
static UINT32 TaskLed_ID;
static UINT32 TaskKey_ID;
static UINT32 TaskAdc_ID;
static UINT32 TaskLcd_ID;
static UINT32 TaskUsart1_ID;
static UINT32 TaskUsart6_ID;

uint32_t ADC1_Finish_Sem;
uint32_t Usart1_Recv_Finish_Sem;
uint32_t Usart6_Recv_Finish_Sem;
/* add user code end private variables */

/* private function prototypes --------------------------------------------*/
/* add user code begin function prototypes */
void System_Init(void);
UINT32 TaskMain(void);
UINT32 TaskLed(void);
UINT32 TaskKey(void);
UINT32 TaskAdc(void);
UINT32 TaskLcd(void);
UINT32 TaskUsart1(void);
UINT32 TaskUsart6(void);
/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */

/* add user code end 0 */

/**
  * @brief main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  /* add user code begin 1 */

  /* add user code end 1 */

  /* system clock config. */
  wk_system_clock_config();

  /* config periph clock. */
  wk_periph_clock_config();

  /* init debug function. */
  wk_debug_config();

  /* nvic config. */
  wk_nvic_config();

  /* timebase config. */
  wk_timebase_init();

  /* init gpio function. */
  wk_gpio_config();

  /* init dma1 channel1 */
  wk_dma1_channel1_init();
  /* config dma channel transfer parameter */
  /* user need to modify define values DMAx_CHANNELy_XXX_BASE_ADDR and DMAx_CHANNELy_BUFFER_SIZE in at32xxx_wk_config.h */
  wk_dma_channel_config(DMA1_CHANNEL1, 
                        (uint32_t)&ADC1->odt, 
                        DMA1_CHANNEL1_MEMORY_BASE_ADDR, 
                        DMA1_CHANNEL1_BUFFER_SIZE);
  dma_channel_enable(DMA1_CHANNEL1, TRUE);

  /* init usart1 function. */
  wk_usart1_init();

  /* init usart6 function. */
  wk_usart6_init();

  /* init i2c2 function. */
  wk_i2c2_init();

  /* init adc1 function. */
  wk_adc1_init();

  /* init rtc function. */
  wk_rtc_init();

  /* init tmr2 function. */
  wk_tmr2_init();

  /* init tmr6 function. */
  wk_tmr6_init();

  /* init tmr7 function. */
  wk_tmr7_init();

  /* init wdt function. */
  wk_wdt_init();

  /* add user code begin 2 */
  //OS启动前先不开启SysTick
  SysTick->CTRL &= (~SysTick_CTRL_ENABLE_Msk); 
  //SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
  TaskMain();
  /* add user code end 2 */

  while(1)
  {
    /* add user code begin 3 */

    /* add user code end 3 */
  }
}

  /* add user code begin 4 */
/***********************系统初始化**************************/

void System_Init(void)
{
    //ADC启动
    tmr_channel_enable(TMR2, TMR_SELECT_CHANNEL_2, TRUE);
    tmr_output_enable(TMR2, TRUE);
    dma_interrupt_enable(DMA1_CHANNEL1, DMA_FDT_INT, TRUE);
    //打开定时器中断
    tmr_interrupt_enable(TMR6, TMR_OVF_INT, TRUE);
    tmr_interrupt_enable(TMR7, TMR_OVF_INT, TRUE);
    //存储
    UserEpromInit();
    variableReadParaInit();
    EventLogReadInit();
    RdRlayStatus();
    ComPara();
    //串口
 /*
    Meter_usart.Usart1_addr = 1;
    Meter_run.FanComAdd[0] = 1;
    Meter_run.FanComAdd[1] = 2;
    Meter_run.FanComAdd[2] = 3;
   
    Meter_run.Tempert[0] = -4.2;
    Meter_run.Tempert[1] = -10.4;
    Meter_run.Tempert[2] = 124.3;
    
    Meter_run.Humidity[0] = 23.4;
    Meter_run.Humidity[1] = 6.7;
    Meter_run.Humidity[2] = 66.7;
     
    KG_7_Ctl(FAN_CTL_OPEN);
*/
    
    MX_USART1_UART_Init_Ex();
    HAL_INIT_USART1();
    MX_USART6_UART_Init_Ex();
    HAL_INIT_USART6();
    //显示
    led_dsp_init();
    //最后开启看门狗
    //wdt_enable();
}

/***********************事件任务创建**************************/
UINT32 TaskMain(void)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stInitParam;

    if (LOS_KernelInit() != LOS_OK)  //系统内核初始化
    {
        return LOS_NOK;
    }
    
    //创建信号量
    LOS_SemCreate(0, &ADC1_Finish_Sem);
    LOS_SemCreate(0, &Usart1_Recv_Finish_Sem);
    LOS_SemCreate(0, &Usart6_Recv_Finish_Sem);
    
    System_Init();
    //任务1
    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskLed;
    stInitParam.usTaskPrio = TASK_PRIOR_Led;
    stInitParam.pcName = "TaskLed";
    stInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uwRet = LOS_TaskCreate(&TaskLed_ID, &stInitParam);
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }
    //任务2
    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskKey;
    stInitParam.usTaskPrio = TASK_PRIOR_Key;
    stInitParam.pcName = "TaskKey";
    stInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uwRet = LOS_TaskCreate(&TaskKey_ID, &stInitParam);
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }
    //任务3
    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskAdc;
    stInitParam.usTaskPrio = TASK_PRIOR_Adc;
    stInitParam.pcName = "TaskAdc";
    stInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uwRet = LOS_TaskCreate(&TaskAdc_ID, &stInitParam);
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }
    //任务4
    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskLcd;
    stInitParam.usTaskPrio = TASK_PRIOR_Lcd;
    stInitParam.pcName = "TaskLcd";
    stInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uwRet = LOS_TaskCreate(&TaskLcd_ID, &stInitParam);
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }
    //任务5   
    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskUsart1;
    stInitParam.usTaskPrio = TASK_PRIOR_Usart1;
    stInitParam.pcName = "TaskUsart1";
    stInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uwRet = LOS_TaskCreate(&TaskUsart1_ID, &stInitParam);
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }
    //任务6
    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskUsart6;
    stInitParam.usTaskPrio = TASK_PRIOR_Usart6;
    stInitParam.pcName = "TaskUsart6";
    stInitParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    uwRet = LOS_TaskCreate(&TaskUsart6_ID, &stInitParam);
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }
    
    LOS_Start();
    return LOS_OK;
}

/***********************************任务1**************************************/
//故障指示灯及系统参数初始化
UINT32 TaskLed(void)
{
    static uint16_t relaytime = 0;
    while(1)
    {    
        if(relaytime <= 2000) relaytime++;
        if(relaytime >= 10){
            rtc_time_get();
            Transformation_usart_data();
        }
             
        variableWriteParaInit(); //首次判断EEPROM写初始值       
        
        UASRT6_TASK_Poll(); //读取温湿度参数--定时发送指令
        fan_ctl_handle();   //根据温度启动风机
        fan_fault_handle(); //风机故障判断
        
        if(Meter_run.reBootFlag == 1){
            Meter_run.reBootFlag = 0;
            __set_FAULTMASK(1);
            NVIC_SystemReset();
        }
        
        wdt_counter_reload(); //喂狗
        LOS_TaskDelay(100);
    }
}

/***********************************任务2**************************************/
//按键操作
UINT32 TaskKey(void)
{
    while(1)
    {
        Keyboard_pulic();
        LOS_TaskDelay(50);
    }
}

/***********************************任务3**************************************/
UINT32 TaskAdc(void)
{
    while(1)
    {
        LOS_SemPend(ADC1_Finish_Sem, LOS_WAIT_FOREVER);
        Adc_Rms_Conversion();
        LOS_TaskDelay(15);
    }
}

/***********************************任务4**************************************/
//显示
UINT32 TaskLcd(void)
{
    while(1)
    {
        led_main_update();
        LOS_TaskDelay(100);
    }
}

/***********************************任务5**************************************/
UINT32 TaskUsart1(void)
{
    while(1)
    {
        LOS_SemPend(Usart1_Recv_Finish_Sem, LOS_WAIT_FOREVER);
        USART1_TASK_PUBLIC();
    }
}

/***********************************任务6*************************************/
UINT32 TaskUsart6(void)
{
    while(1)
    {
        LOS_SemPend(Usart6_Recv_Finish_Sem, LOS_WAIT_FOREVER);
        USART6_TASK_PUBLIC();
    }
}
  /* add user code end 4 */
