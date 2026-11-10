/*************************************************************** 
 * @FilePath: \....\NewFile\UserApp\UserVariable.c
 * @Version: V1.0.0
 * @Author: ZhongZhanxiong
 * @Date: 2026-05-19 18:11:31
 * @LastEditors: ZhongZhanxiong
 * @LastEditTime: 2026-05-20 11:16:55
 * @Description: 应用程序变量定义文件，以及系统参数用户掉电数据处理文件
 * @Copyright (c) 2025 by PengYang, All Rights Reserved. 
 ***************************************************************/
#include "UserVariable.h"
#include "wk_wdt.h"
#include "User_head.h"

Meter_run_value_type Meter_run;
Meter_Surface_type Meter_Surface;
Meter_ADC_Check_type Meter_ADC;
Meter_usart_value_type Meter_usart;
EVENT_LOG_type EVENT_LOG;
/*宏定义*/

void variableReadParaInit(void)
{
    uint16_t isMemFlag = 0;
    
    isMemFlag = UserEpromReadU16(EE_IS_MEMORY);
    if(0x5555 != isMemFlag)
    {
        Meter_run.FanStopTemp = 45;
        Meter_run.FanStartTemp = 50;
        Meter_run.FanErrTemp = 65;
        Meter_run.DehumiLevel = 70;
        
        Meter_run.FanTimeInter = 12;
        Meter_run.FanTimeRun = 3;
        Meter_run.TaHSensorSele = 0;
        
        Meter_run.FanComAdd[0] = 1;
        Meter_run.FanComAdd[1] = 2;
        Meter_run.FanComAdd[2] = 3;
        
        Meter_run.Com1Baud = 2;
        Meter_run.Com1Parity = 0;
        Meter_run.Com1Databit = 0;
        Meter_run.Com1Stopbit = 0;
        
        Meter_run.Com2Baud = 1;
        Meter_run.Com2Parity = 0;
        Meter_run.Com2Databit = 0;
        Meter_run.Com2Stopbit = 0;
        
        Meter_run.FanSetCur[0] = 100;
        Meter_run.FanEnable[0] = 0;
        Meter_run.FanSetCur[1] = 100;
        Meter_run.FanEnable[1] = 0;
        Meter_run.FanSetCur[2] = 100;
        Meter_run.FanEnable[2] = 0;
        
        Meter_run.DevAdd = 1;
        Meter_usart.Usart1_addr = Meter_run.DevAdd;
        
        Meter_ADC.Fan1_Cur_Adc_base_Check = 2045;
        Meter_ADC.Fan2_Cur_Adc_base_Check = 2045;
        Meter_ADC.Fan3_Cur_Adc_base_Check = 2045;
        Meter_ADC.Fan1_Cur_Adc_full_Check = 1200;
        Meter_ADC.Fan2_Cur_Adc_full_Check = 1200;
        Meter_ADC.Fan3_Cur_Adc_full_Check = 1200;
    }
    else
    {        
        Meter_run.FanStopTemp = UserEpromReadU16(EE_FAN_STOPTEMP);     
        if(Meter_run.FanStopTemp > 200){
           Meter_run.FanStopTemp = 45;
           UserEpromWriteU16(EE_FAN_STOPTEMP, Meter_run.FanStopTemp);
        }
        Meter_run.FanStartTemp = UserEpromReadU16(EE_FAN_STARTTEMP);   
        if(Meter_run.FanStartTemp > 200){
           Meter_run.FanStartTemp = 50;
           UserEpromWriteU16(EE_FAN_STARTTEMP, Meter_run.FanStartTemp);
        }
        Meter_run.FanErrTemp = UserEpromReadU16(EE_FAN_ERRTEMP);
        if(Meter_run.FanErrTemp > 200){
           Meter_run.FanErrTemp = 65;
           UserEpromWriteU16(EE_FAN_ERRTEMP, Meter_run.FanErrTemp);
        }
        //增加判断
        if(Meter_run.FanStopTemp >= Meter_run.FanStartTemp){
          Meter_run.FanStartTemp = Meter_run.FanStopTemp+5;
          UserEpromWriteU16(EE_FAN_STARTTEMP, Meter_run.FanStartTemp);
        }
        if(Meter_run.FanStartTemp >= Meter_run.FanErrTemp){
          Meter_run.FanErrTemp = Meter_run.FanStartTemp+5;
          UserEpromWriteU16(EE_FAN_ERRTEMP, Meter_run.FanErrTemp);
        }
        
        Meter_run.DehumiLevel = UserEpromReadU16(EE_DEHUMI_LEVEL);
        if(Meter_run.DehumiLevel > 200){
           Meter_run.DehumiLevel = 70;
           UserEpromWriteU16(EE_DEHUMI_LEVEL, Meter_run.DehumiLevel);
        }
        if(Meter_run.DehumiLevel < 6){
           Meter_run.DehumiLevel = 6;
           UserEpromWriteU16(EE_DEHUMI_LEVEL, Meter_run.DehumiLevel);
        }
        
        Meter_run.FanTimeInter = UserEpromReadU16(EE_FAN_TIME_INTER);  
        if(Meter_run.FanTimeInter > 300){
           Meter_run.FanTimeInter = 12;
           UserEpromWriteU16(EE_FAN_TIME_INTER, Meter_run.FanTimeInter);
        }
        Meter_run.FanTimeRun =  UserEpromReadU16(EE_FAN_TIME_RUN);  
        if(Meter_run.FanTimeRun > 100){
           Meter_run.FanTimeRun = 3;
           UserEpromWriteU16(EE_FAN_TIME_RUN, Meter_run.FanTimeRun);
        }
        Meter_run.TaHSensorSele = UserEpromReadU16(EE_TAH_SENSOR_SELE); 
        if(Meter_run.TaHSensorSele > 2){
           Meter_run.TaHSensorSele = 0;
           UserEpromWriteU16(EE_TAH_SENSOR_SELE, Meter_run.TaHSensorSele);
        }
       
        Meter_run.FanComAdd[0] =  UserEpromReadU8(EE_COM_FAN1_ADD);
        if(Meter_run.FanComAdd[0] > 255){         
           Meter_run.FanComAdd[0] = 0;
           UserEpromWriteU8(EE_COM_FAN1_ADD, Meter_run.FanComAdd[0]);
        }
        Meter_run.FanComAdd[1] =  UserEpromReadU8(EE_COM_FAN2_ADD);
        if(Meter_run.FanComAdd[1] > 255){
           Meter_run.FanComAdd[1] = 0;
           UserEpromWriteU8(EE_COM_FAN2_ADD, Meter_run.FanComAdd[1]);
        }
        Meter_run.FanComAdd[2] =  UserEpromReadU8(EE_COM_FAN3_ADD);
        if(Meter_run.FanComAdd[2] > 255){
           Meter_run.FanComAdd[2] = 0;
           UserEpromWriteU8(EE_COM_FAN3_ADD, Meter_run.FanComAdd[2]);
        }
        
        Meter_run.Com1Baud = UserEpromReadU8(EE_COM_1_BAUD);
        if(Meter_run.Com1Baud > 6){
           Meter_run.Com1Baud = 2;
           UserEpromWriteU8(EE_COM_1_BAUD, Meter_run.Com1Baud);
        }
        Meter_run.Com1Parity = UserEpromReadU8(EE_COM_1_PARITY);
        if(Meter_run.Com1Parity > 2){
           Meter_run.Com1Parity = 0;
           UserEpromWriteU8(EE_COM_1_PARITY, Meter_run.Com1Parity);
        }
        Meter_run.Com1Databit = UserEpromReadU8(EE_COM_1_DATABIT);
        if(Meter_run.Com1Databit > 1){
           Meter_run.Com1Databit = 0;
           UserEpromWriteU8(EE_COM_1_DATABIT, Meter_run.Com1Databit);
        }
        Meter_run.Com1Stopbit = UserEpromReadU8(EE_COM_1_STOPBIT);
        if(Meter_run.Com1Stopbit > 3){
           Meter_run.Com1Stopbit = 0;
           UserEpromWriteU8(EE_COM_1_STOPBIT, Meter_run.Com1Stopbit);
        }
        
        Meter_run.Com2Baud = UserEpromReadU8(EE_COM_2_BAUD);
        if(Meter_run.Com2Baud > 6){
           Meter_run.Com2Baud = 2;
           UserEpromWriteU8(EE_COM_2_BAUD, Meter_run.Com2Baud);
        }
        Meter_run.Com2Parity = UserEpromReadU8(EE_COM_2_PARITY);
        if(Meter_run.Com2Parity > 2){
           Meter_run.Com2Parity = 0;
           UserEpromWriteU8(EE_COM_2_PARITY, Meter_run.Com2Parity);
        }
        Meter_run.Com2Databit = UserEpromReadU8(EE_COM_2_DATABIT);
        if(Meter_run.Com2Databit > 1){
           Meter_run.Com2Databit = 0;
           UserEpromWriteU8(EE_COM_2_DATABIT, Meter_run.Com2Databit);
        }
        Meter_run.Com2Stopbit = UserEpromReadU8(EE_COM_2_STOPBIT);
        if(Meter_run.Com2Stopbit > 3){
           Meter_run.Com2Stopbit = 0;
           UserEpromWriteU8(EE_COM_2_STOPBIT, Meter_run.Com2Stopbit);
        }
        
        Meter_run.FanSetCur[0] = UserEpromReadU16(EE_FAN_1_CUR);   
        if(Meter_run.FanSetCur[0] > 1000){
           Meter_run.FanSetCur[0] = 100;
           UserEpromWriteU16(EE_FAN_1_CUR, Meter_run.FanSetCur[0]);
        }
        Meter_run.FanEnable[0] = UserEpromReadU16(EE_FAN_1_ENABLE); 
        if(Meter_run.FanEnable[0] > 1){
           Meter_run.FanEnable[0] = 0;
           UserEpromWriteU16(EE_FAN_1_ENABLE, Meter_run.FanEnable[0]);
        }
        Meter_run.FanSetCur[1] = UserEpromReadU16(EE_FAN_2_CUR);
        if(Meter_run.FanSetCur[1] > 1000){
           Meter_run.FanSetCur[1] = 100;
           UserEpromWriteU16(EE_FAN_2_CUR, Meter_run.FanSetCur[1]);
        }
        Meter_run.FanEnable[1] = UserEpromReadU16(EE_FAN_2_ENABLE);
        if(Meter_run.FanEnable[1] > 1){
           Meter_run.FanEnable[1] = 0;
           UserEpromWriteU16(EE_FAN_2_ENABLE, Meter_run.FanEnable[1]);
        }
        Meter_run.FanSetCur[2] = UserEpromReadU16(EE_FAN_3_CUR);
        if(Meter_run.FanSetCur[2] > 1000){
           Meter_run.FanSetCur[2] = 100;
           UserEpromWriteU16(EE_FAN_3_CUR, Meter_run.FanSetCur[2]);
        }
        Meter_run.FanEnable[2] = UserEpromReadU16(EE_FAN_3_ENABLE);
        if(Meter_run.FanEnable[2] > 1){
           Meter_run.FanEnable[2] = 0;
           UserEpromWriteU16(EE_FAN_3_ENABLE, Meter_run.FanEnable[2]);
        }
        
        Meter_run.DevAdd = UserEpromReadU8(EE_COM_DEV_ADD);
        if(Meter_run.DevAdd >= 255){
           Meter_run.DevAdd = 1;
           UserEpromWriteU8(EE_COM_DEV_ADD, Meter_run.DevAdd);
        }
        Meter_usart.Usart1_addr = Meter_run.DevAdd;
        
        Meter_ADC.Fan1_Cur_Adc_base_Check = UserEpromReadU16(EE_FAN_1_CHECK_BASE); 
        if( Meter_ADC.Fan1_Cur_Adc_base_Check >= 5000){
            Meter_ADC.Fan1_Cur_Adc_base_Check = 2045;
            UserEpromWriteU16(EE_FAN_1_CHECK_BASE, Meter_ADC.Fan1_Cur_Adc_base_Check);
        }
        Meter_ADC.Fan2_Cur_Adc_base_Check = UserEpromReadU16(EE_FAN_2_CHECK_BASE);  
        if( Meter_ADC.Fan2_Cur_Adc_base_Check >= 5000){
            Meter_ADC.Fan2_Cur_Adc_base_Check = 2045;
            UserEpromWriteU16(EE_FAN_2_CHECK_BASE, Meter_ADC.Fan2_Cur_Adc_base_Check);
        }
        Meter_ADC.Fan3_Cur_Adc_base_Check = UserEpromReadU16(EE_FAN_3_CHECK_BASE);  
        if( Meter_ADC.Fan3_Cur_Adc_base_Check >= 5000){
            Meter_ADC.Fan3_Cur_Adc_base_Check = 2045;
            UserEpromWriteU16(EE_FAN_3_CHECK_BASE, Meter_ADC.Fan3_Cur_Adc_base_Check);
        }
        Meter_ADC.Fan1_Cur_Adc_full_Check = UserEpromReadU16(EE_FAN_1_CHECK_FULL); 
        if(Meter_ADC.Fan1_Cur_Adc_full_Check >= 5000){
            Meter_ADC.Fan1_Cur_Adc_full_Check = 1200;
            UserEpromWriteU16(EE_FAN_1_CHECK_FULL, Meter_ADC.Fan1_Cur_Adc_full_Check);
        }
        Meter_ADC.Fan2_Cur_Adc_full_Check = UserEpromReadU16(EE_FAN_2_CHECK_FULL); 
        if(Meter_ADC.Fan2_Cur_Adc_full_Check >= 5000){
            Meter_ADC.Fan2_Cur_Adc_full_Check = 1200;
            UserEpromWriteU16(EE_FAN_2_CHECK_FULL, Meter_ADC.Fan2_Cur_Adc_full_Check);
        }
        Meter_ADC.Fan3_Cur_Adc_full_Check = UserEpromReadU16(EE_FAN_3_CHECK_FULL); 
        if(Meter_ADC.Fan3_Cur_Adc_full_Check >= 5000){
            Meter_ADC.Fan3_Cur_Adc_full_Check = 1200;
            UserEpromWriteU16(EE_FAN_3_CHECK_FULL, Meter_ADC.Fan3_Cur_Adc_full_Check);
        }              
    }  
}

