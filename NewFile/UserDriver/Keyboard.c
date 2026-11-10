#include "User_head.h"


#define KEY_CHECK(key, sign)    \
    do{\
        if(!key) { LOS_TaskDelay(5); if(!key){ re_key += sign;} }\
    }while(0)

//按键按下判断及按下的键值
static uint16_t Keyboard_data(void)
{
    uint16_t re_key = 0;

    KEY_CHECK(KEY_SET,        KEY_PRESS_SIGN(KEY_SET_S_SHIFT));
    KEY_CHECK(KEY_DOWN,       KEY_PRESS_SIGN(KEY_DOWN_S_SHIFT));
    KEY_CHECK(KEY_UP,         KEY_PRESS_SIGN(KEY_UP_S_SHIFT));
    KEY_CHECK(KEY_ENT,        KEY_PRESS_SIGN(KEY_ENT_S_SHIFT));
    KEY_CHECK(KEY_HAND_AUTO,  KEY_PRESS_SIGN(KEY_HAND_AUTO_S_SHIFT));
    KEY_CHECK(KEY_FAN1_ST,    KEY_PRESS_SIGN(KEY_FAN1_ST_S_SHIFT));
    KEY_CHECK(KEY_FAN2_ST,    KEY_PRESS_SIGN(KEY_FAN2_ST_S_SHIFT));
    KEY_CHECK(KEY_FAN3_ST,    KEY_PRESS_SIGN(KEY_FAN3_ST_S_SHIFT));

    return re_key;
}

//确定按键键值及按键松开
static uint16_t Keyboard_result(void)
{
    uint16_t re_key;
    re_key = Keyboard_data();
    while(Keyboard_data() != 0)
    {
        LOS_TaskDelay(20);
    }
    return re_key;
}

//按键处理
void Keyboard_pulic(void)
{
    uint16_t keyb = Keyboard_result();

    if(keyb && !Meter_Surface.Keyb)
    {
        //未处理完则丢弃本次按键结果
        Meter_Surface.Keyb = keyb;
    }
}
