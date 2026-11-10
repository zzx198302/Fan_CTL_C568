#ifndef __USER_HEAD_H
#define __USER_HEAD_H

#include "at32f403a_407_wk_config.h"

//Keyboard
void Keyboard_pulic(void);

//UserAdc
void Adc_Rms_Conversion(void);

//UserCommon
void BubbleSort(uint16_t *arr, uint16_t size);
unsigned short getCRC16(volatile uint8_t  *ptr ,uint8_t len);
uint8_t rs485ErrorHandle(enum RS485_ERROR_TYPE errType,
                         uint8_t txBuf[], uint8_t addr, uint8_t func);
void rs485TransmitPolling(usart_type * USARTx, uint8_t txBuf[], uint8_t txLen);
int8_t findComAddIndex( uint8_t comAdd);
void Transformation_usart_data(void);

//UserUsart1_(RS1)
void MX_USART1_UART_Init_Ex(void);
void HAL_INIT_USART1(void);
void USART1_IRQ_PB(void);
void TIMER6_IRQ_U1(void);
void USART1_TASK_PUBLIC(void);

//UserUsart6_(RS2)
void MX_USART6_UART_Init_Ex(void);
void HAL_INIT_USART6(void);
void USART6_IRQ_PB(void);
void TIMER6_IRQ_U6(void);
void USART6_Read_TempHum(uint8_t devAdd);
void USART6_TASK_PUBLIC(void);
void UASRT6_TASK_Poll(void);

//UserIO
void Relay_Ctl(uint8_t num, FAN_CTL fanctl);
void KG_7_Ctl(FAN_CTL fanctl);
void fan_ctl_handle(void);
void fan_fault_handle(void);

//UserEprom
void UserEpromInit(void);
void UserEpromWriteU8(uint16_t addr, uint8_t val);
uint8_t UserEpromReadU8(uint16_t addr);
void UserEpromWriteU16(uint16_t addr, uint16_t val);
uint16_t UserEpromReadU16(uint16_t addr);

//UserEventLog
void Save_EVENT_REC_E(uint16_t EVENT,uint16_t EVENT_ED);

#endif
