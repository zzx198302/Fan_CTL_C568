#ifndef USERVARIABLE_H
#define USERVARIABLE_H

#include "at32f403a_407_wk_config.h"

/***************************版本信息**********************************/
#define SoftVersion         "L1.00"   //20260515

/***************************铁电存储**********************************/
#define EE_IS_MEMORY            0       //写0x5555，表示已存储

#define EE_FAN_PARA_START       16      //风机启停温度、风机电流
#define EE_FAN_STOPTEMP         EE_FAN_PARA_START
#define EE_FAN_STARTTEMP        EE_FAN_PARA_START+2
#define EE_FAN_ERRTEMP          EE_FAN_PARA_START+4
#define EE_FAN_1_CUR            EE_FAN_PARA_START+6
#define EE_FAN_1_ENABLE         EE_FAN_PARA_START+8
#define EE_FAN_2_CUR            EE_FAN_PARA_START+10
#define EE_FAN_2_ENABLE         EE_FAN_PARA_START+12
#define EE_FAN_3_CUR            EE_FAN_PARA_START+14
#define EE_FAN_3_ENABLE         EE_FAN_PARA_START+16

#define EE_COM_PRAR_START       36      //串口参数
#define EE_COM_1_BAUD           EE_COM_PRAR_START
#define EE_COM_1_PARITY         EE_COM_PRAR_START+1
#define EE_COM_1_DATABIT        EE_COM_PRAR_START+2
#define EE_COM_1_STOPBIT        EE_COM_PRAR_START+3        
#define EE_COM_2_BAUD           EE_COM_PRAR_START+4
#define EE_COM_2_PARITY         EE_COM_PRAR_START+5
#define EE_COM_2_DATABIT        EE_COM_PRAR_START+6
#define EE_COM_2_STOPBIT        EE_COM_PRAR_START+7

#define EE_COM_ADD_START        46      //通信地址
#define EE_COM_FAN1_ADD         EE_COM_ADD_START
#define EE_COM_FAN2_ADD         EE_COM_ADD_START+1
#define EE_COM_FAN3_ADD         EE_COM_ADD_START+2
#define EE_COM_DEV_ADD          EE_COM_ADD_START+3 //设备地址

#define EE_FAN_CHECK_START      50      //风机电流校准
#define EE_FAN_1_CHECK_BASE     EE_FAN_CHECK_START
#define EE_FAN_2_CHECK_BASE     EE_FAN_CHECK_START+2
#define EE_FAN_3_CHECK_BASE     EE_FAN_CHECK_START+4
#define EE_FAN_1_CHECK_FULL     EE_FAN_CHECK_START+6
#define EE_FAN_2_CHECK_FULL     EE_FAN_CHECK_START+8
#define EE_FAN_3_CHECK_FULL     EE_FAN_CHECK_START+10

#define EE_FAN_TIME_START       66      //风机定时
#define EE_FAN_TIME_INTER       EE_FAN_TIME_START
#define EE_FAN_TIME_RUN         EE_FAN_TIME_START+2
#define EE_TAH_SENSOR_SELE      EE_FAN_TIME_START+4  //温湿度厂家选择
#define EE_DEHUMI_LEVEL         EE_FAN_TIME_START+6  //除湿器开启湿度

#define EE_RELAY_STATU_START    100    //继电器状态
#define EE_RELAY_1_STATU        EE_RELAY_STATU_START 
#define EE_RELAY_2_STATU        EE_RELAY_STATU_START+2
#define EE_RELAY_3_STATU        EE_RELAY_STATU_START+4
#define EE_RELAY_1_C_STATU      EE_RELAY_STATU_START+6
#define EE_RELAY_2_C_STATU      EE_RELAY_STATU_START+8
#define EE_RELAY_3_C_STATU      EE_RELAY_STATU_START+10
#define EE_TIME_RUN_1_FLAG      EE_RELAY_STATU_START+12
#define EE_TIME_RUN_2_FLAG      EE_RELAY_STATU_START+14
#define EE_TIME_RUN_3_FLAG      EE_RELAY_STATU_START+16

#define EE_EVENT_LOG_START      200
#define EE_EVENT_LOG_TOTALNUM   EE_EVENT_LOG_START   //故障记录总条数
#define EE_EVENT_LOG_POS        EE_EVENT_LOG_START+2
#define EE_EVENT_LOG_RECORD     210                  //210+10*100=1210

