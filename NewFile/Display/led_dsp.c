#include "led_dsp.h"

#if 1   //自检用
#include "wk_system.h"
#endif

/* ***************************数码管显示************************************* *
           |     bit0           |     bit0           |
           |  b        b        |  b        b        |
           |  i        i        |  i        i        |
           |  t        t        |  t        t        |
           |  5        1        |  5        1        |
           |     bit6           |     bit6           |
           |  b        b        |  b        b        |
           |  i        i        |  i        i        |
           |  t        t        |  t        t        |
           |  4        2        |  4        2        |
           |     bit3     bit7  |     bit3     bit7  |...
*******************************************************************************
寄存器地址 |         00H        |         02H        |...|08H
*******************************************************************************
数码管位置 |        左1         |         左2        |...|左4
*/

/* 数码管显示-字符对照表 */
LED_DTUBE_CHAR_S DSP_CHAR_MAP[] = {
    {.c = 'A', .code = 0x77},
    {.c = 'b', .code = 0x7C},
    {.c = 'C', .code = 0x39},
    {.c = 'd', .code = 0x5E},
    {.c = 'E', .code = 0x79},
    {.c = 'F', .code = 0x71},
    {.c = 'G', .code = 0x3D},
    {.c = 'H', .code = 0x76},
    {.c = 'I', .code = 0x0F},
    {.c = 'J', .code = 0x0E},
    {.c = 'K', .code = 0x0E},
    {.c = 'L', .code = 0x38},
    {.c = 'M', .code = 0x37},
    {.c = 'n', .code = 0x54},
    {.c = 'O', .code = 0x5C},
    {.c = 'P', .code = 0x73},
    {.c = 'q', .code = 0x67},
    {.c = 'r', .code = 0x31},
    {.c = 'S', .code = 0x49},
    {.c = 't', .code = 0x78},
    {.c = 'U', .code = 0x3E},
    {.c = 'v', .code = 0x1C},
    {.c = 'W', .code = 0x7E},
    {.c = 'X', .code = 0x64},
    {.c = 'Y', .code = 0x6E},
    {.c = 'Z', .code = 0x5A},
    {.c = '1', .code = 0x06},
    {.c = '2', .code = 0x5B},
    {.c = '3', .code = 0x4F},
    {.c = '4', .code = 0x66},
    {.c = '5', .code = 0x6D},
    {.c = '6', .code = 0x7D},
    {.c = '7', .code = 0x27},
    {.c = '8', .code = 0x7F},
    {.c = '9', .code = 0x6F},
    {.c = '0', .code = 0x3F},
    {.c = ' ', .code = 0x00},
    {.c = '-', .code = 0x40},
};

/**************************************************************************/
/*!
    @brief  根据字符从数码管显示-字符对照表中找出显示码
    
    @params[in] c
                要显示的字符
    @returns
                字符对应的显示码，找不到则不显示
*/
/**************************************************************************/
static uint8_t find_code_from_char(char c){
    uint8_t ret = 0;
    for(int i=0; i<sizeof(DSP_CHAR_MAP)/sizeof(LED_DTUBE_CHAR_S); i++){
        if(c == DSP_CHAR_MAP[i].c){
            ret = DSP_CHAR_MAP[i].code;
            break;
        }
    }
    return ret;
}

/**************************************************************************/
/*!
    @brief  数码管显示一个字符
    
    @params[in] pos
                数码管位置：见LED_D_TUBE_POS_T定义
    @params[in] c
                要显示的字符
    @params[in] isDot
                是否显示小数点，0: 否; 1: 是;
    @returns
                None
*/
/**************************************************************************/
void led_show_char(LED_D_TUBE_POS_T pos, char c, uint8_t isDot){
    uint8_t addr = (uint8_t)pos * 2;
    uint8_t data = find_code_from_char(c);
    data |= (isDot? 0x80 : 0x00);
    dsp_1628_show(addr, &data, 1);
}

/**************************************************************************/
/*!
    @brief  数码管显示字符串
    
    @params[in] posS
                数码管起始位置：见LED_D_TUBE_POS_T定义
    @params[in] str
                要显示的字符串
    @returns
                None
*/
/**************************************************************************/
void led_show_str(LED_D_TUBE_POS_T posS, char str[]){
    uint8_t addr[6] = {0};
    uint8_t data[6] = {0};

    int i=0;
    char *pStr = str;
    while(*pStr){
        addr[i] = (uint8_t)(posS + i) * 2;
        if(addr[i] > ((uint8_t)LED_D_TUBE_POS_L4 * 2))break;
        data[i] = find_code_from_char(*pStr);
        if(!(*(pStr+1))){
            i++;
            break;
        }
        else if((*(pStr+1)) == '.'){
            data[i] |= 0x80;
            pStr++;
        }

        pStr++;
        i++;
    }
    dsp_1628_show_fixAddr(addr, data, i);
}

/**************************************************************************/
/*!
    @brief  数码管显示字符串,并对第N个数码管闪烁
    
    @params[in] posS
                数码管起始位置：见LED_D_TUBE_POS_T定义
    @params[in] str
                要显示的字符串
    @params[in] fpos
                要闪烁的数码管位置（从右开始）
    @returns
                None
*/
/**************************************************************************/
void led_show_str_flick(LED_D_TUBE_POS_T posS, char str[], char fpos){
    static uint8_t flickFlag = 0;
    uint8_t addr[6] = {0};
    uint8_t data[6] = {0};

    int i=0;
    char *pStr = str;
    while(*pStr){
        addr[i] = (uint8_t)(posS + i) * 2;
        if(addr[i] > ((uint8_t)LED_D_TUBE_POS_L4 * 2))break;
        data[i] = find_code_from_char(*pStr);
        
        if(((3-i) == fpos) && (fpos <= 3))
        {
            if(0 == flickFlag)
              data[i] = 0x00;
              
            flickFlag = !flickFlag;
        }
        if(!(*(pStr+1))){
            i++;
            break;
        }
        else if((*(pStr+1)) == '.'){
            data[i] |= 0x80;
            pStr++;
        }

        pStr++;
        i++;
    }
    dsp_1628_show_fixAddr(addr, data, i);
}

/* ***************************指示灯显示************************************* *
寄存器地址 |         0CH        |
    位     | bit0 |   bit1   |   bit2    |   bit3    |   bit4    |     bit5  |    bit6   |    bit7   |
对应指示灯 | 电源 | 手动     | 风机1运行 | 风机1告警 | 风机2运行 | 风机2告警 | 风机3运行 | 风机3告警 |
*******************************************************************************
*/
static uint8_t led_indicate_state = 0;

/**************************************************************************/
/*!
    @brief  指示灯亮灭控制
    
    @params[in] type
                指示灯类型：见LED_I_TYPE_T定义
    @params[in] onoff
                指示灯控制：见LED_I_ONOFF_T定义
    @returns
                None
*/
/**************************************************************************/
void led_onoff(LED_I_TYPE_T type, LED_I_ONOFF_T onoff){
    uint8_t data = (1 << (uint8_t)type);
    if(LED_I_ON == onoff){
        if(!(led_indicate_state & data)){
            led_indicate_state |= data;
            dsp_1628_show(0x0A, &led_indicate_state, 1);
        }
    }else{
        if(led_indicate_state & data){
            led_indicate_state &= (~data);
            dsp_1628_show(0x0A, &led_indicate_state, 1);
        }
    }
}

/**************************************************************************/
/*!
    @brief  翻转指示灯亮灭状态
    
    @params[in] type
                指示灯类型：见LED_I_TYPE_T定义
    @returns
                None
*/
/**************************************************************************/
void led_toggle(LED_I_TYPE_T type){
    uint8_t data = (1 << (uint8_t)type);
    if(led_indicate_state & data){
        led_indicate_state &= (~data);
    }else{
        led_indicate_state |= data;
    }
    dsp_1628_show(0x0C, &led_indicate_state, 1);
}

