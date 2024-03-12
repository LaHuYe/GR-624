#include "pwr_stop.h"
#include "key.h"
#include "led.h"
#include "usb.h"

static void pwr_stop_Init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA); /* 使能GPIOA时钟 */
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB); /* 使能GPIOB时钟 */

    // 配置按键口为下降沿事件
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT; /* GPIO模式为下降沿事件 */
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;           /* 上拉 */
    GPIO_InitStruct.Pin = POWER_Pin;
    LL_GPIO_Init(POWER_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CHARGE_Pin;
    LL_GPIO_Init(CHARGE_GPIO_Port, &GPIO_InitStruct);

    LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTA,LL_EXTI_CONFIG_LINE0);
    LL_EXTI_SetEXTISource(LL_EXTI_CONFIG_PORTB,LL_EXTI_CONFIG_LINE5);

    /* Set EXTI0 to event patterns, falling edge trigger */
    LL_EXTI_InitTypeDef EXTI_InitStruct;
    EXTI_InitStruct.Line = LL_EXTI_LINE_5;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_EVENT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);

    EXTI_InitStruct.Line = LL_EXTI_LINE_0;
    LL_EXTI_Init(&EXTI_InitStruct);
}

static void pwr_wakeUp_Init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA); /* 使能GPIOA时钟 */
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB); /* 使能GPIOB时钟 */

    // 配置按键口为输入模式
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT; /* GPIO模式为下降沿事件 */
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;     /* 上拉 */
    GPIO_InitStruct.Pin = POWER_Pin;
    LL_GPIO_Init(POWER_GPIO_Port, &GPIO_InitStruct);

    // 配置充电口为输入模式
    GPIO_InitStruct.Pin = CHARGE_Pin;
    LL_GPIO_Init(CHARGE_GPIO_Port, &GPIO_InitStruct);
}


// 进入休眠模式
void mcu_enter_sleep(void)
{
    void feedback_time_init(void);
    feedback_time_init();

    pwr_stop_Init(); // 将电源按键以及充电检测初始化为唤醒模式

    /* systick中断关闭，防止systick中断唤醒 */
    LL_SYSTICK_DisableIT();

    /* TIM14中断关闭，防止TIM14中断唤醒 */
    LL_TIM_DisableIT_UPDATE(TIM14);

    /* Enable PWR clock */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    /* STOP mode with low power regulator ON */
    LL_PWR_SetLprMode(LL_PWR_LPR_MODE_LPR);

    /* SRAM retention voltage aligned with digital LDO output */
    LL_PWR_SetStopModeSramVoltCtrl(LL_PWR_SRAM_RETENTION_VOLT_CTRL_LDO);

    /* Enter DeepSleep mode */
    LL_LPM_EnableDeepSleep();

    /* Request Wait For Event */
    __SEV();
    __WFE();
    __WFE();

    LL_LPM_EnableSleep();
    
    /* systick中断开启 */
    LL_SYSTICK_EnableIT();

    /* TIM14中断开启 */
    LL_TIM_EnableIT_UPDATE(TIM14);

    pwr_wakeUp_Init(); // 按键和充电配置为GPIO输入模式
}
