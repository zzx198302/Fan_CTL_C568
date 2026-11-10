/* add user code begin Header */
/**
  **************************************************************************
  * @file     at32f403a_407_wk_config.h
  * @brief    header file of work bench config
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

/* define to prevent recursive inclusion -----------------------------------*/
#ifndef __AT32F403A_407_WK_CONFIG_H
#define __AT32F403A_407_WK_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* includes -----------------------------------------------------------------------*/
#include "stdio.h"
#include "at32f403a_407.h"

/* private includes -------------------------------------------------------------*/
/* add user code begin private includes */
#include "los_task.h"
#include "los_hwi.h"
#include "los_sem.h"
#include "wk_system.h"
#include "UserVariable.h"
#include "M24C64.h"
/* add user code end private includes */

/* exported types -------------------------------------------------------------*/
/* add user code begin exported types */

/* add user code end exported types */

/* exported constants --------------------------------------------------------*/
/* add user code begin exported constants */

/* add user code end exported constants */

/* exported macro ------------------------------------------------------------*/
/* add user code begin exported macro */

/* add user code end exported macro */

/* add user code begin dma define */
/* user can only modify the dma define value */
#define DMA1_CHANNEL1_BUFFER_SIZE   (4*ADC_POINT)
#define DMA1_CHANNEL1_MEMORY_BASE_ADDR   ((uint32_t)(Meter_ADC.Adc1_Value))
//#define DMA1_CHANNEL1_PERIPHERAL_BASE_ADDR  0

//#define DMA1_CHANNEL2_BUFFER_SIZE   0
//#define DMA1_CHANNEL2_MEMORY_BASE_ADDR   0
//#define DMA1_CHANNEL2_PERIPHERAL_BASE_ADDR   0

//#define DMA1_CHANNEL3_BUFFER_SIZE   0
//#define DMA1_CHANNEL3_MEMORY_BASE_ADDR   0
//#define DMA1_CHANNEL3_PERIPHERAL_BASE_ADDR   0

//#define DMA1_CHANNEL4_BUFFER_SIZE   0
//#define DMA1_CHANNEL4_MEMORY_BASE_ADDR   0
//#define DMA1_CHANNEL4_PERIPHERAL_BASE_ADDR   0

//#define DMA1_CHANNEL5_BUFFER_SIZE   0
//#define DMA1_CHANNEL5_MEMORY_BASE_ADDR   0
//#define DMA1_CHANNEL5_PERIPHERAL_BASE_ADDR   0

//#define DMA1_CHANNEL6_BUFFER_SIZE   0
//#define DMA1_CHANNEL6_MEMORY_BASE_ADDR   0
//#define DMA1_CHANNEL6_PERIPHERAL_BASE_ADDR   0

//#define DMA1_CHANNEL7_BUFFER_SIZE   0
//#define DMA1_CHANNEL7_MEMORY_BASE_ADDR   0
//#define DMA1_CHANNEL7_PERIPHERAL_BASE_ADDR   0

//#define DMA2_CHANNEL1_BUFFER_SIZE   0
//#define DMA2_CHANNEL1_MEMORY_BASE_ADDR   0
//#define DMA2_CHANNEL1_PERIPHERAL_BASE_ADDR   0

//#define DMA2_CHANNEL2_BUFFER_SIZE   0
//#define DMA2_CHANNEL2_MEMORY_BASE_ADDR   0
//#define DMA2_CHANNEL2_PERIPHERAL_BASE_ADDR   0

//#define DMA2_CHANNEL3_BUFFER_SIZE   0
//#define DMA2_CHANNEL3_MEMORY_BASE_ADDR   0
//#define DMA2_CHANNEL3_PERIPHERAL_BASE_ADDR   0

//#define DMA2_CHANNEL4_BUFFER_SIZE   0
//#define DMA2_CHANNEL4_MEMORY_BASE_ADDR   0
//#define DMA2_CHANNEL4_PERIPHERAL_BASE_ADDR   0

//#define DMA2_CHANNEL5_BUFFER_SIZE   0
//#define DMA2_CHANNEL5_MEMORY_BASE_ADDR   0
//#define DMA2_CHANNEL5_PERIPHERAL_BASE_ADDR   0

//#define DMA2_CHANNEL6_BUFFER_SIZE   0
//#define DMA2_CHANNEL6_MEMORY_BASE_ADDR   0
//#define DMA2_CHANNEL6_PERIPHERAL_BASE_ADDR   0

//#define DMA2_CHANNEL7_BUFFER_SIZE   0
//#define DMA2_CHANNEL7_MEMORY_BASE_ADDR   0
//#define DMA2_CHANNEL7_PERIPHERAL_BASE_ADDR   0
/* add user code end dma define */

