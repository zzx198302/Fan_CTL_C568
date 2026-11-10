/* define to prevent recursive inclusion -----------------------------------*/
#ifndef __AIP1628_H
#define __AIP1628_H

#ifdef __cplusplus
extern "C" {
#endif

/* includes -----------------------------------------------------------------------*/
#include "at32f403a_407_wk_config.h"

/* exported types -------------------------------------------------------------*/
typedef enum _send_1628_type{
    STB_S_E=0,  //片选脚在发送前拉低，发送后拉高
    STB_S,      //片选脚在发送前拉低，发送后无动作
    STB_E,      //片选脚在发送前无动作，发送后拉高
    STB_NONE,   //片选脚在发送前后均无动作
}SEND_1628_T;

typedef enum _1628_dsp_mode{
    MODE_4GRID_13SEG=0, //4位13段
    MODE_5GRID_12SEG,   //5位12段
    MODE_6GRID_11SEG,   //6位11段
    MODE_7GRID_10SEG,   //7位10段
}DSP_MODE_1628_T;

typedef enum _1628_dsp_bright{
    BRIGHT_P1_16=0,     //脉冲宽度1/16
    BRIGHT_P2_16,       //脉冲宽度2/16
    BRIGHT_P4_16,       //脉冲宽度4/16
    BRIGHT_P10_16,      //脉冲宽度10/16
    BRIGHT_P11_16,      //脉冲宽度11/16
    BRIGHT_P12_16,      //脉冲宽度12/16
    BRIGHT_P13_16,      //脉冲宽度13/16
    BRIGHT_P14_16,      //脉冲宽度14/16
}DSP_BRIGHT_1628_T;
/* exported constants --------------------------------------------------------*/


/* exported macro ------------------------------------------------------------*/


/* Private defines -------------------------------------------------------------*/


/* exported functions ------------------------------------------------------- */
void dsp_1628_init(void);
void dsp_1628_show(uint8_t addrS, uint8_t data[], uint8_t len);
void dsp_1628_show_fixAddr(uint8_t addr[], uint8_t data[], uint8_t len);

#ifdef __cplusplus
}
#endif

#endif
