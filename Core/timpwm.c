#include "timpwm.h"
#include "main.h"

//24000000/100/24=10KHz
// 定时器计数
#define TIM1_PERIOD (100 - 1)
// 定时器预分频
#define TIM1_PRESCALER (24 - 1)


void Tim1_PwmConfig(void);

/**
 * @brief  TIM Configure
 * @param  None
 * @retval None
 */
void Tim1_Pwm_Init(void)
{
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM1);

    /* Configure TIM1 */
    LL_TIM_InitTypeDef TIM1CountInit = {0};
    
    TIM1CountInit.ClockDivision       = LL_TIM_CLOCKDIVISION_DIV1;/* Set divider:tDTS=tCK_INT  */
    TIM1CountInit.CounterMode         = LL_TIM_COUNTERMODE_UP;    /* count mode：up count      */
    TIM1CountInit.Prescaler           = TIM1_PRESCALER;                   /* clock prescaler：24     */
    TIM1CountInit.Autoreload          = TIM1_PERIOD;                   /* auto-reload value：100   */
    TIM1CountInit.RepetitionCounter   = 0;                        /* recount：0                */
    
    /* Initialize TIM1 */
    LL_TIM_Init(TIM1,&TIM1CountInit);
    
    /* Enable output */
    LL_TIM_EnableAllOutputs(TIM1);

    /* Enable TIM1 */
    LL_TIM_EnableCounter(TIM1);

    Tim1_PwmConfig();
}
/**
 * @brief  TIM1 PWM Configure
 * @param  None
 * @retval None
 */
void Tim1_PwmConfig(void)
{
    PUMP_DRIVE_GPIO_CLK_ENABLE();
    
    LL_GPIO_InitTypeDef TIM1CH1MapInit= {0};
    LL_TIM_OC_InitTypeDef TIM_OC_Initstruct ={0};

    TIM1CH1MapInit.Pin        = PUMP_DRIVE_PIN;
    TIM1CH1MapInit.Mode       = LL_GPIO_MODE_ALTERNATE;
    TIM1CH1MapInit.Alternate  = LL_GPIO_AF_3; 
    LL_GPIO_Init(PUMP_DRIVE_GPIO_PORT,&TIM1CH1MapInit);
    
    /* Set PWM channel */
    TIM_OC_Initstruct.OCMode        = LL_TIM_OCMODE_PWM1;       /* channle mode：PWM2         */
    TIM_OC_Initstruct.OCState       = LL_TIM_OCSTATE_ENABLE;    /* Enable selected channle    */
    TIM_OC_Initstruct.OCPolarity    = LL_TIM_OCPOLARITY_HIGH;   /* channle active level：high */
    TIM_OC_Initstruct.OCIdleState   = LL_TIM_OCIDLESTATE_LOW;   /* channle idle polarity：low */
    /* CH1 compare value:250 */
    TIM_OC_Initstruct.CompareValue  = 0;
    /* Configure CH1 */
    LL_TIM_OC_Init(TIM1,PUMP_DRIVE_PWM_CH,&TIM_OC_Initstruct);
}
// 设置PWM占空比
User_StatusTypeDef Tim1_PwmPulseSet(uint32_t Channel, uint32_t Pulse)
{
    if (Channel == LL_TIM_CHANNEL_CH1)
    {
        LL_TIM_OC_SetCompareCH1(TIM1,TIM1_PERIOD * Pulse * 0.01);
    }else if (Channel == LL_TIM_CHANNEL_CH2)
    {
        LL_TIM_OC_SetCompareCH2(TIM1,TIM1_PERIOD * Pulse * 0.01);
    }else if (Channel == LL_TIM_CHANNEL_CH3)
    {
        LL_TIM_OC_SetCompareCH3(TIM1,TIM1_PERIOD * Pulse * 0.01);
    }else if (Channel == LL_TIM_CHANNEL_CH4)
    {
        LL_TIM_OC_SetCompareCH4(TIM1,TIM1_PERIOD * Pulse * 0.01);
    }
    return STATUS_SUCCESS;
}
// 关闭pwm
void Tim1_PwmStop(uint32_t Channel)
{
    LL_TIM_CC_DisableChannel(TIM1, Channel);
}
/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
