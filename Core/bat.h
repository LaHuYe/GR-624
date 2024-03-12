#ifndef __BAT_H
#define __BAT_H

#include "main.h"

#define BAT_ADC_R1 1000000 // 1MΩ
#define BAT_ADC_R2 200000  // 200KΩ

#define BAT_MAX_VOL 12600
#define BAT_MIN_VOL 8400

#define PUMP_MIX_START_VOL 8400     // 电机最小启动电压

#define DISCHARGE_MIN_VOLTAGE_DISPLAY 8400     // 放电电压显示最小值
#define DISCHARGE_VOLTAGE_DISPLAY_RANGE_1 9450 // 放电电压显示区间1临界值
#define DISCHARGE_VOLTAGE_DISPLAY_RANGE_2 10500 // 放电电压显示区间2临界值
#define DISCHARGE_VOLTAGE_DISPLAY_RANGE_3 11550 // 放电电压显示区间3临界值
#define DISCHARGE_MAX_VOLTAGE_DISPLAY 12600     // 放电电压显示最大值

#define CHARGE_MIN_VOLTAGE_DISPLAY 8400     // 充电电压显示最小值
#define CHARGE_VOLTAGE_DISPLAY_RANGE_1 9450 // 充电电压显示区间1临界值
#define CHARGE_VOLTAGE_DISPLAY_RANGE_2 10500 // 充电电压显示区间2临界值
#define CHARGE_VOLTAGE_DISPLAY_RANGE_3 11550 // 充电电压显示区间3临界值
#define CHARGE_MAX_VOLTAGE_DISPLAY 12600     // 充电电压显示最大值

__packed typedef struct
{
    uint8_t ledSite;           // LED电量显示位置
    uint32_t ledUpdataTime;           // LED更新时间戳
    uint8_t volGatherFlag;     // 开机电压采集标志位
    uint16_t actualVol;        // 实际电压（电机启动前的电压）
    uint16_t currentVol;       // 当前电压（电机启动后的电压）
    uint16_t offsetVol;        // 补偿电压（通过运算得出来的电压）
    uint16_t chargeShowVol;    // 充电显示电压（用于显示LED的电压）
    uint16_t dischargeShowVol; // 放电显示电压（用于显示LED的电压）
    float internalRes;         // 电池内阻(mΩ)
} BAT_Vol_t;

uint8_t get_bat_percentage(void);
uint8_t get_low_bat_state(void);
uint16_t get_bat_vol(void);
void bat_vol_Init(void);
uint8_t VOL_POST(void);
void VOL_PowerGather(void);
void LED_SetSite(uint8_t site);
uint16_t BAT_DischargeVol(void);
uint8_t BAT_ChargeVol(void);

#endif
