#ifndef __APPLICATION_H
#define __APPLICATION_H

#include "main.h"
#include "timpwm.h"
#include "pump.h"
#include "iwdg.h"
#include "key.h"
#include "tim.h"
#include "led.h"
#include "adc.h"
#include "bat.h"
#include "pwr_stop.h"

#define DOWNLOAD_DATE "2024/2/6"
// GR-624是项目编号，01是大改动（如增加新功能，换板子等），000是修bug
#define VERSION "GR-624-Bat-00.000"
#define COMMIT_HASH ""

#define MOTOR_OFF 0                            // 关闭电机的PWM占空比
#define LOW_PRESSURE_LEVEL1_PWM_DUTY_CYCLE 35  // 低压挡位1PWM占空比    50W
#define LOW_PRESSURE_LEVEL2_PWM_DUTY_CYCLE 54  // 低压挡位2PWM占空比    70W
#define LOW_PRESSURE_LEVEL3_PWM_DUTY_CYCLE 72  // 低压挡位3PWM占空比    90W
#define HIGH_PRESSURE_LEVEL1_PWM_DUTY_CYCLE 86 // 高压挡位电机PWM占空比 105W

#define LOW_PRESSURE_LEVEL1_STOP_PRESSURE 6000  // 低压挡位1停止气压
#define LOW_PRESSURE_LEVEL2_STOP_PRESSURE 7000  // 低压挡位2停止气压
#define LOW_PRESSURE_LEVEL3_STOP_PRESSURE 8000  // 低压挡位3停止气压(低档的停止电压需等于或高于高压档位的临界气压，不然一体模式下从高压档切回低压档会马上停机)
#define HIGH_PRESSURE_LEVEL_STOP_PRESSURE 50000 // 高压挡位停止气压

#define HIGH_LOW_PRESSURE_CHANGE_INTERVAL 10000 // 电机和气泵的切换间隔
#define LOW_PRESSURE_THRESHOLD 5000             // 切换为低压档位的临界气压,单位Pa
#define HIGH_PRESSURE_THRESHOLD 12000           // 切换为高压档位的临界气压,单位Pa

#define BATTERY_LOW_LEVEL_VALUE 10         // 电池低电量值
#define IDLE_STATE_SLEEP_TIME 20000        // 空闲状态休眠时间（单位：豪秒）
#define BATTERY_INTERNAL_RESISTANCE 5      // 电池内阻补偿值
#define FULL_BATTERY_TIME_COMPENSATION 120 // 电池充满后时间补偿（单位：分钟）

// 气泵运行时电流参数
#define PUMP_CURRENT_THRESHOLD_HIGH 10.0    // 超过的最大电流
#define PUMP_CURRENT_THRESHOLD_LOW 0.000001 // 超过的最小电流
#define MAX_PUMP_SAMPLES 30                 // 采集的次数

#define charg_level_gear1 0
#define charg_level_gear2 50
#define charg_level_gear3 70
#define charg_level_gear4 85
#define charg_level_gear5 100

typedef enum
{
    USER_LED1,
    USER_LED2,
    USER_LED3,
    USER_LED4,
    USER_LED5,
    USER_LED_MAX
} USER_Led_TypeDef;

typedef enum
{
    BOARD_IDLE = 0,      // 空闲状态
    BOARD_LOW_PRESSURE,  // 低压工作状态
    BOARD_HIGH_PRESSURE, // 高压工作状态
    BOARD_WORK,          // 工作状态
    BOARD_STOP,          // 休眠状态
    BOARD_CHARGE,        // 充电状态
    BOARD_DISCONNECT,    // 板子连接或者断开状态
} board_state_t;

typedef enum
{
    NO_STATUS,            // 无状态
    UNDER_VOL_STATUS,     // 欠压状态
    OVER_CURRENT_STATUS,  // 过流状态
    JOB_COMPLETE_STATUS,  // 工作完成状态
    MOTOR_RUNNING_STATUS, // 电机工作状态
    PUMP_RUNNING_STATUS   // 隔膜泵工作状态
} motor_board_state_t;

typedef enum
{
    BOARD_ONE_PIECE_MODE, // 一体模式
    BOARD_SPLIT_MODE,     // 分离模式
} board_mode_t;

typedef enum
{
    NOT_GEAR,            // 无挡位
    LOW_PRESSURE_GEAR_1, // 低压1档
    LOW_PRESSURE_GEAR_2, // 低压2档
    LOW_PRESSURE_GEAR_3, // 低压3档
    HIGH_PRESSURE_GEAR,  // 高压档
} pressure_gear_t;

typedef enum
{
    KEY_UNLOCK, // 解锁
    KEY_LOCK,   // 锁定
} key_lock_t;

typedef struct
{
    uint8_t charge_full_valid_flag;  // 电池充满有效标志位（因为拔出充电器的时候总是会先触发充满事件再出发拔电事件，所以加这个标志位用于判断是否充满）
    uint32_t charge_full_start_time; // 电池满电时的时间戳
} bat_full_t;

typedef struct
{
    board_state_t board_state;
    board_mode_t board_mode;
    pressure_gear_t pressure_gear;
    key_lock_t key_lock;            // 按键锁定
    bat_full_t bat_full;            // 电池充满结构体
    uint8_t motor_board_state;      // 电机板反馈状态
    uint8_t board_fiting_flag;      // 板子启动标志位
    uint8_t balance_charge_flag;    // 均衡充电标志位
    uint8_t bat_low_flag;           // 缺电标志位，用于触发缺点后在开机时会闪烁几次然后关机
    float pressure_value;           // 气压值
    uint32_t idle_state_start_time; // 空闲状态开始时间
    uint32_t change_interval;       // 切换最小间隔
} state_machine_t;                  // 状态机结构体(防止电机和气泵切换过于频繁，看起来像异常)

void app_Init(void);
void app_lication(void);

#endif
