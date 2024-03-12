#include "main.h"
#include "application.h"

volatile uint32_t sysTickMillis = 0;

static void APP_SystemClockConfig(void);

int main(void)
{
    APP_SystemClockConfig(); 
    LL_mDelay(1000);
    app_Init();
    app_lication();
}

/**
  * @brief  Configure Systemclock
  * @param  None
  * @retval None
  */
static void APP_SystemClockConfig(void)
{
    /* Enable HSI */
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1)
    {
    }

    /* Set AHB divider: HCLK = SYSCLK */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

    /* HSISYS used as SYSCLK clock source  */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSISYS);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSISYS)
    {
    }

    /* Set APB1 divider */
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_Init1msTick(24000000);

    /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
    LL_SetSystemCoreClock(24000000);
    /* systick中断开启 */
    LL_SYSTICK_EnableIT();
}

uint32_t LL_GetTick(void)
{
    return sysTickMillis;
}

/**
  * @brief  Tim Period Elaspsed Callback Function
  * @param  None
  * @retval None
  */
void APP_UpdateCallback(void)
{
    keyCheckProcess();
}
// 计算时间差
uint32_t LL_GetTickDiff(uint32_t meiosis)
{
    uint32_t temp = LL_GetTick();
    if (temp >= meiosis)
    {
        temp = temp - meiosis;
    }
    else
    {
        temp = 0xFFFFFFFFU - meiosis + temp;
    }
    return temp;
}
void APP_ErrorHandler(void)
{

}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* Users can add their own printing information as needed,
       for example: //printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* Infinite loop */
    while (1)
    {
    }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
