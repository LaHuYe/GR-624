#ifndef __TIMPWM_H
#define __TIMPWM_H
#include "main.h"

void Tim1_Pwm_Init(void);
void Tim1_PwmStop(uint32_t Channel);
User_StatusTypeDef Tim1_PwmPulseSet(uint32_t Channel, uint32_t Pulse);
#endif