/* Private defines -------------------------------------------------------------*/
#define KG1_PIN    GPIO_PINS_0
#define KG1_GPIO_PORT    GPIOC
#define KG2_PIN    GPIO_PINS_1
#define KG2_GPIO_PORT    GPIOC
#define KG3_PIN    GPIO_PINS_2
#define KG3_GPIO_PORT    GPIOC
#define KG4_PIN    GPIO_PINS_3
#define KG4_GPIO_PORT    GPIOC
#define ADC1_IF1_PIN    GPIO_PINS_4
#define ADC1_IF1_GPIO_PORT    GPIOA
#define ADC1_IF2_PIN    GPIO_PINS_5
#define ADC1_IF2_GPIO_PORT    GPIOA
#define ADC1_IF3_PIN    GPIO_PINS_6
#define ADC1_IF3_GPIO_PORT    GPIOA
#define KG5_PIN    GPIO_PINS_7
#define KG5_GPIO_PORT    GPIOA
#define KG6_PIN    GPIO_PINS_4
#define KG6_GPIO_PORT    GPIOC
#define KG7_PIN    GPIO_PINS_5
#define KG7_GPIO_PORT    GPIOC
#define KG8_PIN    GPIO_PINS_0
#define KG8_GPIO_PORT    GPIOB
#define EE_WP_PIN    GPIO_PINS_2
#define EE_WP_GPIO_PORT    GPIOB
#define EE_SCL_PIN    GPIO_PINS_10
#define EE_SCL_GPIO_PORT    GPIOB
#define EE_SDA_PIN    GPIO_PINS_11
#define EE_SDA_GPIO_PORT    GPIOB
#define USART6_RT_PIN    GPIO_PINS_8
#define USART6_RT_GPIO_PORT    GPIOC
#define USART1_RT_PIN    GPIO_PINS_11
#define USART1_RT_GPIO_PORT    GPIOA
#define SW7_PIN    GPIO_PINS_15
#define SW7_GPIO_PORT    GPIOA
#define SW6_PIN    GPIO_PINS_10
#define SW6_GPIO_PORT    GPIOC
#define SW5_PIN    GPIO_PINS_11
#define SW5_GPIO_PORT    GPIOC
#define SW4_PIN    GPIO_PINS_12
#define SW4_GPIO_PORT    GPIOC
#define SW3_PIN    GPIO_PINS_2
#define SW3_GPIO_PORT    GPIOD
#define SW2_PIN    GPIO_PINS_3
#define SW2_GPIO_PORT    GPIOB
#define SW1_PIN    GPIO_PINS_4
#define SW1_GPIO_PORT    GPIOB
#define LED_STB_PIN    GPIO_PINS_5
#define LED_STB_GPIO_PORT    GPIOB
#define LED_CLK_PIN    GPIO_PINS_6
#define LED_CLK_GPIO_PORT    GPIOB
#define LED_DIO_PIN    GPIO_PINS_7
#define LED_DIO_GPIO_PORT    GPIOB
#define SW8_PIN    GPIO_PINS_8
#define SW8_GPIO_PORT    GPIOB

/* exported functions ------------------------------------------------------- */
  /* system clock config. */
  void wk_system_clock_config(void);

  /* config periph clock. */
  void wk_periph_clock_config(void);

  /* nvic config. */
  void wk_nvic_config(void);

/* add user code begin exported functions */
extern uint32_t ADC1_Finish_Sem;  
extern uint32_t Usart1_Recv_Finish_Sem;
extern uint32_t Usart6_Recv_Finish_Sem;
  
#define KEY_SET         gpio_input_data_bit_read(SW1_GPIO_PORT, SW1_PIN)
#define KEY_DOWN        gpio_input_data_bit_read(SW2_GPIO_PORT, SW2_PIN)
#define KEY_UP          gpio_input_data_bit_read(SW3_GPIO_PORT, SW3_PIN)
#define KEY_ENT         gpio_input_data_bit_read(SW4_GPIO_PORT, SW4_PIN)
#define KEY_HAND_AUTO   gpio_input_data_bit_read(SW5_GPIO_PORT, SW5_PIN)
#define KEY_FAN1_ST     gpio_input_data_bit_read(SW6_GPIO_PORT, SW6_PIN)
#define KEY_FAN2_ST     gpio_input_data_bit_read(SW7_GPIO_PORT, SW7_PIN)
#define KEY_FAN3_ST     gpio_input_data_bit_read(SW8_GPIO_PORT, SW8_PIN)


#define USART1_RT_in    gpio_bits_reset(USART1_RT_GPIO_PORT, USART1_RT_PIN) //接收状态
#define USART1_RT_out   gpio_bits_set(USART1_RT_GPIO_PORT,   USART1_RT_PIN) //发送状态
#define USART6_RT_in    gpio_bits_reset(USART6_RT_GPIO_PORT, USART6_RT_PIN) //接收状态
#define USART6_RT_out   gpio_bits_set(USART6_RT_GPIO_PORT,   USART6_RT_PIN) //发送状态  
/* add user code end exported functions */

#ifdef __cplusplus
}
#endif

#endif