/* ***************************应用************************************* *
*/
static void led_page_main_update(void);
//static void led_page_input_pw_update(void);
static void led_page_paraset_update(void);
static void led_page_set_val_update(void);

static void key_set_press_handle(void);
static void key_down_handle(void);
static void key_up_handle(void);
static void key_ent_press_handle(void);

static void key_handauto_press_handle(void);
static void key_fan1st_press_handle(void);
static void key_fan2st_press_handle(void);
static void key_fan3st_press_handle(void);

//static void save_paraset(void);
//static void recover_paraset(void);

static void __led_switch_to_page(uint8_t page){
    Meter_Surface.cur_page = page;
    Meter_Surface.timer = 0;
}

/**************************************************************************/
/*!
    @brief  显示初始化
    @returns
                None
*/
/**************************************************************************/
void led_dsp_init(void){
    dsp_1628_init();

#if 1
    //自检
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    for(int i=0; i<4; i++){
        led_onoff((LED_I_TYPE_T)(i*2+0), LED_I_ON);
        led_onoff((LED_I_TYPE_T)(i*2+1), LED_I_ON);
        led_show_char((LED_D_TUBE_POS_T)i, '8', 1);
    }
    wk_delay_ms(2000);
    wdt_counter_reload(); //喂狗
    
    for(int i=0; i<4; i++){
        led_onoff((LED_I_TYPE_T)(i*2+0), LED_I_OFF);
        led_onoff((LED_I_TYPE_T)(i*2+1), LED_I_OFF);
        led_show_char((LED_D_TUBE_POS_T)i, ' ', 0);
    }
    wk_delay_ms(1000);
    
    led_show_str(LED_D_TUBE_POS_L1, SoftVersion);
    wk_delay_ms(2000);
    wdt_counter_reload(); //喂狗
    

    led_dsp_mode_update();
#endif

    Meter_Surface.cur_page = DSP_PAGE_TEMP1;
    Meter_Surface.timer = 0;
}

/**************************************************************************/
/*!
    @brief  当前工作模式的显示更新
    @returns
                None
*/
/**************************************************************************/
void led_dsp_mode_update(void){
    /* 指示灯更新 */
    led_onoff(LED_I_POWER, LED_I_ON);
    
    /* 手动自动灯更新 */
    if(1 == Meter_run.HandAuto)
        led_onoff(LED_I_HANDOPT, LED_I_ON);
    else
        led_onoff(LED_I_HANDOPT, LED_I_OFF);
    
    /* 风机指示灯 */
    //风机1
    if(0 == Meter_run.FanStatus[0]){
        led_onoff(LED_I_FAN1RUN, LED_I_OFF);
        led_onoff(LED_I_FAN1ALARM, LED_I_OFF);
    }
    else if(1 == Meter_run.FanStatus[0]){
        led_onoff(LED_I_FAN1RUN, LED_I_ON);
        led_onoff(LED_I_FAN1ALARM, LED_I_OFF);
    }
    else if(2 == Meter_run.FanStatus[0]){
        if(Meter_run.FanCtl[0] == FAN_CTL_CLOSE){
          led_onoff(LED_I_FAN1RUN, LED_I_OFF);
          led_onoff(LED_I_FAN1ALARM, LED_I_ON);
        }
        else if(Meter_run.FanCtl[0] == FAN_CTL_OPEN){
          led_onoff(LED_I_FAN1RUN, LED_I_ON);
          led_onoff(LED_I_FAN1ALARM, LED_I_ON);
        }
    }
    
    //风机2
    if(0 == Meter_run.FanStatus[1]){
        led_onoff(LED_I_FAN2RUN, LED_I_OFF);
        led_onoff(LED_I_FAN2ALARM, LED_I_OFF);
    }
    else if(1 == Meter_run.FanStatus[1]){
        led_onoff(LED_I_FAN2RUN, LED_I_ON);
        led_onoff(LED_I_FAN2ALARM, LED_I_OFF);
    }
    else if(2 == Meter_run.FanStatus[1]){
        if(Meter_run.FanCtl[1] == FAN_CTL_CLOSE){
          led_onoff(LED_I_FAN2RUN, LED_I_OFF);
          led_onoff(LED_I_FAN2ALARM, LED_I_ON);
        }
        else if(Meter_run.FanCtl[1] == FAN_CTL_OPEN){
          led_onoff(LED_I_FAN2RUN, LED_I_ON);
          led_onoff(LED_I_FAN2ALARM, LED_I_ON);
        }
    }
    
    //风机3
    if(0 == Meter_run.FanStatus[2]){
        led_onoff(LED_I_FAN3RUN, LED_I_OFF);
        led_onoff(LED_I_FAN3ALARM, LED_I_OFF);
    }
    else if(1 == Meter_run.FanStatus[2]){
        led_onoff(LED_I_FAN3RUN, LED_I_ON);
        led_onoff(LED_I_FAN3ALARM, LED_I_OFF);
    }
    else if(2 == Meter_run.FanStatus[2]){
        if(Meter_run.FanCtl[2] == FAN_CTL_CLOSE){
          led_onoff(LED_I_FAN3RUN, LED_I_OFF);
          led_onoff(LED_I_FAN3ALARM, LED_I_ON);
        }
        else if(Meter_run.FanCtl[2] == FAN_CTL_OPEN){
          led_onoff(LED_I_FAN3RUN, LED_I_ON);
          led_onoff(LED_I_FAN3ALARM, LED_I_ON);
        }
    }
      
}

/**************************************************************************/
/*!
    @brief  显示定时器更新，放置于1s中断中
    @returns
                None
*/
/**************************************************************************/
void led_dsp_inc_timer(void){
    Meter_Surface.timer++;
}

/**************************************************************************/
/*!
    @brief  显示主更新
    @returns
                None
*/
/**************************************************************************/
#define __key_press_led_handle(sign, func)  \
    do{\
        if(Meter_Surface.Keyb & sign){\
            func;\
            Meter_Surface.Keyb &= ~sign;\
        }\
    }while(0)

void led_main_update(void){
    /**************** 主循环更新 ****************/
    led_dsp_mode_update();

    //清空显示
    led_show_str(LED_D_TUBE_POS_L1, "    ");

    if(Meter_Surface.cur_page <= DSP_PAGE_FUNCUR3){
        led_page_main_update();
    }
    else if(Meter_Surface.cur_page >= DSP_PAGE_PW && Meter_Surface.cur_page <= DSP_PAGE_P9_1){
        led_page_paraset_update();
    }
    else if(Meter_Surface.cur_page >= DSP_PAGE_PW_VAL){
        led_page_set_val_update();
    }

    /**************** 按键处理 ****************/
    //设置键
    __key_press_led_handle(KEY_PRESS_SIGN(KEY_SET_S_SHIFT), key_set_press_handle());
    //减/左移键
    __key_press_led_handle(KEY_PRESS_SIGN(KEY_DOWN_S_SHIFT), key_down_handle());
    //增键
    __key_press_led_handle(KEY_PRESS_SIGN(KEY_UP_S_SHIFT), key_up_handle());
    //确定键
    __key_press_led_handle(KEY_PRESS_SIGN(KEY_ENT_S_SHIFT), key_ent_press_handle());
    
    //手动/自动键
    __key_press_led_handle(KEY_PRESS_SIGN(KEY_HAND_AUTO_S_SHIFT), key_handauto_press_handle());
    //风机1启停键
    __key_press_led_handle(KEY_PRESS_SIGN(KEY_FAN1_ST_S_SHIFT), key_fan1st_press_handle());
    //风机2启停键
    __key_press_led_handle(KEY_PRESS_SIGN(KEY_FAN2_ST_S_SHIFT), key_fan2st_press_handle());
    //风机3启停键
    __key_press_led_handle(KEY_PRESS_SIGN(KEY_FAN3_ST_S_SHIFT), key_fan3st_press_handle());
    

    /**************** 返回主页处理 ****************/
    if(Meter_Surface.cur_page >= DSP_PAGE_PW && Meter_Surface.timer >= 30){        
        __led_switch_to_page(DSP_PAGE_TEMP1);       
    }
}