/***************************宏定义***********************************/
typedef enum _key_sign_shift{
    KEY_SET_S_SHIFT=0,
    KEY_DOWN_S_SHIFT,
    KEY_UP_S_SHIFT,
    KEY_ENT_S_SHIFT,
    KEY_HAND_AUTO_S_SHIFT,
    KEY_FAN1_ST_S_SHIFT,
    KEY_FAN2_ST_S_SHIFT,
    KEY_FAN3_ST_S_SHIFT,   
}KEY_SIGN_SHIFT_T;

#define KEY_PRESS_SIGN(shift)   ((uint16_t)(1<<((uint8_t)(shift))))

/***************************运行数据***********************************/
typedef enum _fan_status{
    FAN_STATUS_STOP=0,
    FAN_STATUS_OPEN,
    FAN_STATUS_FAULT,
}FAN_STATUS_T;

typedef enum _fan_ctl{
    FAN_CTL_CLOSE=0,
    FAN_CTL_OPEN,
}FAN_CTL;

typedef union {
    uint16_t uAllErr;
    struct{
      uint8_t Fan1Err:1;        //风机1故障
      uint8_t Fan2Err:1;        //风机2故障
      uint8_t Fan3Err:1;        //风机3故障
      uint8_t Temp1Alar:1;      //温度1报警
      uint8_t Temp2Alar:1;      //温度2报警
      uint8_t Temp3Alar:1;      //温度3报警
    }ErrBit;
}ErrCode;

typedef struct  Meter_run_value_tab {
    float Tempert[3];        //温度
    float Humidity[3];       //湿度
    float FanCur[3];         //风机电流
    float InterVol;          //内部电流
    float FanCurTemp[3];    
    uint16_t TempAlarm[3];   //温度报警状态（0：正常 1：报警）
    ErrCode errCode;         //故障 
    uint16_t U6ComCount[3];  //通讯次数
    uint32_t TimeOpenCount[3];//定时开启计时
    uint16_t TimeOpenFlag[3]; //定时标志
    uint16_t TimeRunCount[3]; //定时运行时间
    uint8_t  reBootFlag;      //重启
    
    uint16_t HandAuto;       //自动、手动 （0:自动 1:手动）
    uint16_t FanStatus[3];   //风机状态   （0:关闭 1:开启 2:故障）    
    FAN_CTL  FanCtl[3];      //风机控制   （0:关闭 1:开启）
     
    //存储参数
    uint16_t FanStopTemp;    //风机关闭温度
    uint16_t FanStartTemp;   //风机开启温度
    uint16_t FanErrTemp;     //风机故障温度
    uint16_t DehumiLevel;    //除湿器开启湿度
    uint16_t FanSetCur[3];   //风机设置电流
    uint16_t FanEnable[3];   //风机使能    
    uint16_t FanTimeInter;   //启动间隔时间
    uint16_t FanTimeRun;     //定时运行时间
    uint16_t TaHSensorSele;  //温湿度厂家选择（0:山东仁科 1：珠海汉升）

    uint8_t  FanComAdd[3];   //风机通信地址
    uint8_t  DevAdd;         //设备地址
    uint8_t  Com1Baud;       //串口1波特率
    uint8_t  Com1Parity;     //串口1校验位
    uint8_t  Com1Databit;    //串口1数据位
    uint8_t  Com1Stopbit;    //串口1停止位
    uint8_t  Com2Baud;       //串口2波特率
    uint8_t  Com2Parity;     //串口2校验位
    uint8_t  Com2Databit;    //串口2数据位
    uint8_t  Com2Stopbit;    //串口2停止位
    
}Meter_run_value_type;

