/* add user code begin Header */
/**
  **************************************************************************
  * @file     at32f403a_407_int.c
  * @brief    main interrupt service routines.
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

/* includes ------------------------------------------------------------------*/
#include "at32f403a_407_int.h"
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

/* add user code end private define */

/* private macro -------------------------------------------------------------*/
/* add user code begin private macro */

/* add user code end private macro */

/* private variables ---------------------------------------------------------*/
/* add user code begin private variables */

/* add user code end private variables */

/* private function prototypes --------------------------------------------*/
/* add user code begin function prototypes */

/* add user code end function prototypes */

/* private user code ---------------------------------------------------------*/
/* add user code begin 0 */

/* add user code end 0 */

/* external variables ---------------------------------------------------------*/
/* add user code begin external variables */

/* add user code end external variables */

/**
  * @brief  this function handles nmi exception.
  * @param  none
  * @retval none
  */
void NMI_Handler(void)
{
  /* add user code begin NonMaskableInt_IRQ 0 */

  /* add user code end NonMaskableInt_IRQ 0 */

  /* add user code begin NonMaskableInt_IRQ 1 */

  /* add user code end NonMaskableInt_IRQ 1 */
}

/**
  * @brief  this function handles hard fault exception.
  * @param  none
  * @retval none
  */
