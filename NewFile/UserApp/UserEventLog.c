/*************************************************************** 
 * @FilePath: \....\NewFile\UserApp\UserEventLog.c
 * @Version: V1.0.0
 * @Author: ZhongZhanxiong
 * @Date: 2026-06-03 10:11:31
 * @LastEditors: ZhongZhanxiong
 * @LastEditTime: 2026-06-03 11:16:55
 * @Description: 
 * @Copyright (c) 2025 by PengYang, All Rights Reserved. 
 ***************************************************************/
#include "User_head.h"
#include "UserVariable.h"

/*******************************************************************************
函数名称：Save_EVENT_REC_E
函数功能：保存事件记录
函数输入：EVENT:故障，EVENT_ED:故障值
函数返回：NULL
*******************************************************************************/   
void Save_EVENT_REC_E(uint16_t EVENT,uint16_t EVENT_ED)
{
    /* 避免编译器告警 */
    uint16_t year = calendar.year - 2000;
    uint8_t month, date, hour, min, sec;
    month = calendar.month; date = calendar.date; hour = calendar.hour;
    min = calendar.min; sec = calendar.sec;

    EVENT_LOG.Event_Record_E[0] = (year << 8) + month;
    EVENT_LOG.Event_Record_E[1] = ((uint16_t)date << 8) + hour;
    EVENT_LOG.Event_Record_E[2] = ((uint16_t)min << 8) + sec;
    EVENT_LOG.Event_Record_E[3] = EVENT;
    EVENT_LOG.Event_Record_E[4] = EVENT_ED;

    UserEpromWriteU16(EE_EVENT_LOG_RECORD+10*EVENT_LOG.E_Present_Save_ad+0, EVENT_LOG.Event_Record_E[0]);
    UserEpromWriteU16(EE_EVENT_LOG_RECORD+10*EVENT_LOG.E_Present_Save_ad+2, EVENT_LOG.Event_Record_E[1]);
    UserEpromWriteU16(EE_EVENT_LOG_RECORD+10*EVENT_LOG.E_Present_Save_ad+4, EVENT_LOG.Event_Record_E[2]);
    UserEpromWriteU16(EE_EVENT_LOG_RECORD+10*EVENT_LOG.E_Present_Save_ad+6, EVENT_LOG.Event_Record_E[3]);
    UserEpromWriteU16(EE_EVENT_LOG_RECORD+10*EVENT_LOG.E_Present_Save_ad+8, EVENT_LOG.Event_Record_E[4]);

    EVENT_LOG.E_Present_Save_ad++;
    if(EVENT_LOG.E_Present_Save_ad > 99) EVENT_LOG.E_Present_Save_ad = 0;
    UserEpromWriteU16(EE_EVENT_LOG_POS, EVENT_LOG.E_Present_Save_ad);
    EVENT_LOG.E_Present_Save_ad_bak = EVENT_LOG.E_Present_Save_ad;

    EVENT_LOG.E_Total_Num++;
    if(EVENT_LOG.E_Total_Num >= 100) EVENT_LOG.E_Total_Num = 100;
    UserEpromWriteU16(EE_EVENT_LOG_TOTALNUM, EVENT_LOG.E_Total_Num);
}

