#ifndef M24C02_H
#define M24C02_H

#include "at32f403a_407_wk_config.h"
#include "i2c_application.h"
#include "wk_i2c.h"

/*宏定义*/
#define M24C02_I2C_HANDLE       I2C2
#define M24C02_ADDR_W           0xA0
#define M24C02_ADDR_R           0xA1

#define M24C02_TOTAL_SIZE       256
#define M24C02_PAGE_SIZE        16

#define M24C02_RW_TIMEOUT       1000

/*结构体*/


/*全局变量*/

/*函数申明*/
void m24c02_Init(void);
void m24c02_writebyte(uint8_t addr, uint8_t data);
uint8_t m24c02_readbyte(uint8_t addr);
uint8_t m24c02_writebuff(uint8_t addr, uint8_t *buf, uint8_t len);
uint8_t m24c02_readbuff(uint8_t addr, uint8_t *buf, uint8_t len);

#endif