/**************************************************************************/
/*!
    @brief  对float数据转换成字符，因受限于数码管位数，需判断当前float的值，
            根据值的大小判定是否加小数点
    
    @params[in] pStr
                存储float数据字符串
    @params[in] fdata
                传入的float数据
    @returns
                None
*/
/**************************************************************************/
static void Convert_fdata_str(char * pStr, float fdata){
    if(fdata >= 100){
        sprintf(pStr, "%d", (int16_t)fdata);
    }
    else if((fdata >= 10) && (fdata < 100)){
        sprintf(pStr, "%.1f", fdata);   
    }
    else if((fdata >= 0) && (fdata < 10)){
        sprintf(pStr, " %.1f", fdata);   
    }
    else if((fdata > -10) && (fdata < 0)){
        sprintf(pStr, "%.1f", fdata);   
    }
    else if(fdata <= -10){
        sprintf(pStr, "-%d", (int16_t)(-1* fdata));
    }
}

/**************************************************************************/
/*!
    @brief  主页面更新
    @returns
                None
*/
/**************************************************************************/
static void led_page_main_update(void){
    char str[10] = {0};   
    char tempStr[10] = {0};
    uint8_t len = 0;

    switch(Meter_Surface.cur_page){
    case DSP_PAGE_TEMP1:
        //温度1
        Convert_fdata_str(tempStr, Meter_run.Tempert[0]);
            
        len = strlen(tempStr);
        memcpy(str+1, tempStr, len);
        str[0] = 'A';
        led_show_str(LED_D_TUBE_POS_L1, str);     
        break;
    case DSP_PAGE_HUMI1:
        //湿度1
        Convert_fdata_str(tempStr, Meter_run.Humidity[0]);
            
        len = strlen(tempStr);
        memcpy(str+1, tempStr, len);
        str[0] = 'b';
        led_show_str(LED_D_TUBE_POS_L1, str);     
        break;
    case DSP_PAGE_FUNCUR1:
        //风机电流1
        Convert_fdata_str(tempStr, Meter_run.FanCur[0]);
            
        len = strlen(tempStr);
        memcpy(str+1, tempStr, len);
        str[0] = 'C';
        led_show_str(LED_D_TUBE_POS_L1, str);     
        break;
    case DSP_PAGE_TEMP2:
        //温度2
        Convert_fdata_str(tempStr, Meter_run.Tempert[1]);
            
        len = strlen(tempStr);
        memcpy(str+1, tempStr, len);
        str[0] = 'd';
        led_show_str(LED_D_TUBE_POS_L1, str);     
        break;
    case DSP_PAGE_HUMI2:
        //湿度2
        Convert_fdata_str(tempStr, Meter_run.Humidity[1]);
            
        len = strlen(tempStr);
        memcpy(str+1, tempStr, len);
        str[0] = 'E';
        led_show_str(LED_D_TUBE_POS_L1, str);     
        break;
    case DSP_PAGE_FUNCUR2:
        //风机电流2
        Convert_fdata_str(tempStr, Meter_run.FanCur[1]);
            
        len = strlen(tempStr);
        memcpy(str+1, tempStr, len);
        str[0] = 'F';
        led_show_str(LED_D_TUBE_POS_L1, str);     
        break; 
    case DSP_PAGE_TEMP3:
        //温度3
        Convert_fdata_str(tempStr, Meter_run.Tempert[2]);
            
        len = strlen(tempStr);
        memcpy(str+1, tempStr, len);
        str[0] = 'G';
        led_show_str(LED_D_TUBE_POS_L1, str);     
        break;
    case DSP_PAGE_HUMI3:
        //湿度3
        Convert_fdata_str(tempStr, Meter_run.Humidity[2]);
            
        len = strlen(tempStr);
        memcpy(str+1, tempStr, len);
        str[0] = 'H';
        led_show_str(LED_D_TUBE_POS_L1, str);     
        break;
    case DSP_PAGE_FUNCUR3:
        //风机电流3
        Convert_fdata_str(tempStr, Meter_run.FanCur[2]);
            
        len = strlen(tempStr);
        memcpy(str+1, tempStr, len);
        str[0] = 'I';
        led_show_str(LED_D_TUBE_POS_L1, str);     
        break;      
      default:break;
    }

    if(Meter_Surface.timer >= 3){
        Meter_Surface.cur_page++;
        if(Meter_Surface.cur_page > DSP_PAGE_FUNCUR3){
            Meter_Surface.cur_page = DSP_PAGE_TEMP1;
        }
           
        Meter_Surface.timer = 0;
    }
}

/**************************************************************************/
/*!
    @brief  设置页面更新
    @returns
                None
*/
/**************************************************************************/
static void led_page_paraset_update(void){
    char str[10] = {0};
    uint8_t index;
    
    if(Meter_Surface.cur_page == DSP_PAGE_PW){
        led_show_str(LED_D_TUBE_POS_L1, "P00");
    }
    else if((Meter_Surface.cur_page >= DSP_PAGE_P1_1) && 
            (Meter_Surface.cur_page <= DSP_PAGE_P1_4)){
        index = Meter_Surface.cur_page - DSP_PAGE_P1_1 + 1;
        sprintf(str, "P1.%d", index);
        led_show_str(LED_D_TUBE_POS_L1, str);
    }
    else if((Meter_Surface.cur_page >= DSP_PAGE_P2_1) && 
            (Meter_Surface.cur_page <= DSP_PAGE_P2_2)){
        index = Meter_Surface.cur_page - DSP_PAGE_P2_1 + 1;
        sprintf(str, "P2.%d", index);
        led_show_str(LED_D_TUBE_POS_L1, str);
    }
    else if((Meter_Surface.cur_page >= DSP_PAGE_P3_1) && 
            (Meter_Surface.cur_page <= DSP_PAGE_P3_5)){
        index = Meter_Surface.cur_page - DSP_PAGE_P3_1 + 1;
        sprintf(str, "P3.%d", index);
        led_show_str(LED_D_TUBE_POS_L1, str);
    }
    else if((Meter_Surface.cur_page >= DSP_PAGE_P4_1) && 
            (Meter_Surface.cur_page <= DSP_PAGE_P4_7)){
        index = Meter_Surface.cur_page - DSP_PAGE_P4_1 + 1;
        sprintf(str, "P4.%d", index);
        led_show_str(LED_D_TUBE_POS_L1, str);
    }
    else if((Meter_Surface.cur_page >= DSP_PAGE_P5_1) && 
            (Meter_Surface.cur_page <= DSP_PAGE_P5_6)){
        index = Meter_Surface.cur_page - DSP_PAGE_P5_1 + 1;
        sprintf(str, "P5.%d", index);
        led_show_str(LED_D_TUBE_POS_L1, str);
    }
    else if((Meter_Surface.cur_page >= DSP_PAGE_P6_1) &&
            (Meter_Surface.cur_page <= DSP_PAGE_P6_6)){
        index = Meter_Surface.cur_page - DSP_PAGE_P6_1 + 1;
        sprintf(str, "P6.%d", index);
        led_show_str(LED_D_TUBE_POS_L1, str);
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P8_1) {
        index = Meter_Surface.cur_page - DSP_PAGE_P8_1 + 1;
        sprintf(str, "P8.%d", index);
        led_show_str(LED_D_TUBE_POS_L1, str);
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P9_1) {
        index = Meter_Surface.cur_page - DSP_PAGE_P9_1 + 1;
        sprintf(str, "P9.%d", index);
        led_show_str(LED_D_TUBE_POS_L1, str);
    }
}

