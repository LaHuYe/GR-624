#ifndef __PUMP_H
#define __PUMP_H

#include "main.h"

// 气泵占空比
#define PUMP_PULSE

#define PUMP_START_TIME 2000
#define PUMP_END_TIME 2000
#define PUMP_MIN_PULSE 0
#define PUMP_MAX_PULSE 100

#define MOTOR_START_TIME 2000
#define MOTOR_END_TIME 1000
#define MOTOR_MIN_PULSE 11
#define MOTOR_MAX_PULSE 100

typedef enum
{
    PUMP_TYPE   = 0x00U,
    MOTOR_TYPE  = 0x01U
} PWM_StartTypeDef;

typedef struct
{
    uint8_t pwmStartFlsh;
    uint8_t pwmEndFlsh;
    uint8_t minPwmPulse;
    uint8_t maxPwmPulse;
    uint8_t minPwmRecord;
    uint8_t maxPwmRecord;
    uint8_t pulseValue;
    uint32_t pwmStartTime;
    uint32_t pwmEndTime;
    uint32_t softStartTime; 
    uint32_t softEndTime; 
    uint32_t intervalTime;
} softStartConfig_t;


uint32_t LL_GetTickDiff(uint32_t meiosis);
User_StatusTypeDef motor_SoftStart(void);
User_StatusTypeDef pump_SoftStart(void);
User_StatusTypeDef motor_Softstop(void);
User_StatusTypeDef pump_Softstop(void);

User_StatusTypeDef motor_pwm_adjust(uint8_t pulse, uint16_t start_time, uint16_t end_time);
User_StatusTypeDef pump_pwm_adjust(uint8_t pulse, uint16_t start_time, uint16_t end_time);

float get_pump_current(void);
float get_motor_current(void);
void motor_state(uint8_t pulse, uint16_t start_time, uint16_t end_time);
void pump_state(uint8_t pulse, uint16_t start_time, uint16_t end_time);
void motor_pump_handle(void);
#endif
