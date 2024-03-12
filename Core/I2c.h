#ifndef __I2C_H
#define __I2C_H

#include "main.h"

#define I2C_ADDRESS      0xA0               //本机I2C地址
#define I2C_SPEEDCLOCK   100000             //传输速率
#define I2C_DUTYCYCLE    LL_I2C_DUTYCYCLE_16_9 //占空比

#define I2C_STATE_READY    0        /* Ready state */
#define I2C_STATE_BUSY_TX  1        /* Send state */
#define I2C_STATE_BUSY_RX  2        /* Receive state */

void i2c_init(void);
void APP_MasterTransmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
void APP_MasterReceive(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
uint32_t get_i2c_state(void);
#endif
