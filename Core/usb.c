#include "usb.h"
#include "adc.h"

void usb_power_init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA); /* 使能GPIOA时钟 */

    GPIO_InitStruct.Pin = USB_POWER_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;      /* 推挽输出 */
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;          /* 使能上拉 */
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH; /* GPIO速度 */
    /* GPIO初始化 */
    LL_GPIO_Init(USB_POWER_PORT, &GPIO_InitStruct);
    usb_power_state(1);
}

void usb_power_state(uint8_t state)
{
    if (state)
    {
        LL_GPIO_SetOutputPin(USB_POWER_PORT, USB_POWER_PIN);
    }
    else
    {
        LL_GPIO_ResetOutputPin(USB_POWER_PORT, USB_POWER_PIN);
    }
}

void usb_power_deInit(void)
{
    // LL_GPIO_DeInit(USB_POWER_PORT, USB_POWER_PIN);
}
