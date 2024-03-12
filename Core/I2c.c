#include "I2c.h"

I2C_HandleTypeDef I2cHandle;

void i2c_init(void)
{
    I2cHandle.Instance = I2C;                                 /* I2C */
    I2cHandle.Init.ClockSpeed = I2C_SPEEDCLOCK;               /* I2C communication speed */
    I2cHandle.Init.DutyCycle = I2C_DUTYCYCLE;                 /* I2C duty cycle */
    I2cHandle.Init.OwnAddress1 = I2C_ADDRESS;                 /* I2C address */
    I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE; /* Prohibit broadcast calls */
    I2cHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;     /* Allow clock extension */
    if (HAL_I2C_Init(&I2cHandle) != HAL_OK)
    {
        APP_ErrorHandler();
    }
}

