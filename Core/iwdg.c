#include "iwdg.h"

void iwdg_Init(void)
{
    /* Enable LSI */
    LL_RCC_LSI_Enable();
    while (LL_RCC_LSI_IsReady() == 0U) {;}

    /* Enable IWDG */
    LL_IWDG_Enable(IWDG);

    /* Enable write access to IWDG_PR, IWDG_RLR and IWDG_WINR registers */
    LL_IWDG_EnableWriteAccess(IWDG);

    /* Set IWDG prescaler */
    LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_32); /* T=1MS */

    /* Set IWDG reload value */
    LL_IWDG_SetReloadCounter(IWDG, IWDG_Reload); /* 1ms*1000=1s */

    /* Check if all flags Prescaler, Reload & Window Value Update are reset or not */
    while (LL_IWDG_IsReady(IWDG) == 0U) {;}

    /* Reloads IWDG counter with value defined in the reload register */
    LL_IWDG_ReloadCounter(IWDG);
}

void iwdg_FeedDog(void)
{
    LL_IWDG_ReloadCounter(IWDG);
}
