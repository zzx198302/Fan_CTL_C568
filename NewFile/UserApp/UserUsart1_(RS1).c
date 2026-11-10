/*************************************************************** 
 * @FilePath: \....\NewFile\UserApp\UserUsart1_(RS1).c
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

void MX_USART1_UART_Init_Ex(void)
{
    if((Meter_usart.Usart1_BaudRate !=9600) || 
       ((usart_data_bit_num_type)Meter_usart.Usart1_Databit != USART_DATA_8BITS) ||
       ((usart_stop_bit_num_type)Meter_usart.Usart1_Stopbit != USART_STOP_1_BIT) ||
       ((usart_parity_selection_type)Meter_usart.Usart1_Parity != USART_PARITY_NONE))
    {
        usart_init(USART1, Meter_usart.Usart1_BaudRate, (usart_data_bit_num_type)Meter_usart.Usart1_Databit, 
                  (usart_stop_bit_num_type)Meter_usart.Usart1_Stopbit);
        
        usart_parity_selection_config(USART1, (usart_parity_selection_type)Meter_usart.Usart1_Parity);
    }
} 

/*******************************************************************************
功能：初始化
输入：
返回：
时间：
*******************************************************************************/
void HAL_INIT_USART1(void)
{
    Meter_usart.Usart1_num = 0;
    /** 计算3.5个字节延时时间，单位ms **/
    uint8_t nPerByte = 10;
    float fval_3_5Bytes_timeout = (3.5f * nPerByte * 1000 / Meter_usart.Usart1_BaudRate);
    Meter_usart.Usart1_wait35 = (uint16_t)ceil(fval_3_5Bytes_timeout);    //向上取整
    //至少要3ms
    if(Meter_usart.Usart1_wait35 < 3){ Meter_usart.Usart1_wait35 = 3; }

    //启动接收中断
    USART1_RT_in;
    usart_interrupt_enable(USART1, USART_RDBF_INT, TRUE);
}

/*******************************************************************************
功能：清空计数并转为接收状态
输入：
返回：
时间：
*******************************************************************************/
void Start_USART1_Receive(void)
{
    Meter_usart.Usart1_num = 0;   //接收到的字节数
    Meter_usart.Usart1_s_num = 0; //要发送的字节数
    USART1_RT_in;                 //接收状态
    usart_interrupt_enable(USART1, USART_RDBF_INT, TRUE);
}
        
/*******************************************************************************
功能：串口接收中断中
输入：
返回：
时间：
*******************************************************************************/
void USART1_IRQ_PB(void)
{
    if(Meter_usart.Usart1_num < USART_ReceiveData_LONG)  //范围内
    {
        Meter_usart.Usart1_Receive[Meter_usart.Usart1_num++] = usart_data_receive(USART1);
        Meter_usart.Usart1_TIMER = 0;   //开始从零计数
    }
}

/*******************************************************************************
功能：定时器中断中
输入：
返回：
时间：
*******************************************************************************/
void TIMER6_IRQ_U1(void)
{
    Meter_usart.Usart1_TIMER++;
    if(Meter_usart.Usart1_TIMER > 0x9fffffff){ Meter_usart.Usart1_TIMER = 0x8fffffff; }
    if(Meter_usart.Usart1_TIMER == Meter_usart.Usart1_wait35){
        USART1_RT_out;  //发送状态、不接收
        /* disable the usart1 receive interrupt */
        usart_interrupt_enable(USART1, USART_RDBF_INT, FALSE);
        //接收完成标志
        if(Meter_usart.Usart1_num > 1)
        {
            LOS_SemPost(Usart1_Recv_Finish_Sem);
        }else{
            Start_USART1_Receive();
        }
    }
}

/*******************************************************************************
功能：任务中处理
输入：
返回：
时间：
*******************************************************************************/
void USART1_Function_3(void);
void USART1_Function_6(void);
void EpromData(uint16_t regAdd, uint16_t regData);