/***************************显示、按键***********************************/
#define P1_PASSWORD     1       //P1密码
#define P2_PASSWORD     2       //P2密码
#define P3_PASSWORD     3       //P3密码
#define P4_PASSWORD     4       //P4密码
#define P5_PASSWORD     5       //P5密码
#define P6_PASSWORD     6       //P6密码
#define P8_PASSWORD     8       //P8密码
#define P9_PASSWORD     9       //P9密码  //温湿度协议切换
//页面
typedef enum _dsp_page_type{
    DSP_PAGE_TEMP1=0,   //主页面1：变压器温度 (A)
    DSP_PAGE_HUMI1,     //主页面2：变压器湿度 (b)
    DSP_PAGE_FUNCUR1,   //主页面3：变压器风机电流 (C)
    DSP_PAGE_TEMP2,     //主页面4：低压室温度 (d)
    DSP_PAGE_HUMI2,     //主页面5：低压室湿度 (E)
    DSP_PAGE_FUNCUR2,   //主页面3：低压室风机电流 (F)
    DSP_PAGE_TEMP3,     //主页面4：高压室温度 (G)
    DSP_PAGE_HUMI3,     //主页面5：高压室湿度 (H)
    DSP_PAGE_FUNCUR3,   //主页面3：高压室风机电流 (I)
    DSP_PAGE_PW,        //密码输入  9
    DSP_PAGE_P1_1,      //参数设置1.1 10
    DSP_PAGE_P1_2,      //参数设置1.2
    DSP_PAGE_P1_3,      //参数设置1.3
    DSP_PAGE_P1_4,      //参数设置1.4
    DSP_PAGE_P2_1,      //参数设置2.1
    DSP_PAGE_P2_2,      //参数设置2.2
    DSP_PAGE_P3_1,      //参数设置3.1 15
    DSP_PAGE_P3_2,      //参数设置3.2
    DSP_PAGE_P3_3,      //参数设置3.3
    DSP_PAGE_P3_4,      //参数设置3.4
    DSP_PAGE_P3_5,      //参数设置3.5
    DSP_PAGE_P4_1,      //参数设置4.1 20
    DSP_PAGE_P4_2,      //参数设置4.2
    DSP_PAGE_P4_3,      //参数设置4.3 
    DSP_PAGE_P4_4,      //参数设置4.4
    DSP_PAGE_P4_5,      //参数设置4.5
    DSP_PAGE_P4_6,      //参数设置4.6 25
    DSP_PAGE_P4_7,      //参数设置4.7
    DSP_PAGE_P5_1,      //参数设置5.1
    DSP_PAGE_P5_2,      //参数设置5.2
    DSP_PAGE_P5_3,      //参数设置5.3
    DSP_PAGE_P5_4,      //参数设置5.4 30
    DSP_PAGE_P5_5,      //参数设置5.5 
    DSP_PAGE_P5_6,      //参数设置5.6
    DSP_PAGE_P6_1,      //参数设置6.1 
    DSP_PAGE_P6_2,      //参数设置6.2 
    DSP_PAGE_P6_3,      //参数设置6.3 35
    DSP_PAGE_P6_4,      //参数设置6.4 
    DSP_PAGE_P6_5,      //参数设置6.5 
    DSP_PAGE_P6_6,      //参数设置6.6 
    DSP_PAGE_P8_1,      //参数设置8.1
    DSP_PAGE_P9_1,      //参数设置9.1 
    DSP_PAGE_PW_VAL,    //密码输入数值：
    DSP_PAGE_P1_1_VAL,  //参数设置1.1数值：风机关闭温度（15~130 默认40）40
    DSP_PAGE_P1_2_VAL,  //参数设置1.2数值：风机开启温度（15~130 默认45）
    DSP_PAGE_P1_3_VAL,  //参数设置1.3数值：风机报警温度（15~130 默认65）
    DSP_PAGE_P1_4_VAL,  //参数设置1.4数值：除湿机启动湿度（1~100 默认70）
    DSP_PAGE_P2_1_VAL,  //参数设置2.1数值：风机定时启动间隔（0~240h 0：不定时，默认12）
    DSP_PAGE_P2_2_VAL,  //参数设置2.2数值：风机定时运行时间（1~30min  默认3）
    DSP_PAGE_P3_1_VAL,  //参数设置3.1数值：串口1波特率（0-2400 1-4800 2-9600 3-19200 4-38400 5-57600 6-115200 默认2） 45
    DSP_PAGE_P3_2_VAL,  //参数设置3.2数值：串口1校验位（0-无 1-偶 2-奇 默认0）
    DSP_PAGE_P3_3_VAL,  //参数设置3.3数值：串口1数据位（0-8 1-9 默认0）
    DSP_PAGE_P3_4_VAL,  //参数设置3.4数值：串口1停止位（0-1 1-0.5 2-2 3-1.5 默认0）
    DSP_PAGE_P3_5_VAL,  //参数设置3.5数值：设备通信地址    
    DSP_PAGE_P4_1_VAL,  //参数设置4.1数值：串口2波特率（0-2400 1-4800 2-9600 3-19200 4-38400 5-57600 6-115200 默认2） 50
    DSP_PAGE_P4_2_VAL,  //参数设置4.2数值：串口2校验位（0-无 1-偶 2-奇 默认0）
    DSP_PAGE_P4_3_VAL,  //参数设置4.3数值：串口2数据位（0-8 1-9 默认0） 
    DSP_PAGE_P4_4_VAL,  //参数设置4.4数值：串口2停止位（0-1 1-0.5 2-2 3-1.5 默认0）
    DSP_PAGE_P4_5_VAL,  //参数设置4.5数值：低压室温湿度传感器地址（0-255 默认0）
    DSP_PAGE_P4_6_VAL,  //参数设置4.6数值：高压室温湿度传感器地址（0-255 默认0）55
    DSP_PAGE_P4_7_VAL,  //参数设置4.7数值：高压室温湿度传感器地址（0-255 默认0）
    DSP_PAGE_P5_1_VAL,  //参数设置5.1数值：变压器室风机电流（0.1-99.9A 默认10.0）
    DSP_PAGE_P5_2_VAL,  //参数设置5.2数值：变压器室风机故障使能（0-关闭 1-使能 默认0）
    DSP_PAGE_P5_3_VAL,  //参数设置5.3数值：低压室风机电流（0.1-99.9A 默认10.0）
    DSP_PAGE_P5_4_VAL,  //参数设置5.4数值：低压室风机故障使能（0-关闭 1-使能 默认0） 60
    DSP_PAGE_P5_5_VAL,  //参数设置5.5数值：高压室风机电流（0.1-99.9A 默认10.0）
    DSP_PAGE_P5_6_VAL,  //参数设置5.6数值：高压室风机故障使能（0-关闭 1-使能 默认0）
    DSP_PAGE_P6_1_VAL,  //参数设置6.1数值: 年
    DSP_PAGE_P6_2_VAL,  //参数设置6.2数值: 月
    DSP_PAGE_P6_3_VAL,  //参数设置6.3数值: 日  65
    DSP_PAGE_P6_4_VAL,  //参数设置6.4数值: 时
    DSP_PAGE_P6_5_VAL,  //参数设置6.5数值: 分
    DSP_PAGE_P6_6_VAL,  //参数设置6.6数值: 秒
    DSP_PAGE_P8_1_VAL,  //参数设置9.1数值
    DSP_PAGE_P9_1_VAL,  //参数设置9.1数值 
}DSP_PAGE_T;

