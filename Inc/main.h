/**
 ******************************************************************************
 * @file    main.h
 * @author  MCU Application Team
 * @brief   Header for main.c file.
 *          This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) Puya Semiconductor Co.
 * All rights reserved.</center></h2>
 *
 * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "log.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "py32c643_ll_rcc.h"
#include "py32c643_ll_bus.h"
#include "py32c643_ll_system.h"
#include "py32c643_ll_cortex.h"
#include "py32c643_ll_utils.h"
#include "py32c643_ll_pwr.h"
#include "py32c643_ll_gpio.h"
#include "py32c643_ll_exti.h"
#include "py32c643_ll_iwdg.h"
#include "py32c643_ll_adc.h"
#include "py32c643_ll_i2c.h"
#include "py32c643_ll_tim.h"
#include "py32c643xx_ll_Start_Kit.h"

/* Private includes ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Exported variables prototypes ---------------------------------------------*/
extern volatile uint32_t sysTickMillis;

/**********KEY GPIO*************/
#define KEY_UNIT_Pin LL_GPIO_PIN_5
#define KEY_UNIT_GPIO_Port GPIOA
#define KEY_UNIT_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA)
/*******************************/

/**********LED GPIO*************/
#define USER_LED1_PIN LL_GPIO_PIN_7
#define USER_LED1_GPIO_PORT GPIOB
#define USER_LED1_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB)

#define USER_LED2_PIN LL_GPIO_PIN_1
#define USER_LED2_GPIO_PORT GPIOC
#define USER_LED2_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC)
/*******************************/

/**********ADC GPIO*************/
// 隔膜泵检测ADC引脚
#define PUMP_ADC_PIN LL_GPIO_PIN_7
#define PUMP_ADC_GPIO_PORT GPIOA
#define PUMP_ADC_CH LL_ADC_CHANNEL_4
#define PUMP_ADC_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA)
/*******************************/

/*********TIM PWM GPIO**********/
// 隔膜泵 PWM IO口
#define PUMP_DRIVE_PIN LL_GPIO_PIN_1
#define PUMP_DRIVE_GPIO_PORT GPIOB
#define PUMP_DRIVE_PWM_CH LL_TIM_CHANNEL_CH4
#define PUMP_DRIVE_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB)

// 无刷电机 PWM IO口
#define MOTOR_DRIVE_PIN LL_GPIO_PIN_0
#define MOTOR_DRIVE_GPIO_PORT GPIOB
#define MOTOR_DRIVE_PWM_CH LL_TIM_CHANNEL_CH2
#define MOTOR_DRIVE_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB)
/*******************************/

/*********PUMP PWR GPIO**********/
// 电机 反馈 IO口
#define PUMP_FG_PIN LL_GPIO_PIN_0
#define PUMP_FG_GPIO_PORT GPIOA
#define PUMP_FG_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA)
/*******************************/

/*********UART1 GPIO**********/
// UART1 TX
#define UART1_TX_PIN LL_GPIO_PIN_6
#define UART1_TX_GPIO_PORT GPIOA
#define UART1_TX_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA)

// UART1 RX
#define UART1_RX_PIN
#define UART1_RX_GPIO_PORT
/*******************************/

/**********I2C GPIO*************/
// SDA引脚
#define SDA_I2C_PIN LL_GPIO_PIN_6
#define SDA_I2C_GPIO_PORT GPIOB
#define SDA_I2C_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB)
// SCL引脚
#define SCL_I2C_PIN LL_GPIO_PIN_2
#define SCL_I2C_GPIO_PORT GPIOA
#define SCL_I2C_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA)
/*******************************/

/**********BOARD STATE GPIO*************/
// 板级状态输出脚
#define BOARD_STATE_PIN LL_GPIO_PIN_5
#define BOARD_STATE_GPIO_PORT GPIOB
#define BOARD_STATE_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB)
/*******************************/

typedef enum
{
    STATUS_SUCCESS,
    STATUS_ERROR,
    STATUS_OVERFLOW,
    STATUS_WAIT,
    STATUS_TIMEOUT,
    // 可以继续添加其他状态
} User_StatusTypeDef;

/* Exported functions prototypes ---------------------------------------------*/
void APP_ErrorHandler(void);
void APP_UpdateCallback(void);
uint32_t LL_GetTick(void);
uint32_t LL_GetTickDiff(uint32_t meiosis);
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
