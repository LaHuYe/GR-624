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
#include "py32c643_ll_tim.h"
#include "py32c643xx_ll_Start_Kit.h"

/* Private includes ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Exported variables prototypes ---------------------------------------------*/
extern volatile uint32_t sysTickMillis;

/**********KEY GPIO*************/
#define KEY_POWER_Pin LL_GPIO_PIN_5
#define KEY_POWER_GPIO_Port GPIOB
#define KEY_POWER_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
#define CHARGE_PIN LL_GPIO_PIN_0
#define CHARGE_GPIO_PORT GPIOA
#define CHARGE_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
#define FULL_CHARGE_PIN LL_GPIO_PIN_0
#define FULL_CHARGE_GPIO_PORT GPIOB
#define FULL_CHARGE_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
/*******************************/

/**********LED GPIO*************/
#define USER_LED1_PIN LL_GPIO_PIN_5
#define USER_LED1_GPIO_PORT GPIOA
#define USER_LED1_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

#define USER_LED2_PIN LL_GPIO_PIN_6
#define USER_LED2_GPIO_PORT GPIOA
#define USER_LED2_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

#define USER_LED3_PIN LL_GPIO_PIN_7
#define USER_LED3_GPIO_PORT GPIOA
#define USER_LED3_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

#define USER_LED4_PIN LL_GPIO_PIN_1
#define USER_LED4_GPIO_PORT GPIOC
#define USER_LED4_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);

#define USER_LED5_PIN LL_GPIO_PIN_7
#define USER_LED5_GPIO_PORT GPIOB
#define USER_LED5_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
/*******************************/

/**********ADC GPIO*************/
// 电池检测ADC引脚
#define ADC_BAT_PIN LL_GPIO_PIN_4
#define ADC_BAT_GPIO_PORT GPIOA
#define ADC_BAT_CH LL_ADC_CHANNEL_2
#define ADC_BAT_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
/*******************************/

/*********PUMP PWR GPIO**********/
// 电机 电源 IO口
#define PUMP_PWR_PIN LL_GPIO_PIN_1
#define PUMP_PWR_GPIO_PORT GPIOB
#define PUMP_PWR_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

// 电机 反馈 IO口
#define PUMP_FG_PIN LL_GPIO_PIN_2
#define PUMP_FG_GPIO_PORT GPIOB
#define PUMP_FG_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
/*******************************/

/*********UART1 GPIO**********/
// UART1 TX
#define UART1_TX_PIN GPIO_PIN_4
#define UART1_TX_GPIO_PORT GPIOB
// UART1 RX
#define UART1_RX_PIN
#define UART1_RX_GPIO_PORT
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
