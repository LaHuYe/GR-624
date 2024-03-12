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
    APP_UsartTransmit((uint8_t *)dataCmd, sizeof(dataCmd));
    LL_mDelay(1);
    /* Sending data through POLLING */
    APP_UsartTransmit((uint8_t *)showCmd, sizeof(showCmd));
    LL_mDelay(1);
}

void tm1652_seg_show(uint8_t tens, uint8_t ones, uint8_t decimal)
{
    dataCmd[1] = tens;
    dataCmd[2] = (ones | 0x80);
    dataCmd[3] = decimal;
    tm1652_data_write();
}