typedef struct  Meter_Surface_tab {
    uint16_t Keyb;          //获取的按键值
    uint32_t timer;         //定时器
    int8_t cur_page;        //当前页面
    uint16_t u16_value;     //U16类型的中间量
    int16_t inputPw;        //输入的密码
    char x_post;            //设置位移位
    calendar_type set_rtc;  //设置的时钟
    uint8_t set_rtc_enbit;  //设置的时钟使能位：bit0~5 <-> 年月日时分秒
    uint16_t max_record_len;//最长记录长度，故障事件显示时用
} Meter_Surface_type;


/***************************ADC***********************************/
#define ADC_POINT               128

typedef struct  Meter_ADC_Check_tab      //设备校验值
{
    uint16_t Adc1_Value[4*ADC_POINT];
    uint16_t Fan1_Cur_Adc[ADC_POINT];
    uint16_t Fan2_Cur_Adc[ADC_POINT];
    uint16_t Fan3_Cur_Adc[ADC_POINT];
    uint16_t Inter_Vol_Adc[ADC_POINT];
    
    uint16_t Fan_Check_step;            //校准步骤
    uint16_t Inter_Vol_Adc_Avg;         //一次平均值
    uint16_t Fan1_Cur_Adc_ResVal[10];   //一次平均值
    uint16_t Fan2_Cur_Adc_ResVal[10];
    uint16_t Fan3_Cur_Adc_ResVal[10];
    uint16_t Fan_Adc_ResTime;           //一次平均值计数
    uint16_t Fan1_Cur_Adc_Avg;          //二次平均值
    uint16_t Fan2_Cur_Adc_Avg;          //二次平均值
    uint16_t Fan3_Cur_Adc_Avg;          //二次平均值
    
    uint16_t Fan_Check_Time;            //校准平均值计数
    uint16_t Fan1_Cur_Adc_base_Check;   //0值
    uint16_t Fan2_Cur_Adc_base_Check;   //0值
    uint16_t Fan3_Cur_Adc_base_Check;   //0值
    uint16_t Fan1_Cur_Adc_full_Check;   //满值
    uint16_t Fan2_Cur_Adc_full_Check;   //满值
    uint16_t Fan3_Cur_Adc_full_Check;   //满值
}Meter_ADC_Check_type;

