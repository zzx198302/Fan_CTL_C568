/*************************************************************** 
 * @FilePath: \....\NewFile\UserApp\UserIO.c
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
#include "UserVariable.h"

/********************************************************************************
* Function Name  : void Relay_1_Ctl(FAN_CTL fanctl)
* Description    : 磁保持继电器1控制
* Input          : 控制逻辑：开启、关闭
* Output         :
* Return         :
* Author         ：
* Date           ：
*******************************************************************************/   
void Relay_1_Ctl(FAN_CTL fanctl)
{
    if(FAN_CTL_CLOSE == fanctl){
        gpio_bits_set(KG1_GPIO_PORT, KG1_PIN);
        LOS_TaskDelay(150);
        gpio_bits_reset(KG1_GPIO_PORT, KG1_PIN);
        LOS_TaskDelay(150);            
    }
    else if(FAN_CTL_OPEN == fanctl){       
        gpio_bits_set(KG2_GPIO_PORT, KG2_PIN);
        LOS_TaskDelay(150);
        gpio_bits_reset(KG2_GPIO_PORT, KG2_PIN);
        LOS_TaskDelay(150);             
    }
}

void Relay_2_Ctl(FAN_CTL fanctl)
{
    if(FAN_CTL_CLOSE == fanctl){     
        gpio_bits_set(KG3_GPIO_PORT, KG3_PIN);
        LOS_TaskDelay(150);
        gpio_bits_reset(KG3_GPIO_PORT, KG3_PIN);
        LOS_TaskDelay(150);              
    }
    else if(FAN_CTL_OPEN == fanctl){     
        gpio_bits_set(KG4_GPIO_PORT, KG4_PIN);
        LOS_TaskDelay(150);
        gpio_bits_reset(KG4_GPIO_PORT, KG4_PIN);
        LOS_TaskDelay(150);
    }
}

void Relay_3_Ctl(FAN_CTL fanctl)
{
    if(FAN_CTL_CLOSE == fanctl){       
        gpio_bits_set(KG5_GPIO_PORT, KG5_PIN);
        LOS_TaskDelay(150);
        gpio_bits_reset(KG5_GPIO_PORT, KG5_PIN);
        LOS_TaskDelay(150);             
    }
    else if(FAN_CTL_OPEN == fanctl){         
        gpio_bits_set(KG6_GPIO_PORT, KG6_PIN);
        LOS_TaskDelay(150);
        gpio_bits_reset(KG6_GPIO_PORT, KG6_PIN);
        LOS_TaskDelay(150);       
    }
}

//继电器的状态
void Relay_1_Status(FAN_CTL fanctl)
{
     if(FAN_CTL_CLOSE == fanctl){
        if(Meter_run.FanStatus[0] != FAN_STATUS_FAULT)
            Meter_run.FanStatus[0] = FAN_STATUS_STOP;
     }
    else if(FAN_CTL_OPEN == fanctl){
         if(Meter_run.FanStatus[0] != FAN_STATUS_FAULT)
            Meter_run.FanStatus[0] = FAN_STATUS_OPEN;
    }
}

void Relay_2_Status(FAN_CTL fanctl)
{
     if(FAN_CTL_CLOSE == fanctl){
        if(Meter_run.FanStatus[1] != FAN_STATUS_FAULT)
            Meter_run.FanStatus[1] = FAN_STATUS_STOP;
     }
    else if(FAN_CTL_OPEN == fanctl){
         if(Meter_run.FanStatus[1] != FAN_STATUS_FAULT)
            Meter_run.FanStatus[1] = FAN_STATUS_OPEN;
    }
}

void Relay_3_Status(FAN_CTL fanctl)
{
     if(FAN_CTL_CLOSE == fanctl){
        if(Meter_run.FanStatus[2] != FAN_STATUS_FAULT)
            Meter_run.FanStatus[2] = FAN_STATUS_STOP;
     }
    else if(FAN_CTL_OPEN == fanctl){
         if(Meter_run.FanStatus[2] != FAN_STATUS_FAULT)
            Meter_run.FanStatus[2] = FAN_STATUS_OPEN;
    }
}