void variableWriteParaInit(void)
{
    uint16_t isMemFlag = 0;
    
    isMemFlag = UserEpromReadU16(EE_IS_MEMORY);
    if(0x5555 != isMemFlag)
    {
        UserEpromWriteU16(EE_IS_MEMORY, 0x5555);
        UserEpromWriteU16(EE_FAN_STOPTEMP, Meter_run.FanStopTemp); 
        UserEpromWriteU16(EE_FAN_STARTTEMP, Meter_run.FanStartTemp);        
        UserEpromWriteU16(EE_FAN_ERRTEMP, Meter_run.FanErrTemp);
        UserEpromWriteU16(EE_DEHUMI_LEVEL, Meter_run.DehumiLevel);
        
        UserEpromWriteU16(EE_FAN_TIME_INTER, Meter_run.FanTimeInter); 
        UserEpromWriteU16(EE_FAN_TIME_RUN, Meter_run.FanTimeRun);    
        UserEpromWriteU16(EE_TAH_SENSOR_SELE, Meter_run.TaHSensorSele);  
       
        UserEpromWriteU8(EE_COM_FAN1_ADD, Meter_run.FanComAdd[0]);
        UserEpromWriteU8(EE_COM_FAN2_ADD, Meter_run.FanComAdd[1]);
        UserEpromWriteU8(EE_COM_FAN3_ADD, Meter_run.FanComAdd[2]);
        
        UserEpromWriteU8(EE_COM_1_BAUD, Meter_run.Com1Baud);
        UserEpromWriteU8(EE_COM_1_PARITY, Meter_run.Com1Parity);
        UserEpromWriteU8(EE_COM_1_DATABIT, Meter_run.Com1Databit);
        UserEpromWriteU8(EE_COM_1_STOPBIT, Meter_run.Com1Stopbit);
        
        UserEpromWriteU8(EE_COM_2_BAUD, Meter_run.Com2Baud);
        UserEpromWriteU8(EE_COM_2_PARITY, Meter_run.Com2Parity);
        UserEpromWriteU8(EE_COM_2_DATABIT, Meter_run.Com2Databit);
        UserEpromWriteU8(EE_COM_2_STOPBIT, Meter_run.Com2Stopbit);
        
        UserEpromWriteU16(EE_FAN_1_CUR, Meter_run.FanSetCur[0]);       
        UserEpromWriteU16(EE_FAN_1_ENABLE, Meter_run.FanEnable[0]); 
        UserEpromWriteU16(EE_FAN_2_CUR, Meter_run.FanSetCur[1]);
        UserEpromWriteU16(EE_FAN_2_ENABLE, Meter_run.FanEnable[1]);
        UserEpromWriteU16(EE_FAN_3_CUR, Meter_run.FanSetCur[2]);
        UserEpromWriteU16(EE_FAN_3_ENABLE, Meter_run.FanEnable[2]);
        
        UserEpromWriteU8(EE_COM_DEV_ADD, Meter_run.DevAdd);
        Meter_usart.Usart1_addr = Meter_run.DevAdd;
        
        UserEpromWriteU16(EE_FAN_1_CHECK_BASE, Meter_ADC.Fan1_Cur_Adc_base_Check);       
        UserEpromWriteU16(EE_FAN_2_CHECK_BASE, Meter_ADC.Fan2_Cur_Adc_base_Check); 
        UserEpromWriteU16(EE_FAN_3_CHECK_BASE, Meter_ADC.Fan3_Cur_Adc_base_Check);
        UserEpromWriteU16(EE_FAN_1_CHECK_FULL, Meter_ADC.Fan1_Cur_Adc_full_Check);
        UserEpromWriteU16(EE_FAN_2_CHECK_FULL, Meter_ADC.Fan2_Cur_Adc_full_Check);
        UserEpromWriteU16(EE_FAN_3_CHECK_FULL, Meter_ADC.Fan3_Cur_Adc_full_Check);
    }
}

