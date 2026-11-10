#ifndef M24C64_H
#define M24C64_H

#include "at32f403a_407_wk_config.h"
#include "i2c_application.h"
#include "wk_i2c.h"

/*宏定义*/
#define M24C64_I2C_HANDLE       I2C2
#define M24C64_ADDR_W           0xA0
#define M24C64_ADDR_R           0xA1

#define M24C64_TOTAL_SIZE       (8*1024)
#define M24C64_PAGE_SIZE        32

#define M24Cxx_RW_TIMEOUT       1000

#define EE_WP_Low()     gpio_bits_reset(EE_WP_GPIO_PORT, EE_WP_PIN)    
#define EE_WP_High()    gpio_bits_set(EE_WP_GPIO_PORT, EE_WP_PIN)

/*结构体*/


/*全局变量*/

/*函数申明*/
void m24c64_Init(void);
uint8_t m24c64_writebyte(uint16_t addr, uint8_t data);
uint8_t m24c64_readbyte(uint16_t addr);
uint8_t m24c64_writebuff(uint16_t addr, uint8_t *buf, uint8_t len);
uint8_t m24c64_readbuff(uint16_t addr, uint8_t *buf, uint8_t len);

#endif
