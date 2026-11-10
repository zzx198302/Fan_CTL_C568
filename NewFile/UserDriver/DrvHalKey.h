#ifndef DRVHALKEY_H
#define DRVHALKEY_H

#include "at32f403a_407_wk_config.h"

/*宏定义*/
#define KEY_AMOUNT          8                           //按键数量
#define KEY_STEP_PRESS      0                           //按键步骤：按下
#define KEY_STEP_FILTER     1                           //按键步骤：滤波
#define KEY_STEP_FREE       2                           //按键步骤：释放
#define KEY_FILTER_TIME     5                           //按键滤波时间：25ms，任务调用该函数的间隔是5ms
#define KEY_LONG_PRESS_TIME (KEY_FILTER_TIME * 25)      //长按时间：1000ms
#define KEY_RESET_TIME      (KEY_LONG_PRESS_TIME * 5)   //按键状态重置时间：5s，即，按键5s时间后应用程序未响应，则重置按键状态

/*结构体*/
typedef struct {
    unsigned char keyStep;              //按键步骤
    unsigned char spFlag;               //短按标志
    unsigned char lpFlag;               //长按标志
    unsigned char lpFlagBak;            //长按标志备份
}Key_Scan_Str;

/*全局变量*/

/*函数申明*/
void DrvHalKeyScanTick(void);
unsigned short DrvHalKeyRead(void);

#endif