/**************************************************************************/
/*!
    @brief  所设置的值页面更新
    @returns
                None
*/
/**************************************************************************/
static void led_page_set_val_update(void){
    char str[20] = {0};

    switch(Meter_Surface.cur_page){
    case DSP_PAGE_PW_VAL:
        sprintf(str, " %03d", Meter_Surface.inputPw);
        led_show_str_flick(LED_D_TUBE_POS_L1, str, Meter_Surface.x_post);
        break;
    case DSP_PAGE_P1_1_VAL:
    case DSP_PAGE_P1_2_VAL:
    case DSP_PAGE_P1_3_VAL:
    case DSP_PAGE_P1_4_VAL:
        sprintf(str, " %03d", Meter_Surface.u16_value);
        led_show_str_flick(LED_D_TUBE_POS_L1, str, Meter_Surface.x_post);
        break;
        
    case DSP_PAGE_P2_1_VAL:
    case DSP_PAGE_P2_2_VAL:
        sprintf(str, " %03d", Meter_Surface.u16_value);
        led_show_str_flick(LED_D_TUBE_POS_L1, str, Meter_Surface.x_post);
        break;
        
    case DSP_PAGE_P3_1_VAL:
    case DSP_PAGE_P3_2_VAL:
    case DSP_PAGE_P3_3_VAL:
    case DSP_PAGE_P3_4_VAL:
    case DSP_PAGE_P3_5_VAL:
        sprintf(str, " %03d", Meter_Surface.u16_value);
        led_show_str_flick(LED_D_TUBE_POS_L1, str, Meter_Surface.x_post);
        break;
        
    case DSP_PAGE_P4_1_VAL:
    case DSP_PAGE_P4_2_VAL:
    case DSP_PAGE_P4_3_VAL:
    case DSP_PAGE_P4_4_VAL:
    case DSP_PAGE_P4_5_VAL:
    case DSP_PAGE_P4_6_VAL:
    case DSP_PAGE_P4_7_VAL:
        sprintf(str, " %03d", Meter_Surface.u16_value);
        led_show_str_flick(LED_D_TUBE_POS_L1, str, Meter_Surface.x_post);
        break;
    
    case DSP_PAGE_P5_1_VAL:
    case DSP_PAGE_P5_3_VAL:
    case DSP_PAGE_P5_5_VAL:
        sprintf(str, " %02d.%01d", Meter_Surface.u16_value/10, Meter_Surface.u16_value%10);
        led_show_str_flick(LED_D_TUBE_POS_L1, str, Meter_Surface.x_post);
        break;
        
    case DSP_PAGE_P5_2_VAL:
    case DSP_PAGE_P5_4_VAL:
    case DSP_PAGE_P5_6_VAL:
        sprintf(str, " %03d", Meter_Surface.u16_value);
        led_show_str_flick(LED_D_TUBE_POS_L1, str, Meter_Surface.x_post);
        break;
    
    case DSP_PAGE_P6_1_VAL:
    case DSP_PAGE_P6_2_VAL:
    case DSP_PAGE_P6_3_VAL:
    case DSP_PAGE_P6_4_VAL:
    case DSP_PAGE_P6_5_VAL:
    case DSP_PAGE_P6_6_VAL:
        sprintf(str, " %03d", Meter_Surface.u16_value);
        led_show_str_flick(LED_D_TUBE_POS_L1, str, Meter_Surface.x_post);
        break;
    
    case DSP_PAGE_P8_1_VAL:
        sprintf(str, " %03d", Meter_Surface.u16_value);
        led_show_str_flick(LED_D_TUBE_POS_L1, str, Meter_Surface.x_post);
        break;
        
    case DSP_PAGE_P9_1_VAL:
        sprintf(str, " %03d", Meter_Surface.u16_value);
        led_show_str_flick(LED_D_TUBE_POS_L1, str, Meter_Surface.x_post);
        break;
    }
}

/**************************************************************************/
/*!
    @brief  设置键按下后的处理
    @returns
                None
*/
/**************************************************************************/
static void key_set_press_handle(void){

    //只按下设置键
    if(Meter_Surface.cur_page <= DSP_PAGE_FUNCUR3){
        __led_switch_to_page(DSP_PAGE_PW);
        Meter_Surface.inputPw = 0;
        Meter_Surface.x_post = 0;
    }
    else if((Meter_Surface.cur_page >= DSP_PAGE_PW) && 
            (Meter_Surface.cur_page <= DSP_PAGE_P9_1)){
        __led_switch_to_page(DSP_PAGE_TEMP1); 
        if((Meter_Surface.cur_page >= DSP_PAGE_P6_1) && 
           (Meter_Surface.cur_page <= DSP_PAGE_P6_6)){
              rtc_time_set(&Meter_Surface.set_rtc);
        }
    }
    else if((Meter_Surface.cur_page >= DSP_PAGE_PW_VAL) && 
            (Meter_Surface.cur_page <= DSP_PAGE_P9_1_VAL)){
            if(Meter_Surface.cur_page == DSP_PAGE_PW_VAL)
                __led_switch_to_page(DSP_PAGE_PW);
            else if((Meter_Surface.cur_page >= DSP_PAGE_P1_1_VAL) && 
                    (Meter_Surface.cur_page <= DSP_PAGE_P1_4_VAL))
                __led_switch_to_page(DSP_PAGE_P1_1);
            else if((Meter_Surface.cur_page >= DSP_PAGE_P2_1_VAL) && 
                    (Meter_Surface.cur_page <= DSP_PAGE_P2_2_VAL))
                __led_switch_to_page(DSP_PAGE_P2_1);
            else if((Meter_Surface.cur_page >= DSP_PAGE_P3_1_VAL) && 
                    (Meter_Surface.cur_page <= DSP_PAGE_P3_5_VAL))
                __led_switch_to_page(DSP_PAGE_P3_1);
            else if((Meter_Surface.cur_page >= DSP_PAGE_P4_1_VAL) && 
                    (Meter_Surface.cur_page <= DSP_PAGE_P4_7_VAL))
                __led_switch_to_page(DSP_PAGE_P4_1);
            else if((Meter_Surface.cur_page >= DSP_PAGE_P5_1_VAL) && 
                    (Meter_Surface.cur_page <= DSP_PAGE_P5_6_VAL))
                __led_switch_to_page(DSP_PAGE_P5_1);
            else if((Meter_Surface.cur_page >= DSP_PAGE_P6_1_VAL) && 
                    (Meter_Surface.cur_page <= DSP_PAGE_P6_6_VAL))              
                __led_switch_to_page(DSP_PAGE_P6_1); 
            else if(Meter_Surface.cur_page == DSP_PAGE_P8_1_VAL)              
                __led_switch_to_page(DSP_PAGE_P8_1); 
            else if(Meter_Surface.cur_page == DSP_PAGE_P9_1_VAL)              
                __led_switch_to_page(DSP_PAGE_P9_1); 
    }
}

/**************************************************************************/
/*!
    @brief  减键/左移键按下后的处理
    @returns
                None
*/
/**************************************************************************/
static void key_down_handle(void){    
    if( (Meter_Surface.cur_page >= DSP_PAGE_TEMP1) &&
        (Meter_Surface.cur_page <= DSP_PAGE_FUNCUR3) ){
        Meter_Surface.timer = 0;
        if(Meter_Surface.cur_page == DSP_PAGE_TEMP1){
            Meter_Surface.cur_page = DSP_PAGE_FUNCUR3;
        }
        else{
            Meter_Surface.cur_page--;
        }
    }    
     else if((Meter_Surface.cur_page >= DSP_PAGE_PW_VAL) &&
             (Meter_Surface.cur_page <= DSP_PAGE_P9_1_VAL)){
         Meter_Surface.timer = 0;
         Meter_Surface.x_post++;
         if(Meter_Surface.x_post > 2)
            Meter_Surface.x_post = 0;           
    }

}