void EventLogReadInit(void)
{
    /** 故障记录 **/
    EVENT_LOG.E_Present_Save_ad = UserEpromReadU16(EE_EVENT_LOG_POS);
    if(EVENT_LOG.E_Present_Save_ad >= 100)
    {
        EVENT_LOG.E_Present_Save_ad = 0;
        UserEpromWriteU16(EE_EVENT_LOG_POS, EVENT_LOG.E_Present_Save_ad);
    }
    EVENT_LOG.E_Present_Save_ad_bak = EVENT_LOG.E_Present_Save_ad;
    EVENT_LOG.E_Total_Num = UserEpromReadU16(EE_EVENT_LOG_TOTALNUM);
    if(EVENT_LOG.E_Total_Num > 100)
    {
        EVENT_LOG.E_Total_Num = 0;
        UserEpromWriteU16(EE_EVENT_LOG_TOTALNUM, EVENT_LOG.E_Total_Num);
    }
}

void EventLogClear(void)
{
    //故障记录
    EVENT_LOG.E_Present_Save_ad  = 0;
    UserEpromWriteU16(EE_EVENT_LOG_POS, EVENT_LOG.E_Present_Save_ad);
    EVENT_LOG.E_Present_Save_ad_bak = EVENT_LOG.E_Present_Save_ad;
    EVENT_LOG.E_Total_Num = 0;
    UserEpromWriteU16(EE_EVENT_LOG_TOTALNUM, EVENT_LOG.E_Total_Num);
}

