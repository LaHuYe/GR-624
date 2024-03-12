#include "seg.h"

// 0,1,2,3,4,5,6,7,8,9
uint8_t seg_digit[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

void seg_show_pressure(float pressure_value)
{
    uint8_t tens = ((uint32_t)pressure_value) / 10;         // 得出十位数
    uint8_t ones = ((uint32_t)pressure_value) % 10;         // 得出个位数
    uint8_t decimal = (uint32_t)(pressure_value * 10) % 10; // 得出小数点后一位数

    if (pressure_value <= 0)
    {
        tens = 0;
        ones = 0;
        ones = 0;
    }
    // printf("tens:%d ones:%d decimal:%d\r\n",tens,ones,decimal);
    tm1652_seg_show(seg_digit[tens], seg_digit[ones], seg_digit[decimal]);
}
