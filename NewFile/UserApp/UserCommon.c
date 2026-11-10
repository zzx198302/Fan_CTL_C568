/*************************************************************** 
 * @FilePath: \....\NewFile\UserApp\UserCommon.c
 * @Version: V1.0.0
 * @Author: ZhongZhanxiong
 * @Date: 2026-05-19 18:11:31
 * @LastEditors: ZhongZhanxiong
 * @LastEditTime: 2026-05-20 11:16:55
 * @Description: 
 * @Copyright (c) 2025 by PengYang, All Rights Reserved. 
 ***************************************************************/
#include "User_head.h"
#include <math.h>

/********************************************************************************
* Function Name  : void BubbleSort(uint16_t *arr, uint16_t size)
* Description    : 冒泡排序
* Input          : 数组、长度
* Output         : None
* Return         : None
* Author         ：
* Date           ：
*******************************************************************************/
void BubbleSort(uint16_t *arr, uint16_t size)
{
    int i, j, tmp;
    for (i = 0; i < size - 1; i++) {
        for (j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j+1]) {
                tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
        }
    }
}

/*******************************************************************************
函数名称：crc16校验
函数功能：crc16校验
函数输入：字节指针*ptr，数据长度len
函数返回：双字节crc
*******************************************************************************/
unsigned short getCRC16(volatile uint8_t  *ptr ,uint8_t len)
{
    uint8_t i;
    unsigned short crc = 0xFFFF;
    if(len==0)
    {
        len = 1;
    }
    while(len--)
    {
        crc ^= *ptr;
        for(i=0; i<8; i++)
        {
            if(crc&1)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
        ptr++;
    }
    return(crc);
}

/********************************************************************************
* Function Name  : uint8_t rs485ErrorHandle(enum RS485_ERROR_TYPE errType,
                         uint8_t txBuf[], uint8_t addr, uint8_t func)
* Description    : modbus通讯错误帧处理
* Input          :
* Output         :
* Return         :
* Author         ：
* Date           ：
*******************************************************************************/
uint8_t rs485ErrorHandle(enum RS485_ERROR_TYPE errType,
                         uint8_t txBuf[], uint8_t addr, uint8_t func)
{
    unsigned short crc16 = 0;
    uint8_t txCnt = 0;
    uint8_t result = 0;

    txBuf[txCnt++] = addr;
    txBuf[txCnt++] = (0x80 | func);

    switch(errType){
    case RS485_FUN_ERR:
        txBuf[txCnt++] = 0x01;
        break;
    case RS485_ADDR_ERR:
        txBuf[txCnt++] = 0x02;
        break;   
    case RS485_DATA_ERR:
        txBuf[txCnt++] = 0x03;
        break;
    case RS485_OTHER_ERR:
        txBuf[txCnt++] = 0x04;
        break;
    default:
        break;
    }

    crc16 = getCRC16(txBuf, txCnt);
    txBuf[txCnt++] = LOW_BYTE(crc16);
    txBuf[txCnt++] = HIGH_BYTE(crc16);
    result = txCnt;

    return  result;
}

/********************************************************************************
* Function Name  : void rs485TransmitPolling(usart_type * USARTx, uint8_t txBuf[], uint8_t txLen)
* Description    : 串口轮询方式发送
* Input          :
* Output         :
* Return         :
* Author         ：
* Date           ：
*******************************************************************************/
void rs485TransmitPolling(usart_type * USARTx, uint8_t txBuf[], uint8_t txLen)
{
    uint8_t txCnt = 0;

    while(txCnt < txLen)
    {
        while(usart_flag_get(USARTx, USART_TDBE_FLAG) == RESET);
        usart_data_transmit(USARTx, txBuf[txCnt++]);
    }
    //等待发送完毕
    while(usart_flag_get(USARTx, USART_TDC_FLAG) == RESET);
}

/********************************************************************************
* Function Name  : int8_t findComAddIndex( uint8_t comAdd)
* Description    : 查找通信地址索引
* Input          :
* Output         :
* Return         :
* Author         ：
* Date           ：
*******************************************************************************/
int8_t findComAddIndex( uint8_t comAdd)
{
    int8_t retVal = -1;

    for(int i=0; i<3; i++){
        if(comAdd == Meter_run.FanComAdd[i]){
            retVal = i;
            break;
        }     
    }
      
    return retVal;
}

//Modbus赋值
void usart_use_Transformation_int(uint16_t addr,int data)
{
    Meter_usart.usart_record[addr]=(data>>8)&0xff;
    Meter_usart.usart_record[addr+1]=data & 0xff;
}

void usart_use_Transformation_long(uint16_t addr,long data)
{
    Meter_usart.usart_record[addr]=(data>>24)&0xff;
    Meter_usart.usart_record[addr+1]=(data>>16)&0xff;
    Meter_usart.usart_record[addr+2]=(data>>8)&0xff;
    Meter_usart.usart_record[addr+3]=data & 0xff;
}

//更新通信数据缓存
void Transformation_usart_data(void)
{
    uint16_t a;
    uint16_t tmpData[10];
    uint16_t tmpLogData;
    
    a = 0;
    usart_use_Transformation_int(a+0, (int)(Meter_run.Tempert[0]*10));
    usart_use_Transformation_int(a+2, (int)(Meter_run.Humidity[0]*10));
    usart_use_Transformation_int(a+4, (int)(Meter_run.FanCur[0]*100));
    usart_use_Transformation_int(a+6, (int)(Meter_run.Tempert[1]*10));
    usart_use_Transformation_int(a+8, (int)(Meter_run.Humidity[1]*10));
    usart_use_Transformation_int(a+10,(int)(Meter_run.FanCur[1]*100));
    usart_use_Transformation_int(a+12,(int)(Meter_run.Tempert[2]*10));
    usart_use_Transformation_int(a+14,(int)(Meter_run.Humidity[2]*10));
    usart_use_Transformation_int(a+16,(int)(Meter_run.FanCur[2]*100));
    usart_use_Transformation_int(a+18,(int)(Meter_run.FanStatus[0]));
    usart_use_Transformation_int(a+20,(int)(Meter_run.FanStatus[1]));
    usart_use_Transformation_int(a+22,(int)(Meter_run.FanStatus[2]));
    usart_use_Transformation_int(a+24,(int)(Meter_run.TempAlarm[0]));
    usart_use_Transformation_int(a+26,(int)(Meter_run.TempAlarm[1]));
    usart_use_Transformation_int(a+28,(int)(Meter_run.TempAlarm[2]));
    
    a = 42;
    usart_use_Transformation_int(a+8, (int)(Meter_run.FanStopTemp));
    usart_use_Transformation_int(a+10,(int)(Meter_run.FanStartTemp));
    usart_use_Transformation_int(a+12,(int)(Meter_run.FanErrTemp));
    usart_use_Transformation_int(a+14,(int)(Meter_run.FanTimeInter));
    usart_use_Transformation_int(a+16,(int)(Meter_run.FanTimeRun));
    
    usart_use_Transformation_int(a+18,(int)(Meter_run.Com1Baud));
    usart_use_Transformation_int(a+20,(int)(Meter_run.Com1Parity));
    usart_use_Transformation_int(a+22,(int)(Meter_run.Com1Databit));
    usart_use_Transformation_int(a+24,(int)(Meter_run.Com1Stopbit));
    usart_use_Transformation_int(a+26,(int)(Meter_run.Com2Baud));
    usart_use_Transformation_int(a+28,(int)(Meter_run.Com2Parity));
    usart_use_Transformation_int(a+30,(int)(Meter_run.Com2Databit));
    usart_use_Transformation_int(a+32,(int)(Meter_run.Com2Stopbit));
    usart_use_Transformation_int(a+34,(int)(Meter_run.DevAdd));
    usart_use_Transformation_int(a+36,(int)(Meter_run.FanComAdd[0]));
    
    usart_use_Transformation_int(a+38,(int)(Meter_run.FanComAdd[1]));
    usart_use_Transformation_int(a+40,(int)(Meter_run.FanComAdd[2]));
    usart_use_Transformation_int(a+42,(int)(Meter_run.FanSetCur[0]));
    usart_use_Transformation_int(a+44,(int)(Meter_run.FanSetCur[1]));
    usart_use_Transformation_int(a+46,(int)(Meter_run.FanSetCur[2]));
    usart_use_Transformation_int(a+48,(int)(Meter_run.FanEnable[0]));
    usart_use_Transformation_int(a+50,(int)(Meter_run.FanEnable[1]));
    usart_use_Transformation_int(a+52,(int)(Meter_run.FanEnable[2]));
    usart_use_Transformation_int(a+54,(int)(Meter_run.DehumiLevel));
    
    a = 200;
    usart_use_Transformation_int(a+0, (int)(Meter_run.TaHSensorSele));
    
    a = 300;
    usart_use_Transformation_int(a+0, (int)(EVENT_LOG.E_Total_Num));
    usart_use_Transformation_int(a+2, (int)(EVENT_LOG.E_Present_Save_ad_bak));
    if((EVENT_LOG.E_Present_Save_ad_bak == 0) && (EVENT_LOG.E_Total_Num == 0))
    {
        tmpData[0] = 0;
        tmpData[1] = 0;
        tmpData[2] = 0;
        tmpData[3] = 0;
        tmpData[4] = 0;
        tmpData[5] = 0;
        tmpData[6] = 0;
        tmpData[7] = 0;
    }
    else if((EVENT_LOG.E_Present_Save_ad_bak == 0) && (EVENT_LOG.E_Total_Num == 100))
    {
        EVENT_LOG.E_Present_Save_ad_bak = 100;
        usart_use_Transformation_int(a+2, (int)(EVENT_LOG.E_Present_Save_ad_bak));
        tmpLogData = UserEpromReadU16(EE_EVENT_LOG_RECORD + 10*(EVENT_LOG.E_Present_Save_ad_bak-1)+0);
        tmpData[0] = (tmpLogData >> 8) & 0x00ff;
        tmpData[1] = (tmpLogData >> 0) & 0x00ff;
        tmpLogData = UserEpromReadU16(EE_EVENT_LOG_RECORD + 10*(EVENT_LOG.E_Present_Save_ad_bak-1)+2);
        tmpData[2] = (tmpLogData >> 8) & 0x00ff;
        tmpData[3] = (tmpLogData >> 0) & 0x00ff;
        tmpLogData = UserEpromReadU16(EE_EVENT_LOG_RECORD + 10*(EVENT_LOG.E_Present_Save_ad_bak-1)+4);
        tmpData[4] = (tmpLogData >> 8) & 0x00ff;
        tmpData[5] = (tmpLogData >> 0) & 0x00ff;
        tmpLogData = UserEpromReadU16(EE_EVENT_LOG_RECORD + 10*(EVENT_LOG.E_Present_Save_ad_bak-1)+6);
        tmpData[6] = (tmpLogData >> 0) & 0x00ff;
        tmpLogData = UserEpromReadU16(EE_EVENT_LOG_RECORD + 10*(EVENT_LOG.E_Present_Save_ad_bak-1)+8);
        tmpData[7] = (tmpLogData >> 0) & 0x00ff;
    }
    else
    {
        tmpLogData = UserEpromReadU16(EE_EVENT_LOG_RECORD + 10*(EVENT_LOG.E_Present_Save_ad_bak-1)+0);
        tmpData[0] = (tmpLogData >> 8) & 0x00ff;
        tmpData[1] = (tmpLogData >> 0) & 0x00ff;
        tmpLogData = UserEpromReadU16(EE_EVENT_LOG_RECORD + 10*(EVENT_LOG.E_Present_Save_ad_bak-1)+2);
        tmpData[2] = (tmpLogData >> 8) & 0x00ff;
        tmpData[3] = (tmpLogData >> 0) & 0x00ff;
        tmpLogData = UserEpromReadU16(EE_EVENT_LOG_RECORD + 10*(EVENT_LOG.E_Present_Save_ad_bak-1)+4);
        tmpData[4] = (tmpLogData >> 8) & 0x00ff;
        tmpData[5] = (tmpLogData >> 0) & 0x00ff;
        tmpLogData = UserEpromReadU16(EE_EVENT_LOG_RECORD + 10*(EVENT_LOG.E_Present_Save_ad_bak-1)+6);
        tmpData[6] = (tmpLogData >> 0) & 0x00ff;
        tmpLogData = UserEpromReadU16(EE_EVENT_LOG_RECORD + 10*(EVENT_LOG.E_Present_Save_ad_bak-1)+8);
        tmpData[7] = (tmpLogData >> 0) & 0x00ff;
    }
    usart_use_Transformation_int(a+4, (int)(tmpData[0]));
    usart_use_Transformation_int(a+6, (int)(tmpData[1]));
    usart_use_Transformation_int(a+8, (int)(tmpData[2]));
    usart_use_Transformation_int(a+10,(int)(tmpData[3]));
    usart_use_Transformation_int(a+12,(int)(tmpData[4]));
    usart_use_Transformation_int(a+14,(int)(tmpData[5]));
    usart_use_Transformation_int(a+16,(int)(tmpData[6]));
    usart_use_Transformation_int(a+18,(int)(tmpData[7]));

    
    a = 400;   
    usart_use_Transformation_int(a+0, (int)(Meter_run.HandAuto));
    usart_use_Transformation_int(a+2, (int)(Meter_run.FanCtl[0]));
    usart_use_Transformation_int(a+4, (int)(Meter_run.FanCtl[1]));
    usart_use_Transformation_int(a+6, (int)(Meter_run.FanCtl[2]));   
}




