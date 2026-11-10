/*******************************************************************
 * @FilePath: \....\NewFile\UserApp\UserVariable.c
 * @Version: V1.0.0
 * @Author: ZhongZhanxiong
 * @Date: 2026-05-19 18:11:31
 * @LastEditors: ZhongZhanxiong
 * @LastEditTime: 2026-05-20 11:16:55
 * @Description: EEPROM读写，M24C02容量是2048bit（即：256Byte）
                 存储1页为16Byte,同时存参数时，2个字节做一个变量
 * @Copyright (c) 2025 by PengYang, All Rights Reserved. 
********************************************************************/
#include "M24C02.h"

i2c_handle_type hi2c2;

void delayMs(uint16_t ms)
{
    for(int i=0; i<240; i++){
        __NOP();
    }
}
   
void m24c02_Init(void){
    wk_i2c2_init();
    
    hi2c2.i2cx = M24C02_I2C_HANDLE;
}

// 写1字节
void m24c02_writebyte(uint8_t addr, uint8_t data){
    uint8_t result = 0;
    uint8_t buf[2];
    buf[0] = addr;   // 内部地址
    buf[1] = data;   // 数据
    
    result = i2c_master_transmit(&hi2c2, M24C02_ADDR_W, buf, 2, M24C02_RW_TIMEOUT);
    delayMs(5); // EEPROM 写周期
}

// 读1字节
uint8_t m24c02_readbyte(uint8_t addr){
    uint8_t data = 0;
    
    i2c_master_transmit(&hi2c2, M24C02_ADDR_W, &addr, 1, M24C02_RW_TIMEOUT);
    i2c_master_receive(&hi2c2, M24C02_ADDR_R, &data, 1, M24C02_RW_TIMEOUT);
    
    return data;
}

// 多字节写（自动分页）
uint8_t m24c02_writebuff(uint8_t addr, uint8_t *buf, uint8_t len){
    uint8_t i, cnt;
	uint8_t tempbuf[40];
	
    if (addr + len > M24C02_TOTAL_SIZE) return 1;
    
    while(len > 0){
        cnt = M24C02_PAGE_SIZE - (addr % M24C02_PAGE_SIZE);
        if (cnt > len) cnt = len;
		
		memset(tempbuf, 0, 40);
		tempbuf[0] = addr;
		for(i=0; i<cnt; i++){
			tempbuf[1+i] = buf[i];
		}
		
        i2c_master_transmit(&hi2c2, M24C02_ADDR_W, &tempbuf[0], (1+cnt), M24C02_RW_TIMEOUT);
        delayMs(5);

        addr += cnt;
        buf  += cnt;
        len  -= cnt;
    }
    return 0;
}


// 多字节读
uint8_t m24c02_readbuff(uint8_t addr, uint8_t *buf, uint8_t len){
    if (addr + len > M24C02_TOTAL_SIZE) return 1;

    i2c_master_transmit(&hi2c2, M24C02_ADDR_W, &addr, 1, M24C02_RW_TIMEOUT);
    i2c_master_receive(&hi2c2, M24C02_ADDR_R, buf, len, M24C02_RW_TIMEOUT);
    return 0;
}