/********************************************************************************
* Function Name  : void Relay_Ctl(uint8_t num, FAN_CTL fanctl)
* Description    : 磁保持继电器控制
* Input          : fanctl 控制逻辑：开启、关闭
                   num：控制第几个磁保持继电器
* Output         :
* Return         :
* Author         ：因接线问题：将电路图图中的继电器1->实际继电器2
                                             继电器2->实际继电器3
                                             继电器3->实际继电器1
* Date           ：
*******************************************************************************/     

void Relay_Ctl(uint8_t num, FAN_CTL fanctl){
    switch(num)
    {
    case 0:
        Relay_1_Status(fanctl);
        Relay_2_Ctl(fanctl);  
        Meter_run.FanCtl[0] = fanctl;
        WrRelayCtlStatusLog(0, fanctl);
        break;
    case 1:
        Relay_2_Status(fanctl);
        Relay_3_Ctl(fanctl); 
        Meter_run.FanCtl[1] = fanctl;
        WrRelayCtlStatusLog(1, fanctl);
        break;
    case 2:
        Relay_3_Status(fanctl);
        Relay_1_Ctl(fanctl);
        Meter_run.FanCtl[2] = fanctl;
        WrRelayCtlStatusLog(2, fanctl);
        break;
    default:
        break;
    }
}

/********************************************************************************
* Function Name  : void KG_7_Ctl(FAN_CTL fanctl)
* Description    : 开关7控制
* Input          : 控制逻辑：开启、关闭
* Output         :
* Return         :
* Author         ：
* Date           ：
*******************************************************************************/
void KG_7_Ctl(FAN_CTL fanctl)
{
    if(FAN_CTL_OPEN == fanctl){
        gpio_bits_set(KG7_GPIO_PORT, KG7_PIN);
    }
    else if(FAN_CTL_CLOSE == fanctl){
        gpio_bits_reset(KG7_GPIO_PORT, KG7_PIN);
    }
}


//在100ms任务事件中
#define FAN_START_TIME  30
#define FAN_CLOSE_TIME  30
#define TEMP_ALARM_TIME 50

