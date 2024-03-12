#include "pump.h"
#include "adc.h"
#include "timpwm.h"

#define PUMP_START_TIME 2000
#define PUMP_END_TIME 1000
#define PUMP_MIN_PULSE 0
#define PUMP_MAX_PULSE 100

#define MOTOR_START_TIME 2000
#define MOTOR_END_TIME 1000
#define MOTOR_MIN_PULSE 11
#define MOTOR_MAX_PULSE 100

softStartConfig_t motor_SoftStartConfig = {0};
softStartConfig_t pump_SoftStartConfig = {0};
static uint8_t motor_pulseValue,pump_pulseValue;

// 电机占空比调节
static User_StatusTypeDef motor_PulseSet(uint32_t Pulse)
{
    return Tim1_PwmPulseSet(LL_TIM_CHANNEL_CH3, Pulse);
}
// 气泵占空比调节
static User_StatusTypeDef pump_PulseSet(uint32_t Pulse)
{
    return Tim1_PwmPulseSet(PUMP_DRIVE_PWM_CH, Pulse);
}

// PWM缓启动
static User_StatusTypeDef pwm_SoftStart(softStartConfig_t *softStartConfig)
{
    if (softStartConfig->maxPwmPulse == softStartConfig->minPwmPulse)
    {
        return STATUS_ERROR;
    }
    
    if (!softStartConfig->pwmStartFlsh)
    {
        softStartConfig->pwmStartFlsh = 1;
        softStartConfig->pwmStartTime = LL_GetTick();
        softStartConfig->intervalTime = softStartConfig->softStartTime / (softStartConfig->maxPwmPulse - softStartConfig->minPwmPulse);
        softStartConfig->minPwmRecord = softStartConfig->minPwmPulse;
        pumpPrintf(LOG_DEBUG,"interval_ms:%d\r\n", softStartConfig->intervalTime);
    }
    if (softStartConfig->intervalTime == 0)
    {
        return STATUS_ERROR;
    }
    softStartConfig->pulseValue = (LL_GetTickDiff(softStartConfig->pwmStartTime) / softStartConfig->intervalTime) + softStartConfig->minPwmRecord;
    //softStartConfig->pulseValue < softStartConfig->minPwmPulse是为了防溢出
    if (softStartConfig->pulseValue >= softStartConfig->maxPwmPulse || softStartConfig->pulseValue < softStartConfig->minPwmRecord)
    {
        softStartConfig->pulseValue = softStartConfig->maxPwmPulse;
        return STATUS_OVERFLOW; // PWM溢出
    }
    return STATUS_SUCCESS;
}

// PWM缓关闭
static User_StatusTypeDef pwm_SoftEnd(softStartConfig_t *softStartConfig)
{
    if (softStartConfig->maxPwmPulse == softStartConfig->minPwmPulse)
    {
        return STATUS_ERROR;
    }
    if (!softStartConfig->pwmEndFlsh)
    {
        softStartConfig->pwmEndFlsh = 1;
        softStartConfig->pwmEndTime = LL_GetTick();
        softStartConfig->intervalTime = softStartConfig->softEndTime / (softStartConfig->maxPwmPulse - softStartConfig->minPwmPulse);
        softStartConfig->maxPwmRecord = softStartConfig->maxPwmPulse;
        pumpPrintf(LOG_DEBUG,"interval_ms:%d\r\n", softStartConfig->intervalTime);
    }
    if (softStartConfig->intervalTime == 0)
    {
        return STATUS_ERROR;
    }
    
    softStartConfig->pulseValue = softStartConfig->maxPwmRecord - (LL_GetTickDiff(softStartConfig->pwmEndTime) / softStartConfig->intervalTime);
    //softStartConfig->pulseValue > softStartConfig->maxPwmPulse是为了防溢出
    if (softStartConfig->pulseValue <= softStartConfig->minPwmPulse || softStartConfig->pulseValue > softStartConfig->maxPwmRecord)
    {
        softStartConfig->pulseValue = softStartConfig->minPwmPulse;
        return STATUS_OVERFLOW;
    }
    return STATUS_SUCCESS;
}