void WrRelayStatusLog(uint8_t num, FAN_CTL fanctl)
{
      UserEpromWriteU16(EE_RELAY_1_STATU+num*2, fanctl);
}

void WrRelayCtlStatusLog(uint8_t num, FAN_CTL fanctl)
{
      UserEpromWriteU16(EE_RELAY_1_C_STATU+num*2, fanctl);
}

void RdRlayStatus(void)
{
    for(int i=0; i<3; i++){
        Meter_run.FanStatus[i] = UserEpromReadU16(EE_RELAY_1_STATU+i*2);
        if(Meter_run.FanStatus[i] > 10)
            Meter_run.FanStatus[i] = 0;
        Meter_run.FanCtl[i] = (FAN_CTL)(UserEpromReadU16(EE_RELAY_1_C_STATU+i*2));
        if(Meter_run.FanCtl[i] > 10)
            Meter_run.FanCtl[i] = (FAN_CTL)0;
        
        Meter_run.FanStatus[i] = Meter_run.FanCtl[i];
        Meter_run.TimeOpenFlag[i] = UserEpromReadU16(EE_TIME_RUN_1_FLAG+i*2);
        if(Meter_run.TimeOpenFlag[i] > 2)
           Meter_run.TimeOpenFlag[i] = 0;
        
        //注意：这里写这个判断，使温度启动风机后，断电。重新上电后，按定时的处理
        if(Meter_run.FanCtl[i] == FAN_CTL_OPEN)
           Meter_run.TimeOpenFlag[i] = 1;
        
    }
    
    
}

