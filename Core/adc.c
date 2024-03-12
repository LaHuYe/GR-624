#include "adc.h"

CircularQueue bat_adc_queue;
uint32_t channels[] = {ADC_BAT_CH};
static uint32_t adc_gather_value[sizeof(channels) / sizeof(channels[0])];

void adc_gpio_init(void)
{
    ADC_BAT_GPIO_CLK_ENABLE();
    LL_GPIO_SetPinPull(ADC_BAT_GPIO_PORT,ADC_BAT_PIN,LL_GPIO_PULL_NO);  // 关闭内部上下拉
    LL_GPIO_SetPinMode(ADC_BAT_GPIO_PORT, ADC_BAT_PIN, LL_GPIO_MODE_ANALOG);
}

void adc_queue_init(void)
{
    initQueue(&bat_adc_queue);
}

void adc_Init(void)
{
    uint32_t all_channels = 0;

    adc_gpio_init();
    adc_queue_init();

    LL_ADC_InitTypeDef ADC_Init;
    LL_ADC_REG_InitTypeDef LL_ADC_REG_InitType;

    /* Enable ADC clock */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);

    /* Initialize partical features of ADC instance */
    ADC_Init.Clock=LL_ADC_CLOCK_SYNC_PCLK_DIV8;
    ADC_Init.DataAlignment=LL_ADC_DATA_ALIGN_RIGHT;
    ADC_Init.LowPowerMode=LL_ADC_LP_AUTOWAIT;
    ADC_Init.Resolution=LL_ADC_RESOLUTION_12B;
    LL_ADC_Init(ADC1,&ADC_Init);
    /* Sampling time 239.5 ADC clock cycles */
    LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_239CYCLES_5);

    /* Initialize partical features of ADC instance */
    LL_ADC_REG_InitType.ContinuousMode=LL_ADC_REG_CONV_CONTINUOUS;
    LL_ADC_REG_InitType.Overrun=LL_ADC_REG_OVR_DATA_OVERWRITTEN;
    LL_ADC_REG_InitType.SequencerDiscont=LL_ADC_REG_SEQ_DISCONT_DISABLE;
    LL_ADC_REG_InitType.TriggerSource=LL_ADC_REG_TRIG_SOFTWARE;
    LL_ADC_REG_Init(ADC1,&LL_ADC_REG_InitType);

    /* Set internal channel that connected to VrefInt as conversion channel */
    for (uint8_t i = 0; i < sizeof(channels) / sizeof(channels[0]); i++)
    {
        all_channels |= channels[i];
    }
    LL_ADC_REG_SetSequencerChannels(ADC1, all_channels);
    if (LL_ADC_IsEnabled(ADC1) == 0)
    {
        /* Enable ADC calibration */
        LL_ADC_StartCalibration(ADC1);
        while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
        {
        }
        /* The delay between the end of ADC calibration and ADC enablement is at least 4 ADC clocks */
        LL_mDelay(1);
    }

    /* Enable ADC */
    LL_ADC_Enable(ADC1);

    /* The delay between ADC enablement and ADC stabilization is at least 8 ADC clocks */
    LL_mDelay(1);

    /* Start ADC conversion (if it is software triggered then start conversion directly) */
    LL_ADC_REG_StartConversion(ADC1);
}
//1s采集一下adc,采集10次
void adc_gather_handle(void)
{
    static uint32_t adc_gather_interval;
    if (LL_GetTickDiff(adc_gather_interval) >= 100)   
    {
        for (size_t i = 0; i < sizeof(channels) / sizeof(channels[0]); i++)
        {
            // 等待转换完成
            while (LL_ADC_IsActiveFlag_EOC(ADC1) == 0);
            
            // 读取转换数据
            adc_gather_value[i] = LL_ADC_REG_ReadConversionData12(ADC1);
            if (i == 0)
            {
                enqueue(&bat_adc_queue, adc_gather_value[i]);
            }       
            adcPrintf(LOG_DEBUG,"adc[%d]:%d\r\n", i, adc_gather_value[i]);
        }
        adc_gather_interval = LL_GetTick();
    }
}

uint32_t get_bat_adc_value(void)
{ 
    // return movingAverage(&bat_adc_queue);
    return adc_gather_value[0];
}
