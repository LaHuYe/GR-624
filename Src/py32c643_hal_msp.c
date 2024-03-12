/**
 ******************************************************************************
 * @file    py32c643_hal_msp.c
 * @author  MCU Application Team
 * @brief   This file provides code for the MSP Initialization
 *          and de-Initialization codes.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* External functions --------------------------------------------------------*/

/**
 * @brief Initialize Global MSP.
 */
void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
}

/**
 * @brief Initialize MSAR for USART.
 * @param huart：Initialize the handle of USART
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();

    /* GPIO initialization
    PA03：TX
    PB05：RX
    */
    GPIO_InitStruct.Pin = UART1_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(UART1_TX_GPIO_PORT, &GPIO_InitStruct);

    //   GPIO_InitStruct.Pin = GPIO_PIN_5;
    //   GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    //   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART1 interrupt enable */
    //   HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
    //   HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/**
 * @brief Initialize TIM MSP
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        GPIO_InitTypeDef GPIO_InitStruct;
        /* Enable TIM1 Clock */
        __HAL_RCC_TIM1_CLK_ENABLE();
        /* Enable GPIOA Clock */
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* Mode AF-PP */
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        /* PULLUP */
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        /* SPEED HIGH */
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

        // 电机IO口
        GPIO_InitStruct.Pin = MOTOR_PWM_PIN;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
        HAL_GPIO_Init(MOTOR_PWM_GPIO_PORT, &GPIO_InitStruct);

        // 气泵IO口
        GPIO_InitStruct.Pin = PUMP_PWM_PIN;
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
        HAL_GPIO_Init(PUMP_PWM_GPIO_PORT, &GPIO_InitStruct);
    }
    if (htim->Instance == TIM14)
    {
        /* Enable TIM1 Clock */
        __HAL_RCC_TIM14_CLK_ENABLE(); 
        
        /* Interrupt Configuration */
        NVIC_EnableIRQ(TIM14_IRQn);
        NVIC_SetPriority(TIM14_IRQn,0);
    }
    
}

/**
  * @brief Initialize ADC MSP
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();             
    __HAL_RCC_GPIOB_CLK_ENABLE(); /* GPIOB clock enable */
    __HAL_RCC_GPIOC_CLK_ENABLE(); /* GPIOB clock enable */
    
    GPIO_InitStruct.Pin = KEY_ADC_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(KEY_ADC_GPIO_PORT, &GPIO_InitStruct);       

    GPIO_InitStruct.Pin = BALANCE1_ADC_PIN;
    HAL_GPIO_Init(BALANCE1_ADC_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BALANCE2_ADC_PIN;
    HAL_GPIO_Init(BALANCE2_ADC_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BALANCE3_ADC_PIN;
    HAL_GPIO_Init(BALANCE3_ADC_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = PUMP_ADC_PIN;
    HAL_GPIO_Init(PUMP_ADC_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MOTOR_ADC_PIN;
    HAL_GPIO_Init(MOTOR_ADC_GPIO_PORT, &GPIO_InitStruct);
}
/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
