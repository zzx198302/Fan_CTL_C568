/*************************************************************** 
 * @FilePath: \....\NewFile\UserApp\UserUsart6_(RS2).c
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

void MX_USART6_UART_Init_Ex(void)
{
    if((Meter_usart.Usart6_BaudRate !=9600) || 
       ((usart_data_bit_num_type)Meter_usart.Usart6_Databit != USART_DATA_8BITS) ||
       ((usart_stop_bit_num_type)Meter_usart.Usart6_Stopbit != USART_STOP_1_BIT) ||
       ((usart_parity_selection_type)Meter_usart.Usart6_Parity != USART_PARITY_NONE))
    {
        usart_init(USART6, Meter_usart.Usart6_BaudRate, (usart_data_bit_num_type)Meter_usart.Usart6_Databit, 
                  (usart_stop_bit_num_type)Meter_usart.Usart6_Stopbit);
        
        usart_parity_selection_config(USART6, (usart_parity_selection_type)Meter_usart.Usart6_Parity);
    }
} 
   
/*******************************************************************************
功能：初始化
输入：
返回：
时间：
*******************************************************************************/
void HAL_INIT_USART6(void)
{
    Meter_usart.Usart6_num = 0;
    /** 计算3.5个字节延时时间，单位ms **/
    uint8_t nPerByte = 10;
    float fval_3_5Bytes_timeout = (3.5f * nPerByte * 1000 / Meter_usart.Usart6_BaudRate);
    Meter_usart.Usart6_wait35 = (uint16_t)ceil(fval_3_5Bytes_timeout);    //向上取整
    //至少要3ms
    if(Meter_usart.Usart6_wait35 < 3){ Meter_usart.Usart6_wait35 = 3; }

    //启动接收中断，接收一字节后触发中断
    USART6_RT_in;
    usart_interrupt_enable(USART6, USART_RDBF_INT, TRUE);
}

/*******************************************************************************
功能：清空计数并转为接收状态
输入：
返回：
时间：
*******************************************************************************/
void Start_USART6_Receive(void)
{
    Meter_usart.Usart6_num = 0;   //接收到的字节数
    Meter_usart.Usart6_s_num = 0; //要发送的字节数
    USART6_RT_in;
    usart_interrupt_enable(USART6, USART_RDBF_INT, TRUE);
}

/*******************************************************************************
功能：串口接收中断中
输入：
返回：
时间：
*******************************************************************************/
void USART6_IRQ_PB(void)
{
    if(Meter_usart.Usart6_num < USART_ReceiveData_LONG)  //范围内
    {
        Meter_usart.Usart6_Receive[Meter_usart.Usart6_num++] = usart_data_receive(USART6);
        Meter_usart.Usart6_TIMER = 0;   //开始从零计数
    }
}

/*******************************************************************************
功能：定时器中断中
输入：
返回：
时间：
***************************************************************************** */
void TIMER6_IRQ_U6(void)
{
    Meter_usart.Usart6_TIMER++;
    if(Meter_usart.Usart6_TIMER > 0x9fffffff){ Meter_usart.Usart6_TIMER = 0x8fffffff; }
    if(Meter_usart.Usart6_TIMER == Meter_usart.Usart6_wait35){
        USART6_RT_out;  //发送状态、不接收
        /* disable the usart2 receive interrupt */
        usart_interrupt_enable(USART6, USART_RDBF_INT, FALSE);
        //接收完成标志
        if(Meter_usart.Usart6_num > 1)
        {
            LOS_SemPost(Usart6_Recv_Finish_Sem);
        }else{
            Start_USART6_Receive();
        }
    }
}

/*******************************************************************************
功能：发送指令读取温湿度
输入：
返回：
时间：
***************************************************************************** */
void USART6_Read_TempHum(uint8_t devAdd){
    uint8_t tempbuff[10];
    uint16_t crcresult;
    
    USART6_RT_out;  //发送状态、不接收
    
    //山东仁科
    tempbuff[0] = devAdd;
    tempbuff[1] = 0x03;
  if(Meter_run.TaHSensorSele == 0){
    tempbuff[2] = 0;
    tempbuff[3] = 0;
  }
  else if(Meter_run.TaHSensorSele == 1){
    tempbuff[2] = 0;
    tempbuff[3] = 154;
  }
    tempbuff[4] = 0;
    tempbuff[5] = 2;
    
    crcresult = getCRC16(tempbuff, 6);
    tempbuff[6] = crcresult & 0xff;
    tempbuff[7] = (crcresult >> 8) & 0xff;
    
    memcpy(Meter_usart.Usart6_Transmit, tempbuff, 8);
    Meter_usart.Usart6_s_num = 8;
    
    rs485TransmitPolling(USART6, Meter_usart.Usart6_Transmit, Meter_usart.Usart6_s_num);
    Start_USART6_Receive();
}

/*******************************************************************************
功能：任务中处理
输入：
返回：
时间：
*******************************************************************************/
void USART6_Function_3(void);

