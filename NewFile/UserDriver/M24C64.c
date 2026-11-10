/*******************************************************************
 * @FilePath: \....\NewFile\UserApp\UserVariable.c
 * @Version: V1.0.0
 * @Author: ZhongZhanxiong
 * @Date: 2026-05-19 18:11:31
 * @LastEditors: ZhongZhanxiong
 * @LastEditTime: 2026-05-20 11:16:55
 * @Description: EEPROM读写，M24C64容量是64kbit（即：8kByte）
                 存储1页为32Byte,同时存参数时，2个字节做一个变量
 * @Copyright (c) 2025 by PengYang, All Rights Reserved. 
********************************************************************/
#include "M24C64.h"

#define     USE_OS      1        
i2c_handle_type hi2c2;

void delayus(uint16_t us)
{
    for(int i=0; i<240; i++){
        __NOP();
    }
}

void delayms(uint16_t ms)
{
  #if USE_OS
      LOS_TaskDelay(ms);
  #else
      wk_delay_ms(ms);
  #endif
}
   
void m24c64_Init(void){
    //wk_i2c2_init();
    
    hi2c2.i2cx = M24C64_I2C_HANDLE;
}

/********************************************************************************
* Function Name  : uint8_t m24c64_writebyte(uint16_t addr, uint8_t data)
* Description    : eeprom写1个字节
* Input          : 
* Output         :
* Return         : 0-成功，其他值-失败
* Author         ：
* Date           ：
*******************************************************************************/
uint8_t m24c64_writebyte(uint16_t addr, uint8_t data){
    uint8_t result = 0;
    uint8_t tempbuf[3];
    
    tempbuf[0] = (addr >> 8) & 0xff;   // 内部地址
    tempbuf[1] = (addr >> 0) & 0xff;
    tempbuf[2] = data;   // 数据   
    result = i2c_master_transmit(&hi2c2, M24C64_ADDR_W, tempbuf, 3, M24Cxx_RW_TIMEOUT);
    
    delayms(5); // EEPROM 写周期
    return result;
}

/********************************************************************************
* Function Name  : uint8_t m24c64_readbyte(uint16_t addr)
* Description    : eeprom读1个字节
* Input          : 
* Output         :
* Return         : 读取值
* Author         ：
* Date           ：
*******************************************************************************/
uint8_t m24c64_readbyte(uint16_t addr){
    uint8_t data = 0;
    uint8_t tempbuf[2];
    
    tempbuf[0] = (addr >> 8) & 0xff;   // 内部地址
    tempbuf[1] = (addr >> 0) & 0xff;
    i2c_master_transmit(&hi2c2, M24C64_ADDR_W, &tempbuf[0], 2, M24Cxx_RW_TIMEOUT);
    i2c_master_receive(&hi2c2, M24C64_ADDR_R, &data, 1, M24Cxx_RW_TIMEOUT);
    
    return data;
}

/********************************************************************************
* Function Name  : uint8_t m24c64_writebuff(uint16_t addr, uint8_t *buf, uint8_t len)
* Description    : 多字节写（自动分页）,物理地址的分页
* Input          : 
* Output         :
* Return         :  1-失败 0-成功
* Author         ：
* Date           ：
*******************************************************************************/
uint8_t m24c64_writebuff(uint16_t addr, uint8_t *buf, uint8_t len){
    uint8_t i, cnt;
    uint8_t tempbuf[40];
    
    if (addr + len > M24C64_TOTAL_SIZE) return 1;
    
    while(len > 0){
        cnt = M24C64_PAGE_SIZE - (addr % M24C64_PAGE_SIZE);
        if (cnt > len) cnt = len;
        
        memset(tempbuf, 0, 40);
        tempbuf[0] = (addr >> 8) & 0xff;   // 内部地址
        tempbuf[1] = (addr >> 0) & 0xff;
        
        for(i=0; i<cnt; i++){
            tempbuf[2+i] = buf[i];
        }
        
        i2c_master_transmit(&hi2c2, M24C64_ADDR_W, tempbuf, (2+cnt), M24Cxx_RW_TIMEOUT);     
        delayms(5);

        addr += cnt;
        buf  += cnt;
        len  -= cnt;
    }
    return 0;
}

/********************************************************************************
* Function Name  : uint8_t m24c64_readbuff(uint16_t addr, uint8_t *buf, uint8_t len)
* Description    : 多字节读
* Input          : 
* Output         :
* Return         :  1-失败 0-成功
* Author         ：
* Date           ：
*******************************************************************************/
uint8_t m24c64_readbuff(uint16_t addr, uint8_t *buf, uint8_t len){
    uint8_t tempbuf[2];
    
    if (addr + len > M24C64_TOTAL_SIZE) return 1;
    
    tempbuf[0] = (addr >> 8) & 0xff;   // 内部地址
    tempbuf[1] = (addr >> 0) & 0xff;
    i2c_master_transmit(&hi2c2, M24C64_ADDR_W, tempbuf, 2, M24Cxx_RW_TIMEOUT);
    i2c_master_receive(&hi2c2, M24C64_ADDR_R, buf, len, M24Cxx_RW_TIMEOUT);
    return 0;
}



