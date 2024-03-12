#ifndef __UART_H
#define __UART_H
#include "main.h"

void uart_init(void);
void APP_UsartTransmit(uint8_t *pData, uint16_t Size);

#endif