void USART6_TASK_PUBLIC(void)
{
    uint8_t Usart6_num_bak;
    unsigned short crc_result;
    uint8_t Temporary_uint8_t[2];

    Usart6_num_bak = Meter_usart.Usart6_num;
    if(Meter_usart.Usart6_Receive[0]==Meter_usart.Usart6_Transmit[0]) //地址错误不应答
    {
        Meter_usart.Usart6_SendCnt = 0;
        Meter_usart.Usart6_RorSFlag = 0;  //接收标志清零
        crc_result = getCRC16(Meter_usart.Usart6_Receive,Usart6_num_bak-2);
        Temporary_uint8_t[1] = crc_result & 0xff;
        Temporary_uint8_t[0] = (crc_result >> 8) & 0xff;
        //crc校验错误不应答
        if( (Meter_usart.Usart6_Receive[Usart6_num_bak-1]==Temporary_uint8_t[0]) &&
            (Meter_usart.Usart6_Receive[Usart6_num_bak-2]==Temporary_uint8_t[1]) )
        {
            switch(Meter_usart.Usart6_Receive[1])
            {
            case 0x03:
                USART6_Function_3();
                break;
            default: //命令码无效不应答
                Start_USART6_Receive();
                break;
            }
        }
        else
            Start_USART6_Receive();
    }
    else
        Start_USART6_Receive();
}

/*******************************************************************************
功能：modbus功能码3
输入：
返回：
时间：
*******************************************************************************/
void USART6_Function_3(void)
{
    uint8_t Usart6_data_len;
    int16_t TemporaryValue;
    int8_t  index;
    float   TemperValue;
    float   HumiValue;
        
    Usart6_data_len = Meter_usart.Usart6_Transmit[5]*2;
    if(Meter_usart.Usart6_Receive[2] == Usart6_data_len)
    {
      if(Meter_run.TaHSensorSele == 0){ //山东仁科
        TemporaryValue = (int16_t)((Meter_usart.Usart6_Receive[3] << 8) + Meter_usart.Usart6_Receive[4]);
        HumiValue = (float)(TemporaryValue * 0.1);
        TemporaryValue = (int16_t)((Meter_usart.Usart6_Receive[5] << 8) + Meter_usart.Usart6_Receive[6]);
        TemperValue = (float)(TemporaryValue * 0.1);
      }
      else if(Meter_run.TaHSensorSele == 1){ //珠海汉升
        TemporaryValue = (int16_t)((Meter_usart.Usart6_Receive[3] << 8) + Meter_usart.Usart6_Receive[4]);
        TemperValue = (float)(TemporaryValue * 0.01);
        TemporaryValue = (int16_t)((Meter_usart.Usart6_Receive[5] << 8) + Meter_usart.Usart6_Receive[6]);
        HumiValue = (float)(TemporaryValue * 0.01);
      }
        //放入实际温湿度中
        index = findComAddIndex(Meter_usart.Usart6_Receive[0]);
        Meter_run.Tempert[index] = TemperValue;
        Meter_run.Humidity[index] = HumiValue;
        
        Meter_run.U6ComCount[index] = 0;
        
    }
    Start_USART6_Receive();  
}

/*******************************************************************************
功能：UASRT6_TASK_Poll:主动查询数据 （100ms任务中）
输入：
返回：
时间：
*******************************************************************************/ 
#define COM_TIME_OUT    30      //通讯超时3秒
void UASRT6_TASK_Poll(void) 
{
    uint8_t no = 0;
    uint8_t mbsadd = 0;
    
    //发送数据
    Meter_usart.Usart6_SendCnt++;
    if(0 == Meter_usart.Usart6_RorSFlag)
    {
        if(Meter_usart.Usart6_SendCnt >= 5)
        {                     
            no = (Meter_usart.Usart6_SendNum % 3);
            Meter_usart.Usart6_SendNum++;
            if(Meter_usart.Usart6_SendNum >= 3)
               Meter_usart.Usart6_SendNum = 0;
            
            if(0 != Meter_run.FanComAdd[no]){
                mbsadd = Meter_run.FanComAdd[no];         
                Meter_usart.Usart6_SendCnt = 0;
                Meter_usart.Usart6_RorSFlag = 1;
                
                //发送数据
                USART6_Read_TempHum(mbsadd);
                if(Meter_run.U6ComCount[no] <= 0xff)
                   Meter_run.U6ComCount[no]++;
            }
            else{
                 Meter_usart.Usart6_SendCnt = 5;               
            }            
        }
    }
    else if(1 == Meter_usart.Usart6_RorSFlag){
        if(Meter_usart.Usart6_SendCnt >= COM_TIME_OUT){  //3s没有回复指令
            Meter_usart.Usart6_SendCnt = 0;
            Meter_usart.Usart6_RorSFlag = 0;           
        }
    }
    
    //判断通讯中断
    for(int i=0; i<3; i++){
      if(Meter_run.U6ComCount[i] >= 10){
          Meter_run.Tempert[i] = 0;
          Meter_run.Humidity[i] = 0;
      }
    }
}