// 电机PWM调节（有缓启动）
User_StatusTypeDef motor_pwm_adjust(uint8_t pulse)
{
    static uint8_t pulseLast;
    motor_SoftStartConfig.softStartTime = MOTOR_START_TIME;
    motor_SoftStartConfig.softEndTime = MOTOR_END_TIME;
    if (pulse != motor_pulseValue)
    {
        motor_pulseValue = pulse;
        motor_SoftStartConfig.pwmEndFlsh = 0;
        motor_SoftStartConfig.pwmStartFlsh = 0;
    }
    if (pulse >= motor_SoftStartConfig.pulseValue)
    {
        motor_SoftStartConfig.pwmEndFlsh = 0;
        motor_SoftStartConfig.minPwmPulse = motor_SoftStartConfig.pulseValue;
        if (motor_SoftStartConfig.minPwmPulse == 0)
        {
            motor_SoftStartConfig.minPwmPulse = MOTOR_MIN_PULSE;    //电机从占空比百分之10开始启动
        }
        motor_SoftStartConfig.maxPwmPulse = pulse;
        if (pwm_SoftStart(&motor_SoftStartConfig) == STATUS_SUCCESS)
        {
            if (pulseLast != motor_SoftStartConfig.pulseValue)
            {
                pumpPrintf(LOG_NOTIC," max motor_SoftStart pulseValue:%d\r\n", motor_SoftStartConfig.pulseValue);
                pulseLast = motor_SoftStartConfig.pulseValue;
                motor_PulseSet(motor_SoftStartConfig.pulseValue);
            }
            return STATUS_WAIT; // 启动中
        }
        motor_SoftStartConfig.pwmStartFlsh = 0;
        motor_SoftStartConfig.minPwmPulse = motor_SoftStartConfig.maxPwmPulse;
    }else
    {
        motor_SoftStartConfig.pwmStartFlsh = 0;
        motor_SoftStartConfig.maxPwmPulse = motor_SoftStartConfig.pulseValue;
        motor_SoftStartConfig.minPwmPulse = pulse;
        if (pwm_SoftEnd(&motor_SoftStartConfig) == STATUS_SUCCESS)
        {
            if (pulseLast != motor_SoftStartConfig.pulseValue)
            {
                pumpPrintf(LOG_NOTIC,"min motor_SoftStart pulseValue:%d\r\n", motor_SoftStartConfig.pulseValue);
                pulseLast = motor_SoftStartConfig.pulseValue;
                motor_PulseSet(motor_SoftStartConfig.pulseValue);
            }
            return STATUS_WAIT;
        }
        motor_SoftStartConfig.pwmEndFlsh = 0;
        motor_SoftStartConfig.maxPwmPulse = motor_SoftStartConfig.minPwmPulse;
        if (pulse == 0)
        {
            Tim1_PwmStop(LL_TIM_CHANNEL_CH3);
        }
    } 
    return STATUS_SUCCESS; // 启动完成
}

