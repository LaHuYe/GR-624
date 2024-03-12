#include "charge.h"
#include <string.h>
#include "bat.h"
#if 0
void bat_bypass_init(void)
{
    LL_IOP_GRP1_EnableClock (LL_IOP_GRP1_PERIPH_GPIOA);

    LL_GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = BALANCE1_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH; /* GPIO速度 */
    LL_GPIO_Init(BALANCE1_GPIO_PORT, &GPIO_InitStruct);
    // 充满检测IO初始化
    GPIO_InitStruct.Pin = BALANCE2_PIN;
    LL_GPIO_Init(BALANCE2_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BALANCE3_PIN;
    LL_GPIO_Init(BALANCE3_GPIO_PORT, &GPIO_InitStruct);
}

// 电池旁路开启函数
void bat_bypass_switch(uint8_t bat_site)
{
    switch (bat_site)
    {
    case 0:
        LL_GPIO_SetOutputPin(BALANCE1_GPIO_PORT, BALANCE1_PIN);
        LL_GPIO_ResetOutputPin(BALANCE2_GPIO_PORT, BALANCE2_PIN);
        LL_GPIO_ResetOutputPin(BALANCE3_GPIO_PORT, BALANCE3_PIN);
        break;
    case 1:
        LL_GPIO_ResetOutputPin(BALANCE1_GPIO_PORT, BALANCE1_PIN);
        LL_GPIO_SetOutputPin(BALANCE2_GPIO_PORT, BALANCE2_PIN);
        LL_GPIO_ResetOutputPin(BALANCE3_GPIO_PORT, BALANCE3_PIN);   
        break;
    case 2:
        LL_GPIO_ResetOutputPin(BALANCE1_GPIO_PORT, BALANCE1_PIN);
        LL_GPIO_ResetOutputPin(BALANCE2_GPIO_PORT, BALANCE2_PIN);  
        LL_GPIO_SetOutputPin(BALANCE3_GPIO_PORT, BALANCE3_PIN);
        break;
    default:
        LL_GPIO_ResetOutputPin(BALANCE1_GPIO_PORT, BALANCE1_PIN);
        LL_GPIO_ResetOutputPin(BALANCE2_GPIO_PORT, BALANCE2_PIN);  
        LL_GPIO_ResetOutputPin(BALANCE3_GPIO_PORT, BALANCE3_PIN);
        break;
    }
}

// 快速排序
void swap(uint16_t *a, uint16_t *b)
{
    uint16_t temp = *a;
    *a = *b;
    *b = temp;
}

void bubbleSort(uint16_t arr[], uint16_t size) {
    for (size_t i = 0; i < size - 1; i++) {
        for (size_t j = 0; j < size - i - 1; j++) {
            // 如果当前元素大于下一个元素，交换它们
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
            }
        }
    }
}


// 充电均衡函数
void balanceChargeBattery(uint8_t state)
{
    static uint16_t bat_one_adc[3];
    static uint16_t bat_sort_adc[3];
    static uint32_t last_time = 0;
    if (state)
    {
        if (LL_GetTickDiff(last_time) >= 60000)    //每次旁路必须60s以上
        {
            // 获取每节电池的adc值
            bat_one_adc[0] = get_bat1_vol();
            bat_one_adc[1] = get_bat2_vol() - bat_one_adc[0];
            bat_one_adc[2] = get_bat3_vol() - (bat_one_adc[0] + bat_one_adc[1]);

            batPrintf(LOG_DEBUG,"bat_one_adc: \r\n");
            for (size_t i = 0; i < 3; i++)
            {
                batPrintf(LOG_DEBUG,"%d ",bat_one_adc[i]);
            }
            Log("\r\n");
            
            memcpy(bat_sort_adc, bat_one_adc, sizeof(bat_one_adc));
            bubbleSort(bat_sort_adc, 3); // 对每节电池的adc进行排序

            batPrintf(LOG_DEBUG,"bat_sort_adc: \r\n");
            for (size_t i = 0; i < 3; i++)
            {
                batPrintf(LOG_DEBUG,"%d ",bat_sort_adc[i]);
            }
            Log("\r\n");

            // 因为已经排序好了，所以最大电压减最小电压大于0.05，则旁路最大电压的电池
            if ((bat_sort_adc[2] - 50) >= bat_sort_adc[0])
            {
                for (size_t i = 0; i < 3; i++)
                {
                    if (bat_sort_adc[2] == bat_one_adc[i])
                    { // 打开对应的电池旁路开关
                        batPrintf(LOG_DEBUG,"i:%d\r\n",i);
                        bat_bypass_switch(i); 
                        last_time = LL_GetTick();
                    }
                }
            }else
            {
                LL_GPIO_ResetOutputPin(BALANCE1_GPIO_PORT, BALANCE1_PIN);
                LL_GPIO_ResetOutputPin(BALANCE2_GPIO_PORT, BALANCE2_PIN);
                LL_GPIO_ResetOutputPin(BALANCE3_GPIO_PORT, BALANCE3_PIN);
            }
            //电池只要大于4.3V就一直打开旁路
            if (bat_one_adc[0] >= 4300)
                LL_GPIO_SetOutputPin(BALANCE1_GPIO_PORT, BALANCE1_PIN);
            if (bat_one_adc[1] >= 4300)
                LL_GPIO_SetOutputPin(BALANCE2_GPIO_PORT, BALANCE2_PIN);
            if (bat_one_adc[2] >= 4300)
                LL_GPIO_SetOutputPin(BALANCE3_GPIO_PORT, BALANCE3_PIN);
        }
    }else
    {
        LL_GPIO_ResetOutputPin(BALANCE1_GPIO_PORT, BALANCE1_PIN);
        LL_GPIO_ResetOutputPin(BALANCE2_GPIO_PORT, BALANCE2_PIN);
        LL_GPIO_ResetOutputPin(BALANCE3_GPIO_PORT, BALANCE3_PIN);
    }
}
#endif
