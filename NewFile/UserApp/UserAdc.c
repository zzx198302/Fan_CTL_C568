/*************************************************************** 
 * @FilePath: \....\NewFile\UserApp\UserAdc.c
 * @Version: V1.0.0
 * @Author: ZhongZhanxiong
 * @Date: 2026-05-19 18:11:31
 * @LastEditors: ZhongZhanxiong
 * @LastEditTime: 2026-05-20 11:16:55
 * @Description: 应用程序变量定义文件，以及系统参数用户掉电数据处理文件
 * @Copyright (c) 2025 by PengYang, All Rights Reserved. 
 ***************************************************************/
#include "User_head.h"
#include <math.h>

void Adc_Rms_Conversion(void){
    uint32_t dataSum, squerVal[3], X2X;
    
    for(int i=0; i<ADC_POINT; i++){
        Meter_ADC.Fan1_Cur_Adc[i]  = Meter_ADC.Adc1_Value[(i*4) + 0];
        Meter_ADC.Fan2_Cur_Adc[i]  = Meter_ADC.Adc1_Value[(i*4) + 1];
        Meter_ADC.Fan3_Cur_Adc[i]  = Meter_ADC.Adc1_Value[(i*4) + 2];
        Meter_ADC.Inter_Vol_Adc[i] = Meter_ADC.Adc1_Value[(i*4) + 3];
    }
    //========================================
    /*
    dma_channel_enable(DMA1_CHANNEL1, TRUE);
    tmr_channel_enable(TMR2, TMR_SELECT_CHANNEL_2, TRUE);
    tmr_output_enable(TMR2, TRUE);
    dma_interrupt_enable(DMA1_CHANNEL1, DMA_FDT_INT, TRUE);
    */
   
    //==============内部电压==================
    dataSum = 0;
    BubbleSort((uint16_t *)&Meter_ADC.Inter_Vol_Adc[0], ADC_POINT);
    for(int j=30; j<90; j++){
        dataSum += Meter_ADC.Inter_Vol_Adc[j];
    }
    Meter_ADC.Inter_Vol_Adc_Avg = (uint16_t)(dataSum/60.0+0.5);
    Meter_run.InterVol = (float)((3.3 * Meter_ADC.Inter_Vol_Adc_Avg) / 4095.0);
    
    //==============风机电流==================
    //电流均方根
    squerVal[0] = 0;
    squerVal[1] = 0;
    squerVal[2] = 0;
    
    if(Meter_ADC.Fan_Check_step == 10){ //0值
        for(int i=0; i<ADC_POINT; i++){
            squerVal[0] += Meter_ADC.Fan1_Cur_Adc[i] * Meter_ADC.Fan1_Cur_Adc[i];
            squerVal[1] += Meter_ADC.Fan2_Cur_Adc[i] * Meter_ADC.Fan2_Cur_Adc[i];
            squerVal[2] += Meter_ADC.Fan3_Cur_Adc[i] * Meter_ADC.Fan3_Cur_Adc[i];
        }
    }
    else{
        for(int i=0; i<ADC_POINT; i++){
            if(Meter_ADC.Fan1_Cur_Adc[i] > Meter_ADC.Fan1_Cur_Adc_base_Check)
                X2X = Meter_ADC.Fan1_Cur_Adc[i] - Meter_ADC.Fan1_Cur_Adc_base_Check;
            else
                X2X = Meter_ADC.Fan1_Cur_Adc_base_Check - Meter_ADC.Fan1_Cur_Adc[i];
            squerVal[0] += X2X*X2X;
            
            if(Meter_ADC.Fan2_Cur_Adc[i] > Meter_ADC.Fan2_Cur_Adc_base_Check)
                X2X = Meter_ADC.Fan2_Cur_Adc[i] - Meter_ADC.Fan2_Cur_Adc_base_Check;
            else
                X2X = Meter_ADC.Fan2_Cur_Adc_base_Check - Meter_ADC.Fan2_Cur_Adc[i];
            squerVal[1] += X2X*X2X;
            
            if(Meter_ADC.Fan3_Cur_Adc[i] > Meter_ADC.Fan3_Cur_Adc_base_Check)
                X2X = Meter_ADC.Fan3_Cur_Adc[i] - Meter_ADC.Fan3_Cur_Adc_base_Check;
            else
                X2X = Meter_ADC.Fan3_Cur_Adc_base_Check - Meter_ADC.Fan3_Cur_Adc[i];
            squerVal[2] += X2X*X2X;
        }
    }
    
    Meter_ADC.Fan1_Cur_Adc_ResVal[Meter_ADC.Fan_Adc_ResTime] = (uint16_t)(sqrt(squerVal[0]/ADC_POINT)+0.5);
    Meter_ADC.Fan2_Cur_Adc_ResVal[Meter_ADC.Fan_Adc_ResTime] = (uint16_t)(sqrt(squerVal[1]/ADC_POINT)+0.5);
    Meter_ADC.Fan3_Cur_Adc_ResVal[Meter_ADC.Fan_Adc_ResTime] = (uint16_t)(sqrt(squerVal[2]/ADC_POINT)+0.5);
    Meter_ADC.Fan_Adc_ResTime++;
    if(10 <= Meter_ADC.Fan_Adc_ResTime){
        Meter_ADC.Fan_Adc_ResTime = 0;
        
        dataSum = 0;
        BubbleSort((uint16_t *)&Meter_ADC.Fan1_Cur_Adc_ResVal[0], 10);
        for(int i=2; i<8; i++){
            dataSum += Meter_ADC.Fan1_Cur_Adc_ResVal[i];
        }
        Meter_ADC.Fan1_Cur_Adc_Avg = (uint16_t)(dataSum/6.0+0.5);
        
        dataSum = 0;
        BubbleSort((uint16_t *)&Meter_ADC.Fan2_Cur_Adc_ResVal[0], 10);
        for(int i=2; i<8; i++){
            dataSum += Meter_ADC.Fan2_Cur_Adc_ResVal[i];
        }
        Meter_ADC.Fan2_Cur_Adc_Avg = (uint16_t)(dataSum/6.0+0.5);
        
        dataSum = 0;
        BubbleSort((uint16_t *)&Meter_ADC.Fan3_Cur_Adc_ResVal[0], 10);
        for(int i=2; i<8; i++){
            dataSum += Meter_ADC.Fan3_Cur_Adc_ResVal[i];
        }
        Meter_ADC.Fan3_Cur_Adc_Avg = (uint16_t)(dataSum/6.0+0.5);
        
        //校准
        if(0 != Meter_ADC.Fan_Check_step){
            Meter_ADC.Fan_Check_Time++;
            if(2 <= Meter_ADC.Fan_Check_Time){
                if(Meter_ADC.Fan_Check_step == 10){
                    Meter_ADC.Fan1_Cur_Adc_base_Check = Meter_ADC.Fan1_Cur_Adc_Avg;
                    Meter_ADC.Fan2_Cur_Adc_base_Check = Meter_ADC.Fan2_Cur_Adc_Avg;
                    Meter_ADC.Fan3_Cur_Adc_base_Check = Meter_ADC.Fan3_Cur_Adc_Avg;
                    //存储参数
                    UserEpromWriteU16(EE_FAN_1_CHECK_BASE, Meter_ADC.Fan1_Cur_Adc_base_Check); 
                    UserEpromWriteU16(EE_FAN_2_CHECK_BASE, Meter_ADC.Fan2_Cur_Adc_base_Check); 
                    UserEpromWriteU16(EE_FAN_3_CHECK_BASE, Meter_ADC.Fan3_Cur_Adc_base_Check); 
                }
                else if(Meter_ADC.Fan_Check_step == 100){
                    Meter_ADC.Fan1_Cur_Adc_full_Check = Meter_ADC.Fan1_Cur_Adc_Avg;
                    Meter_ADC.Fan2_Cur_Adc_full_Check = Meter_ADC.Fan2_Cur_Adc_Avg;
                    Meter_ADC.Fan3_Cur_Adc_full_Check = Meter_ADC.Fan3_Cur_Adc_Avg;                    
                    //存储参数
                    UserEpromWriteU16(EE_FAN_1_CHECK_FULL, Meter_ADC.Fan1_Cur_Adc_full_Check); 
                    UserEpromWriteU16(EE_FAN_2_CHECK_FULL, Meter_ADC.Fan2_Cur_Adc_full_Check); 
                    UserEpromWriteU16(EE_FAN_3_CHECK_FULL, Meter_ADC.Fan3_Cur_Adc_full_Check); 
                }
                Meter_ADC.Fan_Check_step = 0;
                Meter_ADC.Fan_Check_Time = 0;  
            }
        }
        else{//实际电流值
          
            Meter_run.FanCurTemp[0] = (float)(10.0*Meter_ADC.Fan1_Cur_Adc_Avg/Meter_ADC.Fan1_Cur_Adc_full_Check);
            Meter_run.FanCurTemp[1] = (float)(10.0*Meter_ADC.Fan2_Cur_Adc_Avg/Meter_ADC.Fan2_Cur_Adc_full_Check);
            Meter_run.FanCurTemp[2] = (float)(10.0*Meter_ADC.Fan3_Cur_Adc_Avg/Meter_ADC.Fan3_Cur_Adc_full_Check);
            
            for(int i=0; i<3; i++){
              if((Meter_run.FanCurTemp[i] <= 0.1)){            
                  Meter_run.FanCurTemp[i] = 0;
              }
            }
            Meter_run.FanCur[0] = Meter_run.FanCurTemp[1]; 
            Meter_run.FanCur[1] = Meter_run.FanCurTemp[2]; 
            Meter_run.FanCur[2] = Meter_run.FanCurTemp[0]; 
        }
    }
    
    /*     
    dma_data_number_set(DMA1_CHANNEL1,DMA1_CHANNEL1_BUFFER_SIZE);
    dma_interrupt_enable(DMA1_CHANNEL1, DMA_FDT_INT, TRUE);
    dma_channel_enable(DMA1_CHANNEL1, TRUE);
    adc_ordinary_conversion_data_get(ADC1);
    tmr_period_value_set(TMR2,155);//设置周期值
    tmr_channel_value_set(TMR2, TMR_SELECT_CHANNEL_2, 78);//设置50%占空比值
    tmr_counter_value_set(TMR2,0);
    adc_dma_mode_enable(ADC1, TRUE);
    tmr_counter_enable(TMR2, TRUE);
    */

}