// 气泵PWM调节（有缓启动）
User_StatusTypeDef pump_pwm_adjust(uint8_t pulse)
{
    static uint8_t pulseLast;
    pump_SoftStartConfig.softStartTime = PUMP_START_TIME;
    pump_SoftStartConfig.softEndTime = PUMP_END_TIME;
    if (pulse != 0)
        LL_GPIO_SetOutputPin(PUMP_PWR_GPIO_PORT,PUMP_PWR_PIN);

    pump_pulseValue = pulse;
    if (pulse >= pump_SoftStartConfig.pulseValue)
    {
        pump_SoftStartConfig.pwmEndFlsh = 0;    //标志位得放在if里面，不然会出现在切换电机和切换气泵太频繁时，一直在启动中，导致没触发标志位，从而两个电机都停止
        pump_SoftStartConfig.minPwmPulse = pump_SoftStartConfig.pulseValue;
        if (pump_SoftStartConfig.minPwmPulse == 0)
        {
            pump_SoftStartConfig.minPwmPulse = PUMP_MIN_PULSE;
        }
        pump_SoftStartConfig.maxPwmPulse = pulse;
        if (pwm_SoftStart(&pump_SoftStartConfig) == STATUS_SUCCESS)
        {
            if (pulseLast != pump_SoftStartConfig.pulseValue)
            {
                pumpPrintf(LOG_NOTIC,"max pwm_SoftStart pulseValue:%d\r\n", pump_SoftStartConfig.pulseValue);
                pulseLast = pump_SoftStartConfig.pulseValue;
                pump_PulseSet(pump_SoftStartConfig.pulseValue);
            }
            return STATUS_WAIT; // 启动中
        }
        pump_SoftStartConfig.pwmStartFlsh = 0;
        pump_SoftStartConfig.minPwmPulse = pump_SoftStartConfig.maxPwmPulse;
        
        
    }else
    {
        pump_SoftStartConfig.pwmStartFlsh = 0;
        pump_SoftStartConfig.maxPwmPulse = pump_SoftStartConfig.pulseValue;
        pump_SoftStartConfig.minPwmPulse = pulse;
        if (pwm_SoftEnd(&pump_SoftStartConfig) == STATUS_SUCCESS)
        {
            if (pulseLast != pump_SoftStartConfig.pulseValue)
            {
				pumpPrintf(LOG_NOTIC,"min pwm_SoftStart pulseValue:%d\r\n", pump_SoftStartConfig.pulseValue);
                pulseLast = pump_SoftStartConfig.pulseValue;
                pump_PulseSet(pump_SoftStartConfig.pulseValue);
            }
            return STATUS_WAIT;
        }
        pump_SoftStartConfig.pwmEndFlsh = 0; 
        pump_SoftStartConfig.maxPwmPulse = pump_SoftStartConfig.minPwmPulse;  
        if (pulse == 0)
        {
            LL_GPIO_ResetOutputPin(PUMP_PWR_GPIO_PORT,PUMP_PWR_PIN);    //关闭电机电源
        }
    }  
    return STATUS_SUCCESS; // 启动完成
}

float get_pump_current(void)
{
    // uint32_t pump_vol;
    // float pump_current = 0.0f;
    // pumpPrintf(LOG_DEBUG,"get_pump_adc_value:%d\r\n", get_pump_adc_value());
    // pump_vol = (5000 * get_pump_adc_value()) / 4095;
    // pumpPrintf(LOG_DEBUG,"pump_vol:%dmV\r\n", pump_vol);
    // pump_current = pump_vol /10;
    // pumpPrintf(LOG_DEBUG,"pump_current:%fA\r\n", pump_current);
    // return pump_current;
	return 0;
}

float get_motor_current(void)
{
    // uint32_t motor_vol;
    // float motor_current = 0.0f;
    // pumpPrintf(LOG_DEBUG,"get_motor_adc_value:%d\r\n", get_motor_adc_value());
    // motor_vol = (5000 * get_motor_adc_value()) / 4095;
    // pumpPrintf(LOG_DEBUG,"motor_vol:%dmV\r\n", motor_vol);
    // motor_current = motor_vol /10;
    // pumpPrintf(LOG_DEBUG,"motor_current:%fA\r\n", motor_current);
    // return motor_current;
	return 0;
}



void motor_state(uint8_t pulse)
{
    motor_pulseValue = pulse;
}
void pump_state(uint8_t pulse)
{
    pump_pulseValue = pulse;
}

void motor_pump_handle(void)
{
    motor_pwm_adjust(motor_pulseValue);
    pump_pwm_adjust(pump_pulseValue);
}
#if 0