//m^n函数
//返回值:m^n次方.
uint16_t maths_pow(uint8_t m,uint8_t n)
{
    uint16_t result=1;	 
    while(n--)result*=m;    
    return result;
}	

uint16_t maths_add(uint16_t data)
{
    uint16_t a,b,c,d=0;
    
    d = data;
    a = data % maths_pow(10, Meter_Surface.x_post+1);
    b = maths_pow(10, Meter_Surface.x_post);
    c = a / b;
    
    if(c < 9)
      d += b;
    
    return d;
}

uint16_t maths_sub(uint16_t data)
{
    uint16_t a,b,c,d=0;
    
    d = data;
    a = data % maths_pow(10, Meter_Surface.x_post+1);
    b = maths_pow(10, Meter_Surface.x_post);
    c = a / b;
    
    if(c >= 1)
      d -= b;
    
    return d;
}

uint16_t maths_data(uint16_t data)
{
    uint16_t a,b,c,d=0;
    
    d = data;
    a = data % maths_pow(10, Meter_Surface.x_post+1);
    b = maths_pow(10, Meter_Surface.x_post);
    c = a / b;
    
    if(c < 9)
      d += b;
    else if(c == 9)
      d -= (9*b);
    
    if(d > 999)
       d = 0;
    
    return d;
}
/**************************************************************************/
/*!
    @brief  增键按下后的处理
    @returns
                None
*/
/**************************************************************************/
static void key_up_handle(void){
    if(Meter_Surface.cur_page < DSP_PAGE_PW){
        Meter_Surface.timer = 0;
        Meter_Surface.cur_page++;
        if(Meter_Surface.cur_page >= DSP_PAGE_PW){
            Meter_Surface.cur_page = DSP_PAGE_TEMP1;
        }
    }   
    else if(Meter_Surface.cur_page == DSP_PAGE_PW_VAL){
        Meter_Surface.timer = 0;
        Meter_Surface.inputPw = maths_data(Meter_Surface.inputPw);       
    }
    //P1:温度参数
    else if((Meter_Surface.cur_page >= DSP_PAGE_P1_1) && 
            (Meter_Surface.cur_page <= DSP_PAGE_P1_4)){
        Meter_Surface.cur_page++;
        if(Meter_Surface.cur_page > DSP_PAGE_P1_4)
           Meter_Surface.cur_page = DSP_PAGE_P1_1;
    }   
    else if((Meter_Surface.cur_page >= DSP_PAGE_P1_1_VAL) ||
            (Meter_Surface.cur_page >= DSP_PAGE_P1_4_VAL)){
        Meter_Surface.timer = 0;
        Meter_Surface.u16_value = maths_data(Meter_Surface.u16_value);    
    }
    //P2：风机定时
    else if((Meter_Surface.cur_page >= DSP_PAGE_P2_1) && 
            (Meter_Surface.cur_page <= DSP_PAGE_P2_2)){
        Meter_Surface.cur_page++;
        if(Meter_Surface.cur_page > DSP_PAGE_P2_2)
           Meter_Surface.cur_page = DSP_PAGE_P2_1;
    }
    else if((Meter_Surface.cur_page >= DSP_PAGE_P2_1_VAL) ||
            (Meter_Surface.cur_page >= DSP_PAGE_P2_2_VAL)){
        Meter_Surface.timer = 0;
        Meter_Surface.u16_value = maths_data(Meter_Surface.u16_value);    
    }
    //P3:串口参数
    else if((Meter_Surface.cur_page >= DSP_PAGE_P3_1) && 
            (Meter_Surface.cur_page <= DSP_PAGE_P3_5)){
        Meter_Surface.cur_page++;
        if(Meter_Surface.cur_page > DSP_PAGE_P3_5)
           Meter_Surface.cur_page = DSP_PAGE_P3_1;
    }
    else if((Meter_Surface.cur_page >= DSP_PAGE_P3_1_VAL) ||
            (Meter_Surface.cur_page >= DSP_PAGE_P3_5_VAL)){
        Meter_Surface.timer = 0;
        Meter_Surface.u16_value = maths_data(Meter_Surface.u16_value);    
    }
    //P4:通讯地址
    else if((Meter_Surface.cur_page >= DSP_PAGE_P4_1) && 
            (Meter_Surface.cur_page <= DSP_PAGE_P4_7)){
        Meter_Surface.cur_page++;
        if(Meter_Surface.cur_page > DSP_PAGE_P4_7)
           Meter_Surface.cur_page = DSP_PAGE_P4_1;
    }
    else if((Meter_Surface.cur_page >= DSP_PAGE_P4_1_VAL) ||
            (Meter_Surface.cur_page >= DSP_PAGE_P4_7_VAL)){
        Meter_Surface.timer = 0;
        Meter_Surface.u16_value = maths_data(Meter_Surface.u16_value);    
    }
    //P5:风机负载设置
    else if((Meter_Surface.cur_page >= DSP_PAGE_P5_1) && 
            (Meter_Surface.cur_page <= DSP_PAGE_P5_6)){
        Meter_Surface.cur_page++;
        if(Meter_Surface.cur_page > DSP_PAGE_P5_6)
           Meter_Surface.cur_page = DSP_PAGE_P5_1;
    }
    else if((Meter_Surface.cur_page >= DSP_PAGE_P5_1_VAL) ||
            (Meter_Surface.cur_page >= DSP_PAGE_P5_6_VAL)){
        Meter_Surface.timer = 0;
        Meter_Surface.u16_value = maths_data(Meter_Surface.u16_value);    
    }
    //P6:设备地址
     else if((Meter_Surface.cur_page >= DSP_PAGE_P6_1) && 
            (Meter_Surface.cur_page <= DSP_PAGE_P6_6)){
        Meter_Surface.cur_page++;
        if(Meter_Surface.cur_page > DSP_PAGE_P6_6)
           Meter_Surface.cur_page = DSP_PAGE_P6_1;
    }
    else if((Meter_Surface.cur_page >= DSP_PAGE_P6_1_VAL) ||
            (Meter_Surface.cur_page >= DSP_PAGE_P6_6_VAL)){
        Meter_Surface.timer = 0;
        Meter_Surface.u16_value = maths_data(Meter_Surface.u16_value);    
    }
    //P9:温湿度协议选择
    else if(Meter_Surface.cur_page == DSP_PAGE_P9_1) {
        Meter_Surface.cur_page = DSP_PAGE_P9_1;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P9_1_VAL) {
        Meter_Surface.timer = 0;
        Meter_Surface.u16_value = maths_data(Meter_Surface.u16_value);    
    }
}



