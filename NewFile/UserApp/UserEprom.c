/*************************************************************** 
 * @FilePath: \....\NewFile\UserApp\UserEprom.c
 * @Version: V1.0.0
 * @Author: ZhongZhanxiong
 * @Date: 2026-05-19 18:11:31
 * @LastEditors: ZhongZhanxiong
 * @LastEditTime: 2026-05-20 11:16:55
 * @Description: 
 * @Copyright (c) 2025 by PengYang, All Rights Reserved. 
 ***************************************************************/
#include "M24C64.h"

/********************************************************************************
* Function Name  : void UserEpromSetWP(confirm_state isEnabled)
* Description    : 设置EEPROM写
* Input          : 是否写
* Output         :
* Return         :
* Author         ：
* Date           ：
*******************************************************************************/  
void UserEpromSetWP(confirm_state isEnabled){
    if(isEnabled)
      EE_WP_Low();
    else
      EE_WP_High();
}

void UserEpromInit(void)
{
    m24c64_Init();
}

void UserEpromWriteU8(uint16_t addr, uint8_t val){   
    UserEpromSetWP(TRUE);
    m24c64_writebyte(addr, val);
    UserEpromSetWP(FALSE);
}

uint8_t UserEpromReadU8(uint16_t addr){
    uint8_t val = 0;
    val = m24c64_readbyte(addr);
    return val; 
}

void UserEpromWriteU16(uint16_t addr, uint16_t val){
    uint8_t *tempbuf;
    tempbuf = (uint8_t *)&val;   //低字节在前
  
    UserEpromSetWP(TRUE);
    m24c64_writebuff(addr, tempbuf, 2);
    UserEpromSetWP(FALSE);
}

uint16_t UserEpromReadU16(uint16_t addr){
    uint16_t retVal = 0;
    uint8_t  tempbuf[2];
    
    m24c64_readbuff(addr, tempbuf, 2);
    retVal = (tempbuf[1]<<8) + tempbuf[0];
    
    return retVal;
}