// 电机开启（缓启动）
User_StatusTypeDef motor_SoftStart(void)
{
    static uint8_t pulseLast;
    motor_SoftStartConfig.softStartTime = MOTOR_START_TIME;
    motor_SoftStartConfig.minPwmPulse = MOTOR_MIN_PULSE;
    motor_SoftStartConfig.maxPwmPulse = MOTOR_MAX_PULSE;
    if (pwm_SoftStart(&motor_SoftStartConfig) == STATUS_SUCCESS)
    {
        if (pulseLast != motor_SoftStartConfig.pulseValue)
        {
            pulseLast = motor_SoftStartConfig.pulseValue;
            motor_PulseSet(motor_SoftStartConfig.pulseValue);
        }
        return STATUS_WAIT; // 启动中
    }
    motor_SoftStartConfig.pwmEndFlsh = 0;
    return STATUS_SUCCESS; // 启动完成
}

// 气泵开启（缓启动）
User_StatusTypeDef pump_SoftStart(void)
{
    static uint8_t pulseLast;
    pump_SoftStartConfig.softStartTime = PUMP_START_TIME;
    pump_SoftStartConfig.minPwmPulse = PUMP_MIN_PULSE;
    pump_SoftStartConfig.maxPwmPulse = PUMP_MAX_PULSE;
    if (pwm_SoftStart(&pump_SoftStartConfig) == STATUS_SUCCESS)
    {
        if (pulseLast != pump_SoftStartConfig.pulseValue)
        {
            pulseLast = pump_SoftStartConfig.pulseValue;
            pump_PulseSet(pump_SoftStartConfig.pulseValue);
        }
        return STATUS_WAIT;
    }
    pump_SoftStartConfig.pwmEndFlsh = 0;
    return STATUS_SUCCESS;
}

// 电机关闭
User_StatusTypeDef motor_Softstop(void)
{
    static uint8_t pulseLast;
    motor_SoftStartConfig.softEndTime = MOTOR_END_TIME;
    motor_SoftStartConfig.minPwmPulse = MOTOR_MIN_PULSE;
    motor_SoftStartConfig.maxPwmPulse = MOTOR_MAX_PULSE;

    if (pwm_SoftEnd(&motor_SoftStartConfig) == STATUS_SUCCESS)
    {
        if (pulseLast != motor_SoftStartConfig.pulseValue)
        {
            pulseLast = motor_SoftStartConfig.pulseValue;
            pump_PulseSet(motor_SoftStartConfig.pulseValue);
        }
        return STATUS_WAIT;
    }
    motor_SoftStartConfig.pwmStartFlsh = 0;
    Tim1_PwmStop(TIM_CHANNEL_3);
    return STATUS_SUCCESS;
}
// 气泵关闭
User_StatusTypeDef pump_Softstop(void)
{
    static uint8_t pulseLast;
    pump_SoftStartConfig.softEndTime = PUMP_END_TIME;
    pump_SoftStartConfig.minPwmPulse = PUMP_MIN_PULSE;
    pump_SoftStartConfig.maxPwmPulse = PUMP_MAX_PULSE;

    if (pwm_SoftEnd(&pump_SoftStartConfig) == STATUS_SUCCESS)
    {
        if (pulseLast != pump_SoftStartConfig.pulseValue)
        {
            pulseLast = pump_SoftStartConfig.pulseValue;
            pump_PulseSet(pump_SoftStartConfig.pulseValue);
        }
        return STATUS_WAIT;
    }
    pump_SoftStartConfig.pwmStartFlsh = 0;
    Tim1_PwmStop(TIM_CHANNEL_4);
    return STATUS_SUCCESS;
}

//     uint32_t test_time;
//     while (1)
//     {
//         if (LL_GetTick() - test_time <= 10000)
//         {
//             pump_SoftStart();
//         }else if (LL_GetTick() - test_time <= 20000)
//         {
//             pump_Softstop();
//         }else
//         {
//             test_time = LL_GetTick();
//         }
//         // motor_SoftStart();
//     }可用此段代码复制到main函数里测试电机缓启动以及缓关闭

#endif