void USART1_TASK_PUBLIC(void)
{
    uint8_t Usart1_num_bak;
    unsigned short crc_result;
    uint8_t Temporary_uint8_t[2];

    Usart1_num_bak = Meter_usart.Usart1_num;
    if( (Meter_usart.Usart1_Receive[0]==0) || (Meter_usart.Usart1_Receive[0]==255) ||
        (Meter_usart.Usart1_Receive[0]== Meter_usart.Usart1_addr))   //地址错误不应答
    {
        if(Meter_usart.Usart1_Receive[0]==0)
        {
            Start_USART1_Receive();
            return;
        }

        crc_result = getCRC16(Meter_usart.Usart1_Receive,Usart1_num_bak-2);
        Temporary_uint8_t[1] = crc_result & 0xff;
        Temporary_uint8_t[0] = (crc_result >> 8) & 0xff;
        //crc校验错误不应答
        if( (Meter_usart.Usart1_Receive[Usart1_num_bak-1]==Temporary_uint8_t[0]) &&
            (Meter_usart.Usart1_Receive[Usart1_num_bak-2]==Temporary_uint8_t[1]) )
        {
            switch(Meter_usart.Usart1_Receive[1]){
            case 0x03:
                USART1_Function_3();
                break;
            case 0x06:
                USART1_Function_6();
                break;
            default: //命令码无效不应答
                Meter_usart.Usart1_s_num = rs485ErrorHandle(RS485_FUN_ERR, Meter_usart.Usart1_Transmit,
                                            Meter_usart.Usart1_Receive[0], Meter_usart.Usart1_Receive[1]);
                Usart1_num_bak = Meter_usart.Usart1_s_num;
                rs485TransmitPolling(USART1, Meter_usart.Usart1_Transmit, Usart1_num_bak);
                Start_USART1_Receive();
                break;
            }
        }else{
            Start_USART1_Receive();
        }
    }else{
        Start_USART1_Receive();
    }
}

/*******************************************************************************
功能：modbus功能码3
输入：
返回：
时间：
*******************************************************************************/
void USART1_Function_3(void)
{
    uint16_t mbegindress;
    uint16_t menddress;
    uint16_t mlongdress;
    uint16_t crcresult;
    uint16_t mi;

    mbegindress = (Meter_usart.Usart1_Receive[2] << 8) + Meter_usart.Usart1_Receive[3];    //起始地址
    mlongdress= (Meter_usart.Usart1_Receive[4] << 8) + Meter_usart.Usart1_Receive[5];      //长度
    menddress=mbegindress+mlongdress;                                                      //结束地址

    if((mbegindress <= 1000)&&(menddress <= 1000)&&(mlongdress <= 100))          //基本测量数据
    {
        Meter_usart.Usart1_Transmit[0] = Meter_usart.Usart1_addr;
        Meter_usart.Usart1_Transmit[1] = 0x03;
        Meter_usart.Usart1_Transmit[2] = 2 * Meter_usart.Usart1_Receive[5];
        Meter_usart.Usart1_s_num = 2 * Meter_usart.Usart1_Receive[5] + 3;
        for(mi=mbegindress*2;mi<menddress*2;mi++)
        {
            Meter_usart.Usart1_Transmit[mi-mbegindress*2+3]=Meter_usart.usart_record[mi];
        }
        crcresult = getCRC16(Meter_usart.Usart1_Transmit,Meter_usart.Usart1_s_num);
        Meter_usart.Usart1_Transmit[Meter_usart.Usart1_s_num] = crcresult & 0xff;
        Meter_usart.Usart1_Transmit[Meter_usart.Usart1_s_num+1] = (crcresult >> 8) & 0xff;
        Meter_usart.Usart1_s_num = Meter_usart.Usart1_s_num+2;
    }   
    else if(mlongdress > 100)
    {
        Meter_usart.Usart1_s_num = rs485ErrorHandle(RS485_DATA_ERR, Meter_usart.Usart1_Transmit,
                                    Meter_usart.Usart1_Receive[0], Meter_usart.Usart1_Receive[1]);
    }
    else
    {
        uint8_t isAddrValid = ( (mbegindress <= 1000 && menddress <= 1000) );
        if(!isAddrValid){
            Meter_usart.Usart1_s_num = rs485ErrorHandle(RS485_ADDR_ERR, Meter_usart.Usart1_Transmit,
                                        Meter_usart.Usart1_Receive[0], Meter_usart.Usart1_Receive[1]);
        }
    }
    //发送数据
    rs485TransmitPolling(USART1, Meter_usart.Usart1_Transmit, Meter_usart.Usart1_s_num);
    Start_USART1_Receive();
}

