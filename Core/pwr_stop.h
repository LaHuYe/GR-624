#ifndef __PWR_STOP_H
#define __PWR_STOP_H

#include "main.h"

#define POWER_Pin LL_GPIO_PIN_5
#define POWER_GPIO_Port GPIOB
#define CHARGE_Pin LL_GPIO_PIN_0
#define CHARGE_GPIO_Port GPIOA

void mcu_enter_sleep(void);

#endif
