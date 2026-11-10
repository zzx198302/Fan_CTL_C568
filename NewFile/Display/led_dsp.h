/* define to prevent recursive inclusion -----------------------------------*/
#ifndef __LED_DSP_H
#define __LED_DSP_H

#ifdef __cplusplus
extern "C" {
#endif

/* includes -----------------------------------------------------------------------*/
#include "aip1628_driver.h"
#include "UserVariable.h"
#include "string.h"
#include "User_head.h"

/* exported types -------------------------------------------------------------*/
typedef enum _led_digit_tubes_pos{
    LED_D_TUBE_POS_L1=0,    //左1数码管
    LED_D_TUBE_POS_L2,      //左2数码管
    LED_D_TUBE_POS_L3,      //左3数码管
    LED_D_TUBE_POS_L4,      //左4数码管
}LED_D_TUBE_POS_T;

typedef struct _led_dtube_char_s{
    char c;
    uint8_t code;
}LED_DTUBE_CHAR_S, *pLED_DTUBE_CHAR_S;

typedef enum _led_indicate_type{
    LED_I_POWER=0,      //电源指示灯
    LED_I_HANDOPT,      //手动指示灯
    LED_I_FAN1RUN,      //风机1运行指示灯
    LED_I_FAN1ALARM,    //风机1告警指示灯
    LED_I_FAN2RUN,      //风机2运行指示灯
    LED_I_FAN2ALARM,    //风机2告警指示灯
    LED_I_FAN3RUN,      //风机3运行指示灯
    LED_I_FAN3ALARM,    //风机3告警指示灯
}LED_I_TYPE_T;

typedef enum _led_indicate_onoff{
    LED_I_ON=0,     //指示灯开
    LED_I_OFF,      //指示灯关
}LED_I_ONOFF_T;

/* exported constants --------------------------------------------------------*/


/* exported macro ------------------------------------------------------------*/


/* Private defines -------------------------------------------------------------*/


/* exported functions ------------------------------------------------------- */
void led_show_char(LED_D_TUBE_POS_T pos, char c, uint8_t isDot);
void led_show_str(LED_D_TUBE_POS_T posS, char str[]);
void led_onoff(LED_I_TYPE_T type, LED_I_ONOFF_T onoff);
void led_toggle(LED_I_TYPE_T type);

void led_dsp_init(void);
void led_dsp_mode_update(void);
void led_dsp_inc_timer(void);
void led_main_update(void);

#ifdef __cplusplus
}
#endif

#endif