/*******************************************************************************
功能：modbus功能码6
输入：
返回：
时间：
*******************************************************************************/
void USART1_Function_6(void)
{
    uint16_t Temporary_re_address,Temporary_u16,re_rxd,epromFlag;
    uint16_t crcresult;

    Temporary_re_address = (Meter_usart.Usart1_Receive[2]<<8) + Meter_usart.Usart1_Receive[3];
    Temporary_u16 = (Meter_usart.Usart1_Receive[4]<<8) + Meter_usart.Usart1_Receive[5];
    re_rxd = 0;
    epromFlag = 0;
    //==========================================================================
    switch(Temporary_re_address)
    {
        case 25:
            if((Temporary_u16 >= 15) && (Temporary_u16 <= 130)){
                Meter_run.FanStopTemp = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;
        case 26:
            if((Temporary_u16 >= 15) && (Temporary_u16 <= 130)){
                Meter_run.FanStartTemp = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;
        case 27:
            if((Temporary_u16 >= 15) && (Temporary_u16 <= 130)){
                Meter_run.FanErrTemp = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;    
        case 28:
            if(Temporary_u16 <= 240){
                Meter_run.FanTimeInter = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;   
        case 29:
            if((Temporary_u16 >= 1) && (Temporary_u16 <= 30)){
                Meter_run.FanTimeRun = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;    
    //=================================================================
        case 30:
            if(Temporary_u16 <= 6){
                Meter_run.Com1Baud = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;    
        case 31:
            if(Temporary_u16 <= 2){
                Meter_run.Com1Parity = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;   
        case 32:
            if(Temporary_u16 <= 1){
                Meter_run.Com1Databit = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;   
        case 33:
            if(Temporary_u16 <= 3){
                Meter_run.Com1Stopbit = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;   
        case 34:
            if(Temporary_u16 <= 6){
                Meter_run.Com2Baud = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;   
        case 35:
            if(Temporary_u16 <= 2){
                Meter_run.Com2Parity = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;   
        case 36:
            if(Temporary_u16 <= 1){
                Meter_run.Com2Databit = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;               
        case 37:
            if(Temporary_u16 <= 3){
                Meter_run.Com2Stopbit = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;               
    //=================================================================
        case 38:
            if((Temporary_u16 >= 1) && (Temporary_u16 <= 255)){
                Meter_run.DevAdd = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;    
        case 39:
            if(Temporary_u16 <= 255){
                Meter_run.FanComAdd[0] = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;
        case 40:
            if(Temporary_u16 <= 255){
                Meter_run.FanComAdd[1] = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;
        case 41:
            if(Temporary_u16 <= 255){
                Meter_run.FanComAdd[2] = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;
    //=================================================================            
        case 42:
            if((Temporary_u16 >= 1) && (Temporary_u16 <= 999)){
                Meter_run.FanSetCur[0] = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;            
        case 43:
            if((Temporary_u16 >= 1) && (Temporary_u16 <= 999)){
                Meter_run.FanSetCur[1] = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;                  
        case 44:
            if((Temporary_u16 >= 1) && (Temporary_u16 <= 999)){
                Meter_run.FanSetCur[2] = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;                 
        case 45:
            if(Temporary_u16 <= 1){
                Meter_run.FanEnable[0] = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;                        
        case 46:
            if(Temporary_u16 <= 1){
                Meter_run.FanEnable[1] = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;                        
        case 47:
            if(Temporary_u16 <= 1){
                Meter_run.FanEnable[2] = Temporary_u16;
                epromFlag = 1;
                re_rxd = 1;
            }
            else 
                re_rxd = 2;
            break;                        
    //=================================================================
        case 100:
          Meter_run.TaHSensorSele = Temporary_u16;
          epromFlag = 1;
          re_rxd = 1;
          break;
    //=================================================================  
        case 151:
          if(Temporary_u16 <= EVENT_LOG.E_Total_Num){
            EVENT_LOG.E_Present_Save_ad_bak = Temporary_u16;
          }
          re_rxd = 1;
          break;   
    //=================================================================       
        case 200:
            if(Temporary_u16 >= 1)
              Meter_run.HandAuto = 1;  
            else
              Meter_run.HandAuto = 0;
            re_rxd = 1;
            break;
        case 201:
            if(Temporary_u16 >= 1)
              Meter_run.FanCtl[0] = (FAN_CTL)(1);
            else
              Meter_run.FanCtl[0] = (FAN_CTL)(0);
            epromFlag = 2;
            re_rxd = 1;
            break;
        case 202:
            if(Temporary_u16 >= 1)
              Meter_run.FanCtl[1] = (FAN_CTL)(1);
            else
              Meter_run.FanCtl[1] = (FAN_CTL)(0);
            epromFlag = 2;
            re_rxd = 1;
            break;
        case 203:
            if(Temporary_u16 >= 1)
              Meter_run.FanCtl[2] = (FAN_CTL)(1);
            else
              Meter_run.FanCtl[2] = (FAN_CTL)(0);
            epromFlag = 2;
            re_rxd = 1;
            break;
        case 204:
            Meter_ADC.Fan_Check_step = 10;
            re_rxd = 1;
            break;
        case 205:
            Meter_ADC.Fan_Check_step = 100;
            re_rxd = 1;
            break;
        default:
            break;
                       
            
    }

    //返回处理
    if(re_rxd==1)   //应答返回
    {      
        Meter_usart.Usart1_Transmit[0] = Meter_usart.Usart1_addr;
        Meter_usart.Usart1_Transmit[1] = 0x6;
        Meter_usart.Usart1_Transmit[2] = Meter_usart.Usart1_Receive[2];
        Meter_usart.Usart1_Transmit[3] = Meter_usart.Usart1_Receive[3];
        Meter_usart.Usart1_Transmit[4] = Meter_usart.Usart1_Receive[4];
        Meter_usart.Usart1_Transmit[5] = Meter_usart.Usart1_Receive[5];
        crcresult = getCRC16(Meter_usart.Usart1_Transmit,6);
        Meter_usart.Usart1_Transmit[6] = crcresult & 0xff;
        Meter_usart.Usart1_Transmit[7] = (crcresult >> 8) & 0xff;
        Meter_usart.Usart1_s_num = 8;
    }
    else if(re_rxd==2)
    {
        Meter_usart.Usart1_s_num = rs485ErrorHandle(RS485_DATA_ERR, Meter_usart.Usart1_Transmit,
                                        Meter_usart.Usart1_Receive[0], Meter_usart.Usart1_Receive[1]);
    }
    //发送数据
    rs485TransmitPolling(USART1, Meter_usart.Usart1_Transmit, Meter_usart.Usart1_s_num);
    Start_USART1_Receive();
    
    if(epromFlag == 1){  
        EpromData(Temporary_re_address, Temporary_u16);
    }
    else if(epromFlag == 2){
        if(201 == Temporary_re_address)
          Relay_Ctl(0,Meter_run.FanCtl[0]);
        else if(202 == Temporary_re_address)
          Relay_Ctl(1,Meter_run.FanCtl[1]);
         else if(203 == Temporary_re_address)
          Relay_Ctl(2,Meter_run.FanCtl[2]);
    }
    //if(Meter_run.ResetXitong == 1){ LOS_TaskDelay(1000); }
}

void EpromData(uint16_t regAdd, uint16_t regData){
    if(25 == regAdd)
        UserEpromWriteU16(EE_FAN_STOPTEMP, regData); 
    else if(26 == regAdd)
        UserEpromWriteU16(EE_FAN_STARTTEMP, regData);  
    else if(27 == regAdd)
        UserEpromWriteU16(EE_FAN_ERRTEMP, regData);
    else if(28 == regAdd)
        UserEpromWriteU16(EE_FAN_TIME_INTER, regData);  
    else if(29 == regAdd)
        UserEpromWriteU16(EE_FAN_TIME_RUN, regData);
    //===========================
    else if(30 == regAdd)
        UserEpromWriteU8(EE_COM_1_BAUD, regData);  
    else if(31 == regAdd)
        UserEpromWriteU8(EE_COM_1_PARITY, regData);
    else if(32 == regAdd)
        UserEpromWriteU8(EE_COM_1_DATABIT, regData);  
    else if(33 == regAdd)
        UserEpromWriteU8(EE_COM_1_STOPBIT, regData);
    else if(34 == regAdd)
        UserEpromWriteU8(EE_COM_2_BAUD, regData);  
    else if(35 == regAdd)
        UserEpromWriteU8(EE_COM_2_PARITY, regData);
    else if(36 == regAdd)
        UserEpromWriteU8(EE_COM_2_DATABIT, regData);  
    else if(37 == regAdd)
        UserEpromWriteU8(EE_COM_2_STOPBIT, regData);
    //=============================
    else if(38 == regAdd)
        UserEpromWriteU8(EE_COM_DEV_ADD, regData);  
    else if(39 == regAdd)
        UserEpromWriteU8(EE_COM_FAN1_ADD, regData);
    else if(40 == regAdd)
        UserEpromWriteU8(EE_COM_FAN2_ADD, regData);  
    else if(41 == regAdd)
        UserEpromWriteU8(EE_COM_FAN3_ADD, regData);
    //=============================
    else if(42 == regAdd)
        UserEpromWriteU16(EE_FAN_1_CUR, regData);  
    else if(43 == regAdd)
        UserEpromWriteU16(EE_FAN_2_CUR, regData);
    else if(44 == regAdd)
        UserEpromWriteU16(EE_FAN_3_CUR, regData);  
    else if(45 == regAdd)
        UserEpromWriteU16(EE_FAN_1_ENABLE, regData);
    else if(46 == regAdd)
        UserEpromWriteU16(EE_FAN_2_ENABLE, regData);  
    else if(47 == regAdd)
        UserEpromWriteU16(EE_FAN_3_ENABLE, regData);
    //==============================
    else if(100 == regAdd)
        UserEpromWriteU16(EE_TAH_SENSOR_SELE, regData);
}