void ComPara(void)
{
  //串口1
    if(Meter_run.Com1Baud == 0)
      Meter_usart.Usart1_BaudRate = 2400;
    else if(Meter_run.Com1Baud == 1)
      Meter_usart.Usart1_BaudRate = 4800;
    else if(Meter_run.Com1Baud == 2)
      Meter_usart.Usart1_BaudRate = 9600;
    else if(Meter_run.Com1Baud == 3)
      Meter_usart.Usart1_BaudRate = 19200;
    else if(Meter_run.Com1Baud == 4)
      Meter_usart.Usart1_BaudRate = 38400;
    else if(Meter_run.Com1Baud == 5)
      Meter_usart.Usart1_BaudRate = 57600;
    else if(Meter_run.Com1Baud == 6)
      Meter_usart.Usart1_BaudRate = 115200;
    
    if(Meter_run.Com1Parity == 0)
      Meter_usart.Usart1_Parity = USART_PARITY_NONE;
    else if(Meter_run.Com1Parity == 1)
      Meter_usart.Usart1_Parity = USART_PARITY_EVEN;
    else if(Meter_run.Com1Parity == 2)
      Meter_usart.Usart1_Parity = USART_PARITY_ODD;
    
    if(Meter_run.Com1Stopbit == 0)
      Meter_usart.Usart1_Stopbit = USART_STOP_1_BIT;
    else if(Meter_run.Com1Stopbit == 1)
      Meter_usart.Usart1_Stopbit = USART_STOP_0_5_BIT;
    else if(Meter_run.Com1Stopbit == 2)
      Meter_usart.Usart1_Stopbit = USART_STOP_2_BIT;
    else if(Meter_run.Com1Stopbit == 3)
      Meter_usart.Usart1_Stopbit = USART_STOP_1_5_BIT;
    
    if(Meter_run.Com1Databit == 0)
      Meter_usart.Usart1_Databit = USART_DATA_8BITS;
    else if(Meter_run.Com1Databit == 1)
      Meter_usart.Usart1_Databit = USART_DATA_9BITS;
    
    if(USART_PARITY_NONE != Meter_usart.Usart1_Parity)
      Meter_usart.Usart1_Parity = USART_DATA_9BITS;
    
   //串口2
   if(Meter_run.Com2Baud == 0)
      Meter_usart.Usart6_BaudRate = 2400;
    else if(Meter_run.Com2Baud == 1)
      Meter_usart.Usart6_BaudRate = 4800;
    else if(Meter_run.Com2Baud == 2)
      Meter_usart.Usart6_BaudRate = 9600;
    else if(Meter_run.Com2Baud == 3)
      Meter_usart.Usart6_BaudRate = 19200;
    else if(Meter_run.Com2Baud == 4)
      Meter_usart.Usart6_BaudRate = 38400;
    else if(Meter_run.Com2Baud == 5)
      Meter_usart.Usart6_BaudRate = 57600;
    else if(Meter_run.Com2Baud == 6)
      Meter_usart.Usart6_BaudRate = 115200;
    
    if(Meter_run.Com2Parity == 0)
      Meter_usart.Usart6_Parity = USART_PARITY_NONE;
    else if(Meter_run.Com2Parity == 1)
      Meter_usart.Usart6_Parity = USART_PARITY_EVEN;
    else if(Meter_run.Com2Parity == 2)
      Meter_usart.Usart6_Parity = USART_PARITY_ODD;
    
    if(Meter_run.Com2Stopbit == 0)
      Meter_usart.Usart6_Stopbit = USART_STOP_1_BIT;
    else if(Meter_run.Com2Stopbit == 1)
      Meter_usart.Usart6_Stopbit = USART_STOP_0_5_BIT;
    else if(Meter_run.Com2Stopbit == 2)
      Meter_usart.Usart6_Stopbit = USART_STOP_2_BIT;
    else if(Meter_run.Com2Stopbit == 3)
      Meter_usart.Usart6_Stopbit = USART_STOP_1_5_BIT;
    
    if(Meter_run.Com2Databit == 0)
      Meter_usart.Usart6_Databit = USART_DATA_8BITS;
    else if(Meter_run.Com2Databit == 1)
      Meter_usart.Usart6_Databit = USART_DATA_9BITS;
    
    if(USART_PARITY_NONE != Meter_usart.Usart6_Parity)
      Meter_usart.Usart6_Parity = USART_DATA_9BITS;
}

