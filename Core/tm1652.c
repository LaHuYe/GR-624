#include "tm1652.h"
#include "uart.h"

uint8_t dataCmd[7] = {0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t showCmd[2] = {0x18, 0x8E};

void tm1652_data_write(void);

void tm1652_Init(void)
{
    uart_init(); // 串口驱动
}

void tm1652_data_write(void)
{
    if (HAL_UART_Transmit(&UartHandle, (uint8_t *)dataCmd, sizeof(dataCmd), 1000) != HAL_OK)
    {
        // APP_ErrorHandler();
    }
    LL_mDelay(1);
    /* Sending data through POLLING */
    if (HAL_UART_Transmit(&UartHandle, (uint8_t *)showCmd, sizeof(showCmd), 1000) != HAL_OK)
    {
        // APP_ErrorHandler();
    }
    LL_mDelay(1);
}

void tm1652_led_show(USER_Led_TypeDef led, uint8_t state)
{
    uint8_t size = (((led - 1) / 4) + 1);
    if (state)
    {
        dataCmd[size] |= (1 << ((size * 4) - led));
    }
    else
    {
        dataCmd[size] &= ~(1 << ((size * 4) - led));
    }
    tm1652_data_write();
}