void fan_ctl_handle(void){
    static uint16_t FanStartTime[3]={0};
    static uint16_t FanCloseTime[3]={0};
    static uint8_t  StartFlag[3]={0};
    static uint16_t TempAlmTime[3]={0};
    
    /** 温度报警 **/
    for(int j=0; j<3; j++)
    {
        if(Meter_run.TempAlarm[j] == 0){
            if(Meter_run.Tempert[j] >= Meter_run.FanErrTemp){
                TempAlmTime[j]++;
                if(TempAlmTime[j] >= TEMP_ALARM_TIME){
                    Meter_run.TempAlarm[j] = 1;
                    TempAlmTime[j] = TEMP_ALARM_TIME+1;
                    Meter_run.errCode.uAllErr += (1<<(3+j));
                    Save_EVENT_REC_E((1<<(3+j)), (uint16_t)(Meter_run.Tempert[j]));
                }
            }
            else{
                TempAlmTime[j] = 0;
            }
        }
        else if(Meter_run.TempAlarm[j] == 1){
            if(Meter_run.Tempert[j] < (Meter_run.FanErrTemp-5)){
                Meter_run.TempAlarm[j] = 0;
                Meter_run.errCode.uAllErr = Meter_run.errCode.uAllErr & (0x00ff & (~(1<<(3+j)))); //0x38
            }
        }      
    }
    
    /**  超温导致开启风机  **/
    if(0 != Meter_run.HandAuto){
      for(int i=0; i<3; i++){
          FanStartTime[i] = 0;
          FanCloseTime[i] = 0;
          StartFlag[i] = 0;
      }
       return;
    }
  
    for(int i=0; i<3; i++)
    {
        if(StartFlag[i] == 0){
            if(Meter_run.Tempert[i] >= Meter_run.FanStartTemp){
                FanStartTime[i]++;
                if(FanStartTime[i] >= FAN_START_TIME){
                  StartFlag[i] = 1;
                  FanCloseTime[i] = 0;                                   
                  if(Meter_run.FanCtl[i] == FAN_CTL_CLOSE){
                      Relay_Ctl(i, FAN_CTL_OPEN);      
                  }                       
                }
            }
            else{
                FanStartTime[i] = 0;
            }             
        }
        else if(StartFlag[i] == 1){
            if(Meter_run.Tempert[i] < Meter_run.FanStopTemp){
                FanCloseTime[i]++;
                if(FanCloseTime[i] >= FAN_CLOSE_TIME){
                    StartFlag[i] = 0;
                    FanStartTime[i] = 0; 
                    if(Meter_run.FanCtl[i] == FAN_CTL_OPEN){
                        Relay_Ctl(i, FAN_CTL_CLOSE);
                    }
                }
            }
            else{
                FanCloseTime[i] = 0;
            }
        }
    }
    
    /**  开启除湿装置(监测高压室的湿度)  **/
    if(Meter_run.Humidity[2] >=  Meter_run.DehumiLevel){
        KG_7_Ctl(FAN_CTL_OPEN);
    }
    else if(Meter_run.Humidity[2] <= (Meter_run.DehumiLevel-5)){
        KG_7_Ctl(FAN_CTL_CLOSE);
    }
    
    /** 定时开启风机  **/
    if(Meter_run.FanTimeInter == 0)
    {
       for(int i=0; i<3; i++)
       {//若有在运行，则关闭
          //if(Meter_run.FanStatus[i] != FAN_STATUS_STOP){                  
          //   if(Meter_run.FanStatus[i] == FAN_STATUS_FAULT){
          //      Meter_run.FanStatus[i] = FAN_STATUS_OPEN;
          //   }                         
             if(Meter_run.FanCtl[i] == FAN_CTL_OPEN) {
                Relay_Ctl(i, FAN_CTL_CLOSE);
             }
             Meter_run.TimeOpenFlag[i] = 0;           
          //}
       }
       return;
    }
    
    for(int i=0; i<3; i++){
        if(0 == Meter_run.TimeOpenFlag[i]) {
          if(Meter_run.TimeOpenCount[i] >= (Meter_run.FanTimeInter*60)){             
              Meter_run.TimeOpenFlag[i] = 1;
              Meter_run.TimeRunCount[i] = 0;
              
              if(Meter_run.FanCtl[i] == FAN_CTL_CLOSE){
                  Relay_Ctl(i, FAN_CTL_OPEN);      
              }                 
              UserEpromWriteU16((EE_TIME_RUN_1_FLAG+i*2), Meter_run.TimeOpenFlag[i]);  
          }
        }
        else if(1 == Meter_run.TimeOpenFlag[i]) {
          if((Meter_run.TimeRunCount[i] >= (Meter_run.FanTimeRun*60)) && (StartFlag[i] == 0)){
              Meter_run.TimeOpenFlag[i] = 0;
              Meter_run.TimeRunCount[i] = 0;
              
              if(Meter_run.FanCtl[i] == FAN_CTL_OPEN){
                  Relay_Ctl(i, FAN_CTL_CLOSE);
              }              
              UserEpromWriteU16((EE_TIME_RUN_1_FLAG+i*2), Meter_run.TimeOpenFlag[i]);  
          }
        }
    }
    
}

//在100ms任务事件中
#define OPEN_FAN_FAULT_TIME 50     //5秒
#define CLOSE_FAN_FAULT_TIME 50    //5秒

