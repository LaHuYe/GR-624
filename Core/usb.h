#ifndef __USB_H
#define __USB_H
#include "main.h"

#define USB_POWER_PIN LL_GPIO_PIN_12
#define USB_POWER_PORT GPIOA

void usb_power_init(void);
void usb_power_state(uint8_t state);
void usb_power_deInit(void);
#endif
