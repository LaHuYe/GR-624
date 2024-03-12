#ifndef __TM1652_H
#define __TM1652_H
#include "main.h"
typedef enum
{
    USER_LED1 = 1,
    USER_LED2,
    USER_LED3,
    USER_LED4,
    USER_LED5,
    USER_LED6,
    USER_LED7,
    USER_LED8,
    USER_LED9,
    USER_LED10,
    USER_LED11,
    USER_LED12,
    USER_LED13,
    USER_LED14,
    USER_LED_MAX
}USER_Led_TypeDef;

void tm1652_Init(void);
void tm1652_led_show(USER_Led_TypeDef led,uint8_t state);

#endif