void HardFault_Handler(void)
{
  /* add user code begin HardFault_IRQ 0 */

  /* add user code end HardFault_IRQ 0 */
  /* go to infinite loop when hard fault exception occurs */
  while (1)
  {
    /* add user code begin W1_HardFault_IRQ 0 */

    /* add user code end W1_HardFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles memory manage exception.
  * @param  none
  * @retval none
  */
void MemManage_Handler(void)
{
  /* add user code begin MemoryManagement_IRQ 0 */

  /* add user code end MemoryManagement_IRQ 0 */
  /* go to infinite loop when memory manage exception occurs */
  while (1)
  {
    /* add user code begin W1_MemoryManagement_IRQ 0 */

    /* add user code end W1_MemoryManagement_IRQ 0 */
  }
}

/**
  * @brief  this function handles bus fault exception.
  * @param  none
  * @retval none
  */
void BusFault_Handler(void)
{
  /* add user code begin BusFault_IRQ 0 */

  /* add user code end BusFault_IRQ 0 */
  /* go to infinite loop when bus fault exception occurs */
  while (1)
  {
    /* add user code begin W1_BusFault_IRQ 0 */

    /* add user code end W1_BusFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles usage fault exception.
  * @param  none
  * @retval none
  */
void UsageFault_Handler(void)
{
  /* add user code begin UsageFault_IRQ 0 */

  /* add user code end UsageFault_IRQ 0 */
  /* go to infinite loop when usage fault exception occurs */
  while (1)
  {
    /* add user code begin W1_UsageFault_IRQ 0 */

    /* add user code end W1_UsageFault_IRQ 0 */
  }
}

/**
  * @brief  this function handles svcall exception.
  * @param  none
  * @retval none
  */
void SVC_Handler(void)
{
  /* add user code begin SVCall_IRQ 0 */

  /* add user code end SVCall_IRQ 0 */
  /* add user code begin SVCall_IRQ 1 */

  /* add user code end SVCall_IRQ 1 */
}

/**
  * @brief  this function handles debug monitor exception.
  * @param  none
  * @retval none
  */
void DebugMon_Handler(void)
{
  /* add user code begin DebugMonitor_IRQ 0 */

  /* add user code end DebugMonitor_IRQ 0 */
  /* add user code begin DebugMonitor_IRQ 1 */

  /* add user code end DebugMonitor_IRQ 1 */
}

/**
  * @brief  this function handles pendsv_handler exception.
  * @param  none
  * @retval none
  */
//void PendSV_Handler(void)
//{
  /* add user code begin PendSV_IRQ 0 */
//
//  /* add user code end PendSV_IRQ 0 */
  /* add user code begin PendSV_IRQ 1 */
//
//  /* add user code end PendSV_IRQ 1 */
//}

/**
  * @brief  this function handles systick handler.
  * @param  none
  * @retval none
  */
//void SysTick_Handler(void)
//{
  /* add user code begin SysTick_IRQ 0 */
//
//  /* add user code end SysTick_IRQ 0 */


  /* add user code begin SysTick_IRQ 1 */
//
//  /* add user code end SysTick_IRQ 1 */
//}

/**
  * @brief  this function handles DMA1 Channel 1 handler.
  * @param  none
  * @retval none
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* add user code begin DMA1_Channel1_IRQ 0 */
  if(dma_interrupt_flag_get(DMA1_FDT1_FLAG) != RESET)
  {   
    LOS_SemPost(ADC1_Finish_Sem);
    /*
    dma_channel_enable(DMA1_CHANNEL1, FALSE);
    tmr_channel_enable(TMR2, TMR_SELECT_CHANNEL_2, FALSE);
    tmr_output_enable(TMR2, FALSE);
    dma_interrupt_enable(DMA1_CHANNEL1, DMA_FDT_INT, FALSE);
    */
    /*
    dma_channel_enable(DMA1_CHANNEL1, FALSE);
    tmr_counter_enable(TMR2, FALSE);
    adc_dma_mode_enable(ADC1, FALSE);
    dma_interrupt_enable(DMA1_CHANNEL1, DMA_FDT_INT, FALSE);
    */
    
    dma_flag_clear(DMA1_FDT1_FLAG);
  }
  /* add user code end DMA1_Channel1_IRQ 0 */
  /* add user code begin DMA1_Channel1_IRQ 1 */

  /* add user code end DMA1_Channel1_IRQ 1 */
}

/**
  * @brief  this function handles USART1 handler.
  * @param  none
  * @retval none
  */
void USART1_IRQHandler(void)
{
  /* add user code begin USART1_IRQ 0 */
  if(usart_interrupt_flag_get(USART1, USART_RDBF_FLAG) != RESET)
  {
      USART1_IRQ_PB();
  }
  /* add user code end USART1_IRQ 0 */
  /* add user code begin USART1_IRQ 1 */

  /* add user code end USART1_IRQ 1 */
}

/**
  * @brief  this function handles TMR6 handler.
  * @param  none
  * @retval none
  */
void TMR6_GLOBAL_IRQHandler(void)
{
  /* add user code begin TMR6_GLOBAL_IRQ 0 */
  if(tmr_interrupt_flag_get(TMR6, TMR_OVF_FLAG) != RESET)
  {
      tmr_flag_clear(TMR6, TMR_OVF_FLAG);
      TIMER6_IRQ_U1();
      TIMER6_IRQ_U6();
  }
  /* add user code end TMR6_GLOBAL_IRQ 0 */


  /* add user code begin TMR6_GLOBAL_IRQ 1 */

  /* add user code end TMR6_GLOBAL_IRQ 1 */
}

/**
  * @brief  this function handles TMR7 handler.
  * @param  none
  * @retval none
  */
void TMR7_GLOBAL_IRQHandler(void)
{
  /* add user code begin TMR7_GLOBAL_IRQ 0 */
  static uint16_t timeCnt = 0;
  if(tmr_interrupt_flag_get(TMR7, TMR_OVF_FLAG) != RESET)
  {
      tmr_flag_clear(TMR7, TMR_OVF_FLAG);
      timeCnt++;
      if(timeCnt >= 10){
          timeCnt = 0;
          led_dsp_inc_timer();
          
          for(int i=0; i<3; i++){
              if(Meter_run.FanCtl[i] == FAN_CTL_OPEN){
                  Meter_run.TimeOpenCount[i] = 0;
              }
              else if(Meter_run.FanCtl[i] == FAN_CTL_CLOSE){
                  Meter_run.TimeOpenCount[i]++;
              }
              
              if(1 == Meter_run.TimeOpenFlag[i]){
                  Meter_run.TimeRunCount[i]++;
              }
              else{
                  Meter_run.TimeRunCount[i] = 0;
              }
          }
      }
  }
  /* add user code end TMR7_GLOBAL_IRQ 0 */


  /* add user code begin TMR7_GLOBAL_IRQ 1 */

  /* add user code end TMR7_GLOBAL_IRQ 1 */
}

/**
  * @brief  this function handles USART6 handler.
  * @param  none
  * @retval none
  */
void USART6_IRQHandler(void)
{
  /* add user code begin USART6_IRQ 0 */
  if(usart_interrupt_flag_get(USART6, USART_RDBF_FLAG) != RESET)
  {
      USART6_IRQ_PB();
  }
  /* add user code end USART6_IRQ 0 */
  /* add user code begin USART6_IRQ 1 */

  /* add user code end USART6_IRQ 1 */
}

/* add user code begin 1 */

/* add user code end 1 */
