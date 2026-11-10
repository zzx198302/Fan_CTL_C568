#include "aip1628_driver.h"

/* *****************************硬件***************************************** */
#define STB_LOW()       gpio_bits_reset(LED_STB_GPIO_PORT, LED_STB_PIN)
#define STB_HIGH()      gpio_bits_set(LED_STB_GPIO_PORT, LED_STB_PIN)
#define CLK_LOW()       gpio_bits_reset(LED_CLK_GPIO_PORT, LED_CLK_PIN)
#define CLK_HIGH()      gpio_bits_set(LED_CLK_GPIO_PORT, LED_CLK_PIN)
#define DIO_LOW()       gpio_bits_reset(LED_DIO_GPIO_PORT, LED_DIO_PIN)
#define DIO_HIGH()      gpio_bits_set(LED_DIO_GPIO_PORT, LED_DIO_PIN)

/* *****************************底层***************************************** */

/**************************************************************************/
/*!
    @brief  写一个字节
    
    @params[in] type
                发送类型：见SEND_1628_T定义
    @params[in] byte
                要发送的字节
    @returns
                None
*/
/**************************************************************************/
static void send_1628_byte(SEND_1628_T type, uint8_t byte){
    //发送前
    if(STB_S == type || STB_S_E == type){ STB_LOW(); }
    //发送
    for(int i=0; i<8; i++){
        CLK_LOW();
        if(byte & 0x01){
            DIO_HIGH();
        }else{
            DIO_LOW();
        }
        byte >>= 1;
        CLK_HIGH();
    }
    //发送后
    if(STB_E == type || STB_S_E == type){ STB_HIGH(); }
}

/**************************************************************************/
/*!
    @brief  设置显示模式
    
    @params[in] mode
                见SEND_1628_T定义
    @returns
                None
*/
/**************************************************************************/
void set_1628_display_mode(DSP_MODE_1628_T mode){
    uint8_t cmd = (uint8_t)mode;
    send_1628_byte(STB_S_E, cmd);
}

/**************************************************************************/
/*!
    @brief  数据设置
    
    @params[in] rwmode
                读写模式，0x00: 写; 0x02: 读;
    @params[in] addrmode
                地址模式，0x00: 地址自加; 0x04: 固定地址;
    @returns
                None
*/
/**************************************************************************/
void set_1628_data_mode(uint8_t rwmode, uint8_t addrmode){
    uint8_t cmd = 0x40;
    cmd |= rwmode;
    cmd |= addrmode;
    send_1628_byte(STB_S_E, cmd);
}

/**************************************************************************/
/*!
    @brief  设置显示地址
    
    @params[in] addr
                0x00 ~ 0x0D
    @returns
                None
*/
/**************************************************************************/
void set_1628_display_addr(uint8_t addr){
    send_1628_byte(STB_S, (addr|0xC0));
}

/**************************************************************************/
/*!
    @brief  显示控制
    
    @params[in] bright
                亮度，见DSP_BRIGHT_1628_T定义
    @params[in] onoff
                显示开关，0x00: 显示关; 0x08: 显示开;
    @returns
                None
*/
/**************************************************************************/
void set_1628_brightness(DSP_BRIGHT_1628_T bright, uint8_t onoff){
    uint8_t cmd = 0x80;
    cmd |= (uint8_t)bright;
    cmd |= onoff;
    send_1628_byte(STB_S_E, cmd);
}

/* *****************************应用***************************************** */
/**************************************************************************/
/*!
    @brief  初始化
    @returns
                None
*/
/**************************************************************************/
void dsp_1628_init(void){
    //设置显示模式
    set_1628_display_mode(MODE_7GRID_10SEG);
    //设置数据指令
    set_1628_data_mode(0, 0);
    //设置显示地址
    set_1628_display_addr(0);
    //清空显示RAM
    for(int i=0; i<14; i++){
        send_1628_byte(((i == 0x0D)? STB_E : STB_NONE), 0);
    }
    //显示控制
    set_1628_brightness(BRIGHT_P14_16, 0x08);
}

/**************************************************************************/
/*!
    @brief  地址自增模式的显示
    
    @params[in] addrS
                写进显示RAM的起始地址;
    @params[in] data[]
                写进显示RAM的数据buffer;
    @params[in] len
                写进显示RAM的数据长度;
    @returns
                None
*/
/**************************************************************************/
void dsp_1628_show(uint8_t addrS, uint8_t data[], uint8_t len){
    //设置数据指令
    set_1628_data_mode(0, 0);
    //设置显示地址
    set_1628_display_addr(addrS);
    //写入显示RAM
    for(int i=0; i<len; i++){
        send_1628_byte(((i == len-1)? STB_E : STB_NONE), data[i]);
    }
    //显示控制
    set_1628_brightness(BRIGHT_P14_16, 0x08);
}

/**************************************************************************/
/*!
    @brief  地址固定模式的显示
    
    @params[in] addr[]
                写进显示RAM的地址buffer;
    @params[in] data[]
                写进显示RAM的数据buffer;
    @params[in] len
                写进显示RAM的数据长度;
    @returns
                None
*/
/**************************************************************************/
void dsp_1628_show_fixAddr(uint8_t addr[], uint8_t data[], uint8_t len){
    //设置数据指令
    set_1628_data_mode(0, 4);
    //
    for(int i=0; i<len; i++){
        //设置显示地址
        set_1628_display_addr(addr[i]);
        //写入显示RAM
        send_1628_byte(STB_E, data[i]);
    }
    //显示控制
    set_1628_brightness(BRIGHT_P14_16, 0x08);
}
