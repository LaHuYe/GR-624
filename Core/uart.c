#include "uart.h"

UART_HandleTypeDef UartHandle;

//TM1652串口驱动 波特率位19200，数据为为9位，奇校验
//注意，加上奇偶校验的话，数据为必须为9位
void uart_init(void)
{
    UartHandle.Instance = USART1;
    UartHandle.Init.BaudRate = 19200;
    UartHandle.Init.WordLength = UART_WORDLENGTH_9B;
    UartHandle.Init.StopBits = UART_STOPBITS_1;
    UartHandle.Init.Parity = UART_PARITY_ODD;
    UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode = UART_MODE_TX;
    HAL_UART_Init(&UartHandle);
}


// uint8_t aTxBuffer[7] = {0x08, 0x08, 0x04, 0x02, 0x08, 0x00, 0x00};
// uint8_t aRxBuffer[2] = {0x18, 0xFE};
// while (1)
// {
//     if (HAL_UART_Transmit(&UartHandle, (uint8_t *)aTxBuffer, sizeof(aTxBuffer), 5000) != HAL_OK)
//     {
//         APP_ErrorHandler();
//     }
//     LL_mDelay(3);
//     /* Sending data through POLLING */
//     if (HAL_UART_Transmit(&UartHandle, (uint8_t *)aRxBuffer, sizeof(aRxBuffer), 5000) != HAL_OK)
//     {
//         APP_ErrorHandler();
//     }
//     LL_mDelay(1000);
// }此段代码可测试LED灯

