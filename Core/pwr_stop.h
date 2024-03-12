#ifndef __PWR_STOP_H
#define __PWR_STOP_H

#include "main.h"

#define POWER_Pin LL_GPIO_PIN_5
#define POWER_GPIO_Port GPIOB
#define KEY1_Pin LL_GPIO_PIN_4
#define KEY1_GPIO_Port GPIOA
#define KEY2_Pin LL_GPIO_PIN_3
#define KEY2_GPIO_Port GPIOA
#define KEY3_Pin LL_GPIO_PIN_1
#define KEY3_GPIO_Port GPIOA
#define CHARGE_Pin LL_GPIO_PIN_0
#define CHARGE_GPIO_Port GPIOA

void mcu_enter_sleep(void);

#endif