/**************************************************************************/
/*!
    @brief  ENT键按下后的处理
    @returns
                None
*/
/**************************************************************************/
static void key_ent_press_handle(void){
    if(Meter_Surface.cur_page == DSP_PAGE_PW){
        __led_switch_to_page(DSP_PAGE_PW_VAL);
        Meter_Surface.inputPw = 0;
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_Surface.inputPw;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_PW_VAL){
        switch(Meter_Surface.inputPw){
        case P1_PASSWORD:
            __led_switch_to_page(DSP_PAGE_P1_1);
            break;
        case P2_PASSWORD:
            __led_switch_to_page(DSP_PAGE_P2_1);
            break;
        case P3_PASSWORD:
            __led_switch_to_page(DSP_PAGE_P3_1);
            break;
        case P4_PASSWORD:
            __led_switch_to_page(DSP_PAGE_P4_1);
            break;
        case P5_PASSWORD:
            __led_switch_to_page(DSP_PAGE_P5_1);
            break;
        case P6_PASSWORD:
            __led_switch_to_page(DSP_PAGE_P6_1);
            Meter_Surface.set_rtc.year = calendar.year;
            Meter_Surface.set_rtc.month = calendar.month;
            Meter_Surface.set_rtc.date = calendar.date;
            Meter_Surface.set_rtc.hour = calendar.hour;
            Meter_Surface.set_rtc.min = calendar.min;
            Meter_Surface.set_rtc.sec = calendar.sec;
            break;
        case P8_PASSWORD:
            __led_switch_to_page(DSP_PAGE_P8_1);
            break;
        case P9_PASSWORD:
            __led_switch_to_page(DSP_PAGE_P9_1);
            break;
        default:
             __led_switch_to_page(DSP_PAGE_TEMP1);
            break;
        }
    }
    //P1:温度参数
    else if(Meter_Surface.cur_page == DSP_PAGE_P1_1){
        __led_switch_to_page(DSP_PAGE_P1_1_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.FanStopTemp;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P1_2){
        __led_switch_to_page(DSP_PAGE_P1_2_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.FanStartTemp;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P1_3){
        __led_switch_to_page(DSP_PAGE_P1_3_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.FanErrTemp;
    } 
    else if(Meter_Surface.cur_page == DSP_PAGE_P1_4){
        __led_switch_to_page(DSP_PAGE_P1_4_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.DehumiLevel;
    } 
    else if(Meter_Surface.cur_page == DSP_PAGE_P1_1_VAL){
        __led_switch_to_page(DSP_PAGE_P1_2);
        if((Meter_Surface.u16_value >= 15) && (Meter_Surface.u16_value <= 130) &&\
           (Meter_Surface.u16_value < Meter_run.FanStartTemp)){       
            Meter_run.FanStopTemp = Meter_Surface.u16_value;
            //存储设置值
            UserEpromWriteU16(EE_FAN_STOPTEMP, Meter_run.FanStopTemp);
        }
        else{
            Meter_run.FanStopTemp = UserEpromReadU16(EE_FAN_STOPTEMP);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P1_2_VAL){
        __led_switch_to_page(DSP_PAGE_P1_3);
        if((Meter_Surface.u16_value >= 15) && (Meter_Surface.u16_value <= 130) &&\
           (Meter_Surface.u16_value < Meter_run.FanErrTemp)){       
            Meter_run.FanStartTemp = Meter_Surface.u16_value;
            //存储设置值
            UserEpromWriteU16(EE_FAN_STARTTEMP, Meter_run.FanStartTemp);
        }
        else{
            Meter_run.FanStartTemp = UserEpromReadU16(EE_FAN_STARTTEMP);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P1_3_VAL){
        __led_switch_to_page(DSP_PAGE_P1_4);
        if((Meter_Surface.u16_value >= 15) && (Meter_Surface.u16_value <= 130) &&\
           (Meter_Surface.u16_value > Meter_run.FanStartTemp)){       
            Meter_run.FanErrTemp = Meter_Surface.u16_value;
            //存储设置值
            UserEpromWriteU16(EE_FAN_ERRTEMP, Meter_run.FanErrTemp);
        }
        else{
            Meter_run.FanErrTemp = UserEpromReadU16(EE_FAN_ERRTEMP);
        }    
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P1_4_VAL){
        __led_switch_to_page(DSP_PAGE_P1_1);
        if((Meter_Surface.u16_value >= 6) && (Meter_Surface.u16_value <= 100)){       
            Meter_run.DehumiLevel = Meter_Surface.u16_value;
            //存储设置值
            UserEpromWriteU16(EE_DEHUMI_LEVEL, Meter_run.DehumiLevel);
        }
        else{
            Meter_run.DehumiLevel = UserEpromReadU16(EE_DEHUMI_LEVEL);
        }    
    }
    //P2：风机定时
    else if(Meter_Surface.cur_page == DSP_PAGE_P2_1){
        __led_switch_to_page(DSP_PAGE_P2_1_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.FanTimeInter;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P2_2){
        __led_switch_to_page(DSP_PAGE_P2_2_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.FanTimeRun;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P2_1_VAL){
        __led_switch_to_page(DSP_PAGE_P2_2);
        if(Meter_Surface.u16_value <= 240){       
            Meter_run.FanTimeInter = Meter_Surface.u16_value;
            //存储设置值
            UserEpromWriteU16(EE_FAN_TIME_INTER, Meter_run.FanTimeInter);
        }
        else{
            Meter_run.FanTimeInter = UserEpromReadU16(EE_FAN_TIME_INTER);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P2_2_VAL){
        __led_switch_to_page(DSP_PAGE_P2_1);
        if((Meter_Surface.u16_value >= 1) && (Meter_Surface.u16_value <= 30)){       
            Meter_run.FanTimeRun = Meter_Surface.u16_value;
            //存储设置值
            UserEpromWriteU16(EE_FAN_TIME_RUN, Meter_run.FanTimeRun);
        }
        else{
            Meter_run.FanTimeRun = UserEpromReadU16(EE_FAN_TIME_RUN);
        }
    }
    
    //P3:串口参数
    else if(Meter_Surface.cur_page == DSP_PAGE_P3_1){
        __led_switch_to_page(DSP_PAGE_P3_1_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.Com1Baud;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P3_2){
        __led_switch_to_page(DSP_PAGE_P3_2_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.Com1Parity;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P3_3){
        __led_switch_to_page(DSP_PAGE_P3_3_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.Com1Databit;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P3_4){
        __led_switch_to_page(DSP_PAGE_P3_4_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.Com1Stopbit;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P3_5){
        __led_switch_to_page(DSP_PAGE_P3_5_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.DevAdd;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P3_1_VAL){
        __led_switch_to_page(DSP_PAGE_P3_2);
        if(Meter_Surface.u16_value <= 6){       
            Meter_run.Com1Baud = Meter_Surface.u16_value;
            UserEpromWriteU8(EE_COM_1_BAUD, Meter_run.Com1Baud);
        }
        else{
            Meter_run.Com1Baud = UserEpromReadU8(EE_COM_1_BAUD);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P3_2_VAL){
        __led_switch_to_page(DSP_PAGE_P3_3);
        if(Meter_Surface.u16_value <= 2){       
            Meter_run.Com1Parity = Meter_Surface.u16_value;
            UserEpromWriteU8(EE_COM_1_PARITY, Meter_run.Com1Parity);
        }
        else{
            Meter_run.Com1Parity = UserEpromReadU8(EE_COM_1_PARITY);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P3_3_VAL){
        __led_switch_to_page(DSP_PAGE_P3_4);
        if(Meter_Surface.u16_value <= 1){       
            Meter_run.Com1Databit = Meter_Surface.u16_value;
            UserEpromWriteU8(EE_COM_1_DATABIT, Meter_run.Com1Databit);
        }
        else{
            Meter_run.Com1Databit = UserEpromReadU8(EE_COM_1_DATABIT);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P3_4_VAL){
        __led_switch_to_page(DSP_PAGE_P3_5);
        if(Meter_Surface.u16_value <= 3){       
            Meter_run.Com1Stopbit = Meter_Surface.u16_value;
            UserEpromWriteU8(EE_COM_1_STOPBIT, Meter_run.Com1Stopbit);
        }
        else{
            Meter_run.Com1Stopbit = UserEpromReadU8(EE_COM_1_STOPBIT);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P3_5_VAL){
        __led_switch_to_page(DSP_PAGE_P3_1);
        if((Meter_Surface.u16_value >= 1) && (Meter_Surface.u16_value < 255)){       
            Meter_run.DevAdd = Meter_Surface.u16_value;
            UserEpromWriteU8(EE_COM_DEV_ADD, Meter_run.DevAdd);
        }
        else{
            Meter_run.DevAdd = UserEpromReadU8(EE_COM_DEV_ADD);
        }
    }
 
    //P4:通讯地址
    else if(Meter_Surface.cur_page == DSP_PAGE_P4_1){
        __led_switch_to_page(DSP_PAGE_P4_1_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.Com2Baud;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P4_2){
        __led_switch_to_page(DSP_PAGE_P4_2_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.Com2Parity;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P4_3){
        __led_switch_to_page(DSP_PAGE_P4_3_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.Com2Databit;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P4_4){
        __led_switch_to_page(DSP_PAGE_P4_4_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.Com2Stopbit;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P4_5){
        __led_switch_to_page(DSP_PAGE_P4_5_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.FanComAdd[0];
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P4_6){
        __led_switch_to_page(DSP_PAGE_P4_6_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.FanComAdd[1];
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P4_7){
        __led_switch_to_page(DSP_PAGE_P4_7_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.FanComAdd[2];
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P4_1_VAL){
        __led_switch_to_page(DSP_PAGE_P4_2);
        if(Meter_Surface.u16_value <= 6){       
            Meter_run.Com2Baud = Meter_Surface.u16_value;
            UserEpromWriteU8(EE_COM_2_BAUD, Meter_run.Com2Baud);
        }
        else{
            Meter_run.Com2Baud = UserEpromReadU8(EE_COM_2_BAUD);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P4_2_VAL){
        __led_switch_to_page(DSP_PAGE_P4_3);
        if(Meter_Surface.u16_value <= 2){       
            Meter_run.Com2Parity = Meter_Surface.u16_value;
            UserEpromWriteU8(EE_COM_2_PARITY, Meter_run.Com2Parity);
        }
        else{
            Meter_run.Com2Parity = UserEpromReadU8(EE_COM_2_PARITY);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P4_3_VAL){
        __led_switch_to_page(DSP_PAGE_P4_4);
        if(Meter_Surface.u16_value <= 1){       
            Meter_run.Com2Databit = Meter_Surface.u16_value;
            UserEpromWriteU8(EE_COM_2_DATABIT, Meter_run.Com2Databit);
        }
        else{
            Meter_run.Com2Databit = UserEpromReadU8(EE_COM_2_DATABIT);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P4_4_VAL){
        __led_switch_to_page(DSP_PAGE_P4_5);
        if(Meter_Surface.u16_value <= 3){       
            Meter_run.Com2Stopbit = Meter_Surface.u16_value;
            UserEpromWriteU8(EE_COM_2_STOPBIT, Meter_run.Com2Stopbit);
        }
        else{
            Meter_run.Com2Stopbit = UserEpromReadU8(EE_COM_2_STOPBIT);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P4_5_VAL){
        __led_switch_to_page(DSP_PAGE_P4_6);
        if((Meter_Surface.u16_value <= 255) && (Meter_Surface.u16_value !=  Meter_run.FanComAdd[1]) &&\
           (Meter_Surface.u16_value !=  Meter_run.FanComAdd[2]) && (Meter_Surface.u16_value != 0)){       
            Meter_run.FanComAdd[0] = Meter_Surface.u16_value;
            UserEpromWriteU8(EE_COM_FAN1_ADD, Meter_run.FanComAdd[0]);
        }
        else if(Meter_Surface.u16_value == 0){
            Meter_run.FanComAdd[0] = Meter_Surface.u16_value;
            UserEpromWriteU8(EE_COM_FAN1_ADD, Meter_run.FanComAdd[0]);
        }
        else{
            Meter_run.FanComAdd[0] = UserEpromReadU8(EE_COM_FAN1_ADD);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P4_6_VAL){
        __led_switch_to_page(DSP_PAGE_P4_7);
        if((Meter_Surface.u16_value <= 255) && (Meter_Surface.u16_value !=  Meter_run.FanComAdd[0]) &&\
           (Meter_Surface.u16_value !=  Meter_run.FanComAdd[2]) && (Meter_Surface.u16_value != 0)){       
            Meter_run.FanComAdd[1] = Meter_Surface.u16_value;
            UserEpromWriteU8(EE_COM_FAN2_ADD, Meter_run.FanComAdd[1]);
        }
        else if(Meter_Surface.u16_value == 0){
            Meter_run.FanComAdd[1] = Meter_Surface.u16_value;
            UserEpromWriteU8(EE_COM_FAN2_ADD, Meter_run.FanComAdd[1]);
        }
        else{
            Meter_run.FanComAdd[1] = UserEpromReadU8(EE_COM_FAN2_ADD);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P4_7_VAL){
        __led_switch_to_page(DSP_PAGE_P4_1);
        if((Meter_Surface.u16_value <= 255) && (Meter_Surface.u16_value !=  Meter_run.FanComAdd[0]) &&\
           (Meter_Surface.u16_value !=  Meter_run.FanComAdd[1]) && (Meter_Surface.u16_value != 0)){      
            Meter_run.FanComAdd[2] = Meter_Surface.u16_value;
            UserEpromWriteU8(EE_COM_FAN3_ADD, Meter_run.FanComAdd[2]);
        }
        else if(Meter_Surface.u16_value == 0){
            Meter_run.FanComAdd[2] = Meter_Surface.u16_value;
            UserEpromWriteU8(EE_COM_FAN3_ADD, Meter_run.FanComAdd[2]);
        }
        else{
            Meter_run.FanComAdd[2] = UserEpromReadU8(EE_COM_FAN3_ADD);
        }
    }
    //P5:风机负载设置
    else if(Meter_Surface.cur_page == DSP_PAGE_P5_1){
        __led_switch_to_page(DSP_PAGE_P5_1_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.FanSetCur[0];
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P5_2){
        __led_switch_to_page(DSP_PAGE_P5_2_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.FanEnable[0];
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P5_3){
        __led_switch_to_page(DSP_PAGE_P5_3_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.FanSetCur[1];
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P5_4){
        __led_switch_to_page(DSP_PAGE_P5_4_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.FanEnable[1];
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P5_5){
        __led_switch_to_page(DSP_PAGE_P5_5_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.FanSetCur[2];
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P5_6){
        __led_switch_to_page(DSP_PAGE_P5_6_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.FanEnable[2];
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P5_1_VAL){
        __led_switch_to_page(DSP_PAGE_P5_2);
        if((Meter_Surface.u16_value >= 1) && (Meter_Surface.u16_value <= 999)){       
            Meter_run.FanSetCur[0] = Meter_Surface.u16_value;
            UserEpromWriteU16(EE_FAN_1_CUR, Meter_run.FanSetCur[0]);
        }
        else{
            Meter_run.FanSetCur[0] = UserEpromReadU16(EE_FAN_1_CUR);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P5_2_VAL){
        __led_switch_to_page(DSP_PAGE_P5_3);
        if(Meter_Surface.u16_value <= 1){       
            Meter_run.FanEnable[0] = Meter_Surface.u16_value;
            UserEpromWriteU16(EE_FAN_1_ENABLE, Meter_run.FanEnable[0]);
        }
        else{
            Meter_run.FanEnable[0] = UserEpromReadU16(EE_FAN_1_ENABLE);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P5_3_VAL){
        __led_switch_to_page(DSP_PAGE_P5_4);
        if((Meter_Surface.u16_value >= 1) && (Meter_Surface.u16_value <= 999)){       
            Meter_run.FanSetCur[1] = Meter_Surface.u16_value;
            UserEpromWriteU16(EE_FAN_2_CUR, Meter_run.FanSetCur[1]);
        }
        else{
            Meter_run.FanSetCur[1] = UserEpromReadU16(EE_FAN_2_CUR);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P5_4_VAL){
        __led_switch_to_page(DSP_PAGE_P5_5);
        if(Meter_Surface.u16_value <= 1){       
            Meter_run.FanEnable[1] = Meter_Surface.u16_value;
            UserEpromWriteU16(EE_FAN_2_ENABLE, Meter_run.FanEnable[1]);
        }
        else{
            Meter_run.FanEnable[1] = UserEpromReadU16(EE_FAN_2_ENABLE);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P5_5_VAL){
        __led_switch_to_page(DSP_PAGE_P5_6);
        if((Meter_Surface.u16_value >= 1) && (Meter_Surface.u16_value <= 999)){       
            Meter_run.FanSetCur[2] = Meter_Surface.u16_value;
            UserEpromWriteU16(EE_FAN_3_CUR, Meter_run.FanSetCur[2]);
        }
        else{
            Meter_run.FanSetCur[2] = UserEpromReadU16(EE_FAN_3_CUR);
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P5_6_VAL){
        __led_switch_to_page(DSP_PAGE_P5_1);
        if(Meter_Surface.u16_value <= 1){       
            Meter_run.FanEnable[2] = Meter_Surface.u16_value;
            UserEpromWriteU16(EE_FAN_3_ENABLE, Meter_run.FanEnable[2]);
        }
        else{
            Meter_run.FanEnable[2] = UserEpromReadU16(EE_FAN_3_ENABLE);
        }
    }
    //P6:时钟设置
    else if(Meter_Surface.cur_page == DSP_PAGE_P6_1){
        __led_switch_to_page(DSP_PAGE_P6_1_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = calendar.year%100;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P6_2){
        __led_switch_to_page(DSP_PAGE_P6_2_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = calendar.month;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P6_3){
        __led_switch_to_page(DSP_PAGE_P6_3_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = calendar.date;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P6_4){
        __led_switch_to_page(DSP_PAGE_P6_4_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = calendar.hour;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P6_5){
        __led_switch_to_page(DSP_PAGE_P6_5_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = calendar.min;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P6_6){
        __led_switch_to_page(DSP_PAGE_P6_6_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = calendar.sec;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P6_1_VAL){
        __led_switch_to_page(DSP_PAGE_P6_2);
        if((Meter_Surface.u16_value >= 26) && (Meter_Surface.u16_value <= 99)){       
            Meter_Surface.set_rtc.year = Meter_Surface.u16_value + 2000;
        }
        else{
            Meter_Surface.set_rtc.year = calendar.year;
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P6_2_VAL){
        __led_switch_to_page(DSP_PAGE_P6_3);
        if((Meter_Surface.u16_value >= 1) && (Meter_Surface.u16_value <= 12)){       
            Meter_Surface.set_rtc.month = Meter_Surface.u16_value;
        }
        else{
            Meter_Surface.set_rtc.month = calendar.month;
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P6_3_VAL){
        __led_switch_to_page(DSP_PAGE_P6_4);
        if((Meter_Surface.u16_value >= 1) && (Meter_Surface.u16_value <= 31)){       
            Meter_Surface.set_rtc.date = Meter_Surface.u16_value;
        }
        else{
            Meter_Surface.set_rtc.date = calendar.date;
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P6_4_VAL){
        __led_switch_to_page(DSP_PAGE_P6_5);
        if((Meter_Surface.u16_value >= 1) && (Meter_Surface.u16_value <= 24)){       
            Meter_Surface.set_rtc.hour = Meter_Surface.u16_value;
        }
        else{
            Meter_Surface.set_rtc.hour = calendar.hour;
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P6_5_VAL){
        __led_switch_to_page(DSP_PAGE_P6_6);
        if((Meter_Surface.u16_value >= 1) && (Meter_Surface.u16_value <= 60)){       
            Meter_Surface.set_rtc.min = Meter_Surface.u16_value;
        }
        else{
            Meter_Surface.set_rtc.min = calendar.min;
        }
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P6_6_VAL){
        __led_switch_to_page(DSP_PAGE_P6_1);
        if((Meter_Surface.u16_value >= 1) && (Meter_Surface.u16_value <= 60)){       
            Meter_Surface.set_rtc.sec = Meter_Surface.u16_value;  
            rtc_time_set(&Meter_Surface.set_rtc);
        }
        else{
            Meter_Surface.set_rtc.sec = calendar.sec;
        }
    }
    //P8：设备重启
    else if(Meter_Surface.cur_page == DSP_PAGE_P8_1){
        __led_switch_to_page(DSP_PAGE_P8_1_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = 0;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P8_1_VAL){
        __led_switch_to_page(DSP_PAGE_P8_1);
        if(Meter_Surface.u16_value <= 1){       
          if(Meter_Surface.u16_value == 1){
              Meter_run.reBootFlag = 1;
          }          
        }
        else{
            Meter_run.reBootFlag = 0;
        }
    }
    //P9：温湿度协议选择
    else if(Meter_Surface.cur_page == DSP_PAGE_P9_1){
        __led_switch_to_page(DSP_PAGE_P9_1_VAL);
        Meter_Surface.x_post = 0;
        Meter_Surface.u16_value = Meter_run.TaHSensorSele;
    }
    else if(Meter_Surface.cur_page == DSP_PAGE_P9_1_VAL){
        __led_switch_to_page(DSP_PAGE_P9_1);
        if(Meter_Surface.u16_value <= 2){       
            Meter_run.TaHSensorSele = Meter_Surface.u16_value;
            //存储设置值
            UserEpromWriteU16(EE_TAH_SENSOR_SELE, Meter_run.TaHSensorSele);
        }
        else{
            Meter_run.TaHSensorSele = UserEpromReadU16(EE_TAH_SENSOR_SELE);
        }
    }
    
}

/**************************************************************************/
/*!
    @brief  手动/自动键按下后的处理
    @returns
                None
*/
/**************************************************************************/
static void key_handauto_press_handle(void){
    if(Meter_Surface.cur_page <= DSP_PAGE_FUNCUR3){
        Meter_run.HandAuto = !Meter_run.HandAuto;
    }
    
}

/**************************************************************************/
/*!
    @brief  风机1开启/关闭键按下后的处理
    @returns
                None
*/
/**************************************************************************/
static void key_fan1st_press_handle(void){
    if(1 == Meter_run.HandAuto){
        Meter_run.FanCtl[0] = (FAN_CTL)(!Meter_run.FanCtl[0]);
        Relay_Ctl(0,Meter_run.FanCtl[0]);
    }
}

/**************************************************************************/
/*!
    @brief  风机2开启/关闭键按下后的处理
    @returns
                None
*/
/**************************************************************************/
static void key_fan2st_press_handle(void){
    if(1 == Meter_run.HandAuto){
        Meter_run.FanCtl[1] = (FAN_CTL)(!Meter_run.FanCtl[1]);
        Relay_Ctl(1,Meter_run.FanCtl[1]);
    }
}

/**************************************************************************/
/*!
    @brief  风机3开启/关闭键按下后的处理
    @returns
                None
*/
/**************************************************************************/
static void key_fan3st_press_handle(void){
    if(1 == Meter_run.HandAuto){
        Meter_run.FanCtl[2] = (FAN_CTL)(!Meter_run.FanCtl[2]);
        Relay_Ctl(2,Meter_run.FanCtl[2]);
    }  
}

/**************************************************************************/
/*!
    @brief  保存修改的设置
    @returns
                None
*/
/**************************************************************************/
//static void save_paraset(void){
//
//}

/**************************************************************************/
/*!
    @brief  超时恢复修改的设置
    @returns
                None
*/
/**************************************************************************/
//static void recover_paraset(void){
//
//}
