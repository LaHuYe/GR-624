#include "tim.h"
// 24000000/1000/24=1KHz
// 定时器计数
#define TIM14_PERIOD (1000 - 1)
// 定时器预分频
#define TIM14_PRESCALER (24 - 1)
/**
 * @brief  TIM2 Config
 * @param  None
 * @retval None
 */
void Tim14_Init(void)
{
    LL_APB1_GRP2_EnableClock(RCC_APBENR2_TIM14EN);
    /* Configure TIM14 */
    LL_TIM_InitTypeDef TIM14CountInit = {0};

    TIM14CountInit.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1; /* tDTS=tCK_IN                      */
    TIM14CountInit.CounterMode = LL_TIM_COUNTERMODE_UP;       /* Counter used as upcounter        */
    TIM14CountInit.Prescaler = TIM14_PERIOD;                         /* clock prescaler：8000            */
    TIM14CountInit.Autoreload = TIM14_PRESCALER;                     /* Autoreload value：3000           */
    TIM14CountInit.RepetitionCounter = 0;                     /* Repetition counter value：0      */

    /* Initialize TIM14*/
    LL_TIM_Init(TIM14, &TIM14CountInit);

    /* Clear the update interrupt flag (UIF).  */
    LL_TIM_ClearFlag_UPDATE(TIM14);

    /*Enable update interrupt (UIE) */
    LL_TIM_EnableIT_UPDATE(TIM14);

    /*Enable TIM14 */
    LL_TIM_EnableCounter(TIM14);

    /*Enable UPDATE interrupt */
    NVIC_EnableIRQ(TIM14_IRQn);
    NVIC_SetPriority(TIM14_IRQn, 0);
}
