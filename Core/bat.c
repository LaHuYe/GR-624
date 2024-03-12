#include "bat.h"
#include "adc.h"
#include "pump.h"

// uint8_t bat_level[4]=[25,50,75,100];

/**
 * @brief  获取电池百分比
 *
 * @return 返回电池百分比值
 */
uint8_t get_bat_percentage(void)
{
    uint32_t bat_vol, adc_vol;
    uint8_t bat_percentage;
    batPrintf(LOG_DEBUG, "get_bat_adc_value:%d\r\n", get_bat_adc_value());
    adc_vol = (ADC_POWER_VOL * get_bat_adc_value()) / 4095;
    bat_vol = (adc_vol * (BAT_ADC_R1 + BAT_ADC_R2)) / BAT_ADC_R2;
    batPrintf(LOG_DEBUG, "bat_vol %d\r\n", bat_vol);
    if (bat_vol <= BAT_MIN_VOL)
    {
        return 0;
    }
    else
    {
        bat_percentage = ((bat_vol - BAT_MIN_VOL) * 100) / (BAT_MAX_VOL - BAT_MIN_VOL);
        if (bat_percentage > 100)
        {
            bat_percentage = 100;
        }
        batPrintf(LOG_NOTIC, "bat_percentage %d\r\n", bat_percentage);
        return bat_percentage;
    }
}

// 获取电池电压
uint16_t get_bat_vol(void)
{
    uint16_t bat_vol, adc_vol;
    adc_vol = (ADC_POWER_VOL * get_bat_adc_value()) / 4095;
    bat_vol = adc_vol * 6;
    return bat_vol;
}

// 判断电池实际电压与补偿电压是否在同一区间里
static uint8_t VOL_Comparison(uint16_t vol1, uint16_t vol2)
{
    // 定义区间
    uint16_t intervals[] = {DISCHARGE_MIN_VOLTAGE_DISPLAY, 
                            DISCHARGE_VOLTAGE_DISPLAY_RANGE_1, 
                            DISCHARGE_VOLTAGE_DISPLAY_RANGE_2, 
                            DISCHARGE_VOLTAGE_DISPLAY_RANGE_3, 
                            DISCHARGE_MAX_VOLTAGE_DISPLAY};

    // 遍历每个区间
    for (size_t i = 0; i < ((sizeof(intervals) / sizeof(intervals[0])) - 1); i++)
    {
        if (vol1 >= intervals[i] && vol1 < intervals[i + 1] && vol2 >= intervals[i] && vol2 < intervals[i + 1])
        {
            return 1; // 如果在同一个区间内，返回1
        }
    }
    return 0; // 如果不在同一个区间内，返回0
}

BAT_Vol_t batVol;

void bat_vol_Init(void)
{
    memset(&batVol,0,sizeof(BAT_Vol_t));
}

//设置开机采集电压
void VOL_PowerGather(void)
{
    batVol.volGatherFlag = 1;
}

//设置LED位置
void LED_SetSite(uint8_t site)
{
    if (batVol.ledSite > site || batVol.ledSite == 0)
    {
        batVol.ledSite = site;
    }
}

//获取LED位置
uint8_t LED_GetSite(void)
{
    return batVol.ledSite;
}

//开机检测电压
uint8_t VOL_POST(void)
{
    batVol.actualVol = get_bat_vol();
    printf(" batVol.actualVol = %dmV batVol.dischargeShowVol = %dmV\r\n",batVol.actualVol,batVol.dischargeShowVol);
    if (batVol.actualVol >= PUMP_MIX_START_VOL || batVol.dischargeShowVol >= PUMP_MIX_START_VOL)   //实际电压比补偿的值大
        return 1;
    return 0;
        
}


// 电池充电函数，用于充电时显示电压百分比
uint8_t BAT_ChargeVol(void)
{
    switch (batVol.ledSite)
    {
    case 0:
        batVol.dischargeShowVol = DISCHARGE_MIN_VOLTAGE_DISPLAY + 1;
        break;
    case 1:
        batVol.dischargeShowVol = DISCHARGE_VOLTAGE_DISPLAY_RANGE_1 + 1;
        break;
    case 2:
        batVol.dischargeShowVol = DISCHARGE_VOLTAGE_DISPLAY_RANGE_2 + 1;
        break;
    case 3:
        batVol.dischargeShowVol = DISCHARGE_VOLTAGE_DISPLAY_RANGE_3 + 1;
        break;
    }

    if (LL_GetTickDiff(batVol.ledUpdataTime) >= 1800000)  // 大于30分钟
    {
        batVol.ledSite ++;
        if (batVol.ledSite > 3)
        {
            batVol.ledSite = 3;
            
        }
        batVol.ledUpdataTime = LL_GetTick();
        batPrintf(LOG_NOTIC, "batVol.ledUpdataTime:%d batVol.ledSite:%d\r\n", batVol.ledUpdataTime,batVol.ledSite);
    }
    return batVol.ledSite;
}
