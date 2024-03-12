#include "I2c.h"

uint8_t         *pBuffPtr   = NULL;
__IO uint16_t   XferCount   = 0;
__IO uint32_t   Devaddress  = 0;
__IO uint32_t   State       = I2C_STATE_READY;

void i2c_init(void)
{
    SDA_I2C_GPIO_CLK_ENABLE();
    SCL_I2C_GPIO_CLK_ENABLE();

    LL_GPIO_SetPinPull(SDA_I2C_GPIO_PORT,SDA_I2C_PIN,LL_GPIO_PULL_NO);   //SWDIO引脚默认是有上拉的，需要关闭
    LL_GPIO_SetPinPull(SCL_I2C_GPIO_PORT,SCL_I2C_PIN,LL_GPIO_PULL_NO);   //SWCLK引脚默认是有下拉的，需要关闭

    /* Enabke I2C1 peripheral clock */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

    /* Set PB3 to SCL pin , Select alternate function mode
    and fast output speed. output type is Selected open-drain,
    Enable I/O pull up*/
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = SCL_I2C_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
    LL_GPIO_Init(SCL_I2C_GPIO_PORT, &GPIO_InitStruct);

    /* Set PB4 to SDA pin , Select alternate function mode
    and fast output speed. output type is Selected open-drain,
    Enable I/O pull up*/
    GPIO_InitStruct.Pin = SDA_I2C_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
    LL_GPIO_Init(SDA_I2C_GPIO_PORT, &GPIO_InitStruct);

    /* Reset I2C peripheral */
    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I2C1);
    LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C1);

    /* Initialize I2C1 peripheral */
    LL_I2C_InitTypeDef I2C_InitStruct;
    I2C_InitStruct.ClockSpeed      = I2C_SPEEDCLOCK;
    I2C_InitStruct.DutyCycle       = LL_I2C_DUTYCYCLE_16_9;
    I2C_InitStruct.OwnAddress1     = I2C_ADDRESS;
    I2C_InitStruct.TypeAcknowledge = LL_I2C_NACK;
    LL_I2C_Init(I2C1, &I2C_InitStruct);
}

/**
  * @brief  I2C Send data
  * @param  DevAddress：Slave address
  * @param  pData：pData pointer to Send buffer
  * @param  Size：Size Amount of data to be sent
  * @retval None
  */
void APP_MasterTransmit(uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
    /* Clear pos bit */
    LL_I2C_DisableBitPOS(I2C1);
    
    /* Update the slave address, data to be sent, data size, and status to global variables */
    pBuffPtr    = pData;
    XferCount   = Size;
    Devaddress  = DevAddress;
    State       = I2C_STATE_BUSY_TX;
    
    /* Generate Start signal */
    LL_I2C_GenerateStartCondition(I2C1);
    while(LL_I2C_IsActiveFlag_SB(I2C1) != 1);
    
    /* Send slave address */
    LL_I2C_TransmitData8(I2C1, (Devaddress & (uint8_t)(~0x01)));
    while(LL_I2C_IsActiveFlag_ADDR(I2C1) != 1);
    LL_I2C_ClearFlag_ADDR(I2C1);
    
    /* Send data */
    while(XferCount > 0)
    {
        
        while(LL_I2C_IsActiveFlag_TXE(I2C1) != 1);
        LL_I2C_TransmitData8(I2C1, *pBuffPtr);
        pBuffPtr++;
        XferCount--;

        if ((LL_I2C_IsActiveFlag_BTF(I2C1) == 1) && (XferCount != 0U))
        {
        LL_I2C_TransmitData8(I2C1, *pBuffPtr);
        pBuffPtr++;
        XferCount--;
        }

        while(LL_I2C_IsActiveFlag_BTF(I2C1) != 1);
    }
    
    /* Generate STOP signal */
    LL_I2C_GenerateStopCondition(I2C1);

    State = I2C_STATE_READY;
}

/**
  * @brief  I2C receive data
  * @param  DevAddress：Slave address
  * @param  pData：pData pointer to receive buffer
  * @param  Size：Size Amount of data to be received
  * @retval None
  */
void APP_MasterReceive(uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
    /* clear pos bit */
    LL_I2C_DisableBitPOS(I2C1);

    /* Update the slave address, data to be received, data size, and status to global variables */
    pBuffPtr    = pData;
    XferCount   = Size;
    Devaddress  = DevAddress;
    State       = I2C_STATE_BUSY_RX;

    /* Prepare the generation of a ACKnowledge condition */
    LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);

    /* Generate Start signal */
    LL_I2C_GenerateStartCondition(I2C1);
    while(LL_I2C_IsActiveFlag_SB(I2C1) != 1);

    /* Send slave address */
    LL_I2C_TransmitData8(I2C1, (Devaddress | 0x1));
    while(LL_I2C_IsActiveFlag_ADDR(I2C1) != 1);

    /* Receive data */
    if (XferCount == 0U)
    {
        LL_I2C_ClearFlag_ADDR(I2C1);
        LL_I2C_GenerateStopCondition(I2C1);
    }
    else if(XferCount == 1U)
    {
        LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);

        __disable_irq();
        LL_I2C_ClearFlag_ADDR(I2C1);
        LL_I2C_GenerateStopCondition(I2C1);
        __enable_irq();
    }
    else if(XferCount == 2U)
    {
        LL_I2C_EnableBitPOS(I2C1);

        __disable_irq();
        LL_I2C_ClearFlag_ADDR(I2C1);
        LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
        __enable_irq();
    }
    else
    {
        LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
        LL_I2C_ClearFlag_ADDR(I2C1);
    }

    while (XferCount > 0U)
    {
        if (XferCount <= 3U)
        {
        if (XferCount == 1U)
        {
            while(LL_I2C_IsActiveFlag_RXNE(I2C1) != 1);
            *pBuffPtr = LL_I2C_ReceiveData8(I2C1);
            pBuffPtr++;
            XferCount--;
        }
        else if (XferCount == 2U)
        {
            while(LL_I2C_IsActiveFlag_BTF(I2C1) != 1);

            __disable_irq();
            LL_I2C_GenerateStopCondition(I2C1);
            *pBuffPtr = LL_I2C_ReceiveData8(I2C1);
            pBuffPtr++;
            XferCount--;
            __enable_irq();

            *pBuffPtr = LL_I2C_ReceiveData8(I2C1);
            pBuffPtr++;
            XferCount--;
        }
        else
        {
            while(LL_I2C_IsActiveFlag_BTF(I2C1) != 1);

            LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);

            __disable_irq();
            *pBuffPtr = LL_I2C_ReceiveData8(I2C1);
            pBuffPtr++;
            XferCount--;
            while(LL_I2C_IsActiveFlag_BTF(I2C1) != 1);
            LL_I2C_GenerateStopCondition(I2C1);
            *pBuffPtr = LL_I2C_ReceiveData8(I2C1);
            pBuffPtr++;
            XferCount--;
            __enable_irq();

            *pBuffPtr = LL_I2C_ReceiveData8(I2C1);
            pBuffPtr++;
            XferCount--;
        }
        }
        else
        {
        while(LL_I2C_IsActiveFlag_RXNE(I2C1) != 1);

        *pBuffPtr = LL_I2C_ReceiveData8(I2C1);
        pBuffPtr++;
        XferCount--;

        if (LL_I2C_IsActiveFlag_BTF(I2C1) == 1)
        {
            *pBuffPtr = LL_I2C_ReceiveData8(I2C1);
            pBuffPtr++;
            XferCount--;
        }
        }
    }

    State = I2C_STATE_READY;
}

uint32_t get_i2c_state(void)
{
    return State;
}