void fan_fault_handle(void){
  static uint16_t openfaulttime[3]={0};
  static uint16_t closefaulttime[3]={0};
  
    /**  判断切换为手动模式
         风机在自动模式下的状态  **/
    if(1 == Meter_run.HandAuto){
      for(int i=0; i<3; i++){
        if(Meter_run.FanStatus[i] == FAN_STATUS_FAULT){
          if(FAN_CTL_OPEN == Meter_run.FanCtl[i])
            Meter_run.FanStatus[i] = FAN_STATUS_OPEN;
          else if(FAN_CTL_CLOSE == Meter_run.FanCtl[i])
            Meter_run.FanStatus[i] = FAN_STATUS_STOP;
        }
      }
      return;
    }
    
    /**  风机故障判断  
         电流正常范围：90%~120%
         电流异常范围：<90% || >120%
    **/
    for(int i=0; i<3; i++){
      if(Meter_run.FanEnable[i] == 1){
        if(FAN_CTL_OPEN == Meter_run.FanCtl[i]){ //if 4
            if(((uint16_t)(Meter_run.FanCur[i]*10) < (Meter_run.FanSetCur[i]*90/100)) ||
               ((uint16_t)(Meter_run.FanCur[i]*10) > (Meter_run.FanSetCur[i]*120/100))){ //if 3
                openfaulttime[i]++;
                if(openfaulttime[i] >= OPEN_FAN_FAULT_TIME){ //if 2
                    openfaulttime[i] = (OPEN_FAN_FAULT_TIME + 1);
                    if(Meter_run.FanStatus[i] != FAN_STATUS_FAULT){ //if 1
                        Meter_run.FanStatus[i] = FAN_STATUS_FAULT;   
                        Meter_run.errCode.uAllErr += (1<<i);
                        Save_EVENT_REC_E((1<<i), (uint16_t)(Meter_run.FanCur[i]*10));
                    }//end if 1
                }//end if 2
            }//end if 3
            else{ //else 3             
              openfaulttime[i] = 0;
              if(Meter_run.FanStatus[i] == FAN_STATUS_FAULT){
                 Meter_run.FanStatus[i] = FAN_STATUS_OPEN;
                 Meter_run.errCode.uAllErr = Meter_run.errCode.uAllErr & (0x00ff & (~(1<<i))); //0x07
              }
            }//end else 3
        }//endif 4
        else if(FAN_CTL_CLOSE == Meter_run.FanCtl[i]){ //else if 4
            if((uint16_t)(Meter_run.FanCur[i]*10) >= 10){ //if 3  大于1A 
                closefaulttime[i]++;
                if(closefaulttime[i] >= CLOSE_FAN_FAULT_TIME){ //if 2
                    closefaulttime[i] = (CLOSE_FAN_FAULT_TIME + 1);
                    if(Meter_run.FanStatus[i] != FAN_STATUS_FAULT) { //if 1                  
                        Meter_run.FanStatus[i] = FAN_STATUS_FAULT;   
                        Meter_run.errCode.uAllErr += (1<<i);
                        Save_EVENT_REC_E((1<<i), (uint16_t)(Meter_run.FanCur[i]*10));
                    }//end if 1
                }//end if 2
            }//end if 3
            else if((uint16_t)(Meter_run.FanCur[i]*10) <= 2){ //else if 3 小于0.2A
                closefaulttime[i] = 0;
                if(Meter_run.FanStatus[i] == FAN_STATUS_FAULT){
                  Meter_run.FanStatus[i] = FAN_STATUS_STOP;
                  Meter_run.errCode.uAllErr = Meter_run.errCode.uAllErr & (0x00ff & (~(1<<i)));
                }
            }
        }//end else if 4
      }
      else{ //新增：风机状态判断
          if(Meter_run.FanStatus[i] == FAN_STATUS_FAULT){
            if(FAN_CTL_OPEN == Meter_run.FanCtl[i])
              Meter_run.FanStatus[i] = FAN_STATUS_OPEN;
            else if(FAN_CTL_CLOSE == Meter_run.FanCtl[i])
              Meter_run.FanStatus[i] = FAN_STATUS_STOP;
          }
          openfaulttime[i] = 0;
          closefaulttime[i] = 0;
      }
    }

}

/**
 * @brief 	 Brief description
 * @return 	 Description of return value
 * @warning	 Special notes such as non-reentrancy, blocking issues, etc.
 * @note	 Significant modifications
 */
void Fun_Debug(void){

}