/***************************串口***********************************/
#define USART_ReceiveData_LONG      250
//字节处理
#define HIGH_BYTE(VALUE)            (((VALUE)>>8)&(0xFF))
#define LOW_BYTE(VALUE)             ((VALUE)&(0xFF))

enum RS485_ERROR_TYPE{
    RS485_FUN_ERR,
    RS485_ADDR_ERR,
    RS485_DATA_ERR,
    RS485_OTHER_ERR
};

typedef struct  Meter_usart_value_tab      //设备通信
{
    uint8_t usart_record[500];    //数据缓存

    uint8_t Usart1_Receive[USART_ReceiveData_LONG]; //接收数据缓存
    uint8_t Usart1_Transmit[210];  //发送数据缓存
    uint16_t Usart1_addr;          //地址
    uint32_t Usart1_TIMER;         //中断计数器
    uint8_t Usart1_num;            //接收个数
    uint8_t Usart1_s_num;          //发送个数
    uint16_t Usart1_wait35;        //3.5字节时间
    uint32_t Usart1_BaudRate;      //波特率
    uint8_t  Usart1_Parity;        //校验位
    uint8_t  Usart1_Databit;       //数据位
    uint8_t  Usart1_Stopbit;       //停止位

    uint8_t Usart6_Receive[USART_ReceiveData_LONG]; //接收数据缓存
    uint8_t Usart6_Transmit[210];  //发送数据缓存
    uint16_t Usart6_addr;          //地址
    uint32_t Usart6_TIMER;         //中断计数器
    uint8_t Usart6_num;            //接收个数
    uint8_t Usart6_s_num;          //发送个数
    uint16_t Usart6_wait35;        //3.5字节时间
    uint32_t Usart6_BaudRate;      //波特率
    uint8_t  Usart6_Parity;        //校验位
    uint8_t  Usart6_Databit;       //数据位
    uint8_t  Usart6_Stopbit;       //停止位
    uint16_t Usart6_SendCnt;       //发送计时
    uint8_t  Usart6_RorSFlag;      //发送标志：0-可发送（接收完） 1-发送中
    uint8_t  Usart6_SendNum;       //发送几个设备
    
}Meter_usart_value_type;

/***************************事件记录***********************************/
typedef struct  EVENT_LOG_tab
{
    uint16_t Event_Record_E[5];        //(故障信息)3个时间、一个故障事项、一个数据(5*2*100 =1000)
    uint16_t E_Total_Num;              //保存数据总条数
    uint16_t E_Present_Save_ad;        //当前存储地址 0-99 100条
    uint16_t E_Present_Save_ad_bak;
}EVENT_LOG_type;

/*可读运行参数*/
#pragma pack (2)

#pragma pack ()


/*变量申明*/
extern Meter_run_value_type Meter_run;
extern Meter_Surface_type Meter_Surface;
extern Meter_ADC_Check_type Meter_ADC;
extern Meter_usart_value_type Meter_usart;
extern EVENT_LOG_type EVENT_LOG;
/*函数申明*/
void variableReadParaInit(void);
void variableWriteParaInit(void);
void ComPara(void);
void EventLogReadInit(void);
void EventLogClear(void);
void WrRelayStatusLog(uint8_t num, FAN_CTL fanctl);
void WrRelayCtlStatusLog(uint8_t num, FAN_CTL fanctl);
void RdRlayStatus(void);
#endif
