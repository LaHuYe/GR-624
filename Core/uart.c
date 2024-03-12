#include "uart.h"

uint8_t *TxBuff = NULL;
__IO uint16_t TxCount = 0;

//TM1652串口驱动 波特率位19200，数据为为9位，奇校验
//注意，加上奇偶校验的话，数据为必须为9位
void uart_init(void)
{
    /* Enable GPIOB clock */
    UART1_TX_GPIO_CLK_ENABLE();
    /* Enable USART1 peripheral clock */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);

    /* Initialize PB4 */
    LL_GPIO_InitTypeDef GPIO_InitStruct;
    /* Select pin 4 */
    GPIO_InitStruct.Pin = UART1_TX_PIN;
    /* Select alternate mode */
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    /* Set output speed */
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    /* Set output type to push pull */
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    /* Enable pull up */
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    /* Set alternate function to USART1 function  */
    GPIO_InitStruct.Alternate = LL_GPIO_AF1_USART1;
    /* Initialize GPIOB */
    LL_GPIO_Init(UART1_TX_GPIO_PORT,&GPIO_InitStruct);


    /* Set USART feature */
    LL_USART_InitTypeDef USART_InitStruct;
    /* Set baud rate */
    USART_InitStruct.BaudRate = 19200;
    /* set word length to 8 bits: Start bit, 8 data bits, n stop bits */
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_9B;
    /* 1 stop bit */
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    /* Parity control disabled  */
    USART_InitStruct.Parity = LL_USART_PARITY_ODD;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    /* Initialize USART */
    LL_USART_Init(USART1, &USART_InitStruct);

    /* Configure as full duplex asynchronous mode */
    LL_USART_ConfigAsyncMode(USART1);

    /* Enable UART */
    LL_USART_Enable(USART1);
}

void APP_UsartTransmit(uint8_t *pData, uint16_t Size)
{
    TxBuff = pData;
    TxCount = Size;

    /* transmit data */
    while (TxCount > 0)
    {
        /* Wait for TXE bit to be set */
        while(LL_USART_IsActiveFlag_TXE(USART1) != 1);
        /* transmit data */
        LL_USART_TransmitData8(USART1, *TxBuff);
        TxBuff++;
        TxCount--;
    }

    /* Wait for TC bit to be set */
    while(LL_USART_IsActiveFlag_TC(USART1) != 1);
}
