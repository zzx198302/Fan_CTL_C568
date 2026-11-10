/*************************************************************** 
 * @FilePath: \...\NewFile\UserDriver\DrvHalKey.c
 * @Version: V1.0.0
 * @Author: ZhongZhanxiong
 * @Date: 2026-05-12 14:46:04
 * @LastEditors: ZhongZhanxiong
 * @LastEditTime: 2025-06-27 13:55:35
 * @Description: 按键扫描驱动
 * @
 * @Copyright (c) 2026 by PengYang, All Rights Reserved. 
 ***************************************************************/
#include "DrvHalKey.h"
#include "../UserApp/UserVariable.h"
#include "wk_gpio.h"

/*宏定义*/

/*全局变量*/
Key_Scan_Str g_stKeyScanBuf[KEY_AMOUNT] = {0};

/************************************ 
 * @description: 按键pin脚状态读取
 * @param {unsigned char} ch：通道号（0~7）
 * @return {*}按键按下返回1，释放返回0
 *************************************/
unsigned char DrvHalKeyReadPin(unsigned char ch)
{
    unsigned char ret = 0;

    switch (ch)
    {
    case 0:
        ret = (SET == gpio_input_data_bit_read(SW1_GPIO_PORT,SW1_PIN))? 0:1;
        break;
    case 1:
        ret = (SET == gpio_input_data_bit_read(SW2_GPIO_PORT,SW2_PIN))? 0:1;
        break;
    case 2:
        ret = (SET == gpio_input_data_bit_read(SW3_GPIO_PORT,SW3_PIN))? 0:1;
        break;
    case 3:
        ret = (SET == gpio_input_data_bit_read(SW4_GPIO_PORT,SW4_PIN))? 0:1;
        break;
    
    case 4:
        ret = (SET == gpio_input_data_bit_read(SW5_GPIO_PORT,SW5_PIN))? 0:1;
        break;
    case 5:
        ret = (SET == gpio_input_data_bit_read(SW6_GPIO_PORT,SW6_PIN))? 0:1;
        break;
    case 6:
        ret = (SET == gpio_input_data_bit_read(SW7_GPIO_PORT,SW7_PIN))? 0:1;
        break;
    case 7:
        ret = (SET == gpio_input_data_bit_read(SW8_GPIO_PORT,SW8_PIN))? 0:1;
        break;
    default:
        break;
    }

    return ret;
}
/************************************ 
 * @description: 按键扫描tick函数，放在主循环中使用
 * @return {*}
 *************************************/
void DrvHalKeyScanTick(void)
{
    static unsigned int lastFilterTimeBuf[KEY_AMOUNT] = {0};
    unsigned char i = 0;

    for (i = 0; i < KEY_AMOUNT; i++)
    {
        switch (g_stKeyScanBuf[i].keyStep)
        {
        case KEY_STEP_PRESS:
        {
            if ((0 == g_stKeyScanBuf[i].lpFlag) && (0 == g_stKeyScanBuf[i].spFlag)){
                if (1 == DrvHalKeyReadPin(i)){
                    g_stKeyScanBuf[i].keyStep = KEY_STEP_FILTER;
                    lastFilterTimeBuf[i] = 0;
                }
            }
            else{
                if (0 == DrvHalKeyReadPin(i)){
                    if (KEY_RESET_TIME > lastFilterTimeBuf[i]){
                        lastFilterTimeBuf[i]++;
                    }
                    else{
                        g_stKeyScanBuf[i].spFlag = 0;
                        g_stKeyScanBuf[i].lpFlag = 0;
                    }
                }
            }
            break;
        }
        case KEY_STEP_FILTER:
        {
            if (1 == DrvHalKeyReadPin(i)){
                if (KEY_FILTER_TIME < lastFilterTimeBuf[i]){
                    g_stKeyScanBuf[i].keyStep = KEY_STEP_FREE;
                }
                else {
                    lastFilterTimeBuf[i]++;
                }
            }
            else{
                g_stKeyScanBuf[i].keyStep = KEY_STEP_PRESS;
            }
            break;
        }
        case KEY_STEP_FREE:
        {
            if (0 == DrvHalKeyReadPin(i)){
                g_stKeyScanBuf[i].keyStep = KEY_STEP_PRESS;
                g_stKeyScanBuf[i].lpFlag = 0;
                g_stKeyScanBuf[i].spFlag = 1;
                lastFilterTimeBuf[i] = 0;
            }
            else {
                if (KEY_LONG_PRESS_TIME < lastFilterTimeBuf[i]){
                    g_stKeyScanBuf[i].keyStep = KEY_STEP_PRESS;
                    g_stKeyScanBuf[i].lpFlag = 1;
                    g_stKeyScanBuf[i].spFlag = 0;
                    lastFilterTimeBuf[i] = 0;
                }
                else{
                    lastFilterTimeBuf[i]++;
                }
            }
            break;
        } 
        default:
            break;
        }
    }
}
/************************************ 
 * @description: 按键状态读取
 * @param {*} 
 * @return {*}
 * 当长按和短按同时触发时，优先处理长按，忽略短按
 *************************************/
unsigned short DrvHalKeyRead(void)
{
    unsigned short ret = 0;
    int ch = 0;

    /************************************************
    按键：1-”设置/左“ 、2-”增加“ 、3-”减少“、4-”确定“
    按键：5-“手动/自动”、6-“风机1启/停”、7-“风机2启/停”
          8-“风机3启/停”
    长按1/4按键，键值1/4；长按2/3按键，键值11/12；
    长按5-8按键，键值21/22/23/24
    ************************************************/
    if(1 == g_stKeyScanBuf[0].lpFlag)
    {
        g_stKeyScanBuf[0].lpFlag = 0;
        return 1;
    }
    if(1 == g_stKeyScanBuf[1].lpFlag)
    {
         g_stKeyScanBuf[1].lpFlag = 0;
         return 11;
    }
    if(1 == g_stKeyScanBuf[2].lpFlag)
    {
         g_stKeyScanBuf[2].lpFlag = 0;
         return 12;
    }
    if(1 == g_stKeyScanBuf[3].lpFlag)
    {
         g_stKeyScanBuf[3].lpFlag = 0;
         return 4;
    }
    
    for(ch=4; ch<8; ch++)
    {
        if (1 == g_stKeyScanBuf[ch].lpFlag){      
            g_stKeyScanBuf[ch].lpFlag = 0;
            ret = 21+(ch-4);
            if(ret != 0)
              return ret;
        }
    }
    /************************************************
    按键：1-”设置/左“ 、2-”增加“ 、3-”减少“、4-”确定“
    按键：5-“手动/自动”、6-“风机1启/停”、7-“风机2启/停”
          8-“风机3启/停”
    短按1-4按键，键值1/2/3/4
    短按5-8按键，键值21/22/23/24
    ************************************************/  
    for(ch=0; ch<4; ch++)
    {
        if (1 == g_stKeyScanBuf[ch].spFlag){      
            g_stKeyScanBuf[ch].spFlag = 0;
            ret = (ch+1);
            if(ret != 0)
              return ret;
        }
    }
    
    for(ch=4; ch<8; ch++)
    {
        if (1 == g_stKeyScanBuf[ch].spFlag){      
            g_stKeyScanBuf[ch].spFlag = 0;
            ret = 21+(ch-4);
            if(ret != 0)
              return ret;
        }
    }

    return ret;
}
