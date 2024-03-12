#ifndef __I2C_H
#define __I2C_H

#include "main.h"

#define I2C_ADDRESS      0xA0               //本机I2C地址
#define I2C_SPEEDCLOCK   100000             //传输速率
#define I2C_DUTYCYCLE    I2C_DUTYCYCLE_16_9 //占空比

void i2c_init(void);

#endif
