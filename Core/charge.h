#ifndef __CHARGE_H
#define __CHARGE_H

#include "main.h"
#if 0
#define ON 1
#define OFF 0

#define CHARGE_PIN LL_GPIO_PIN_3
#define CHARGE_GPIO_PORT GPIOB

#define FULL_CHARGE_PIN LL_GPIO_PIN_15
#define FULL_CHARGE_GPIO_PORT GPIOA

// 电池1旁路引脚
#define BALANCE1_PIN LL_GPIO_PIN_1
#define BALANCE1_GPIO_PORT GPIOA
// 电池2旁路引脚
#define BALANCE2_PIN LL_GPIO_PIN_2
#define BALANCE2_GPIO_PORT GPIOA
// 电池3旁路引脚
#define BALANCE3_PIN LL_GPIO_PIN_0
#define BALANCE3_GPIO_PORT GPIOA

void bat_bypass_init(void);
void balanceChargeBattery(uint8_t state);
#endif
#endif
