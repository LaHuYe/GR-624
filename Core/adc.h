#ifndef __ADC_H
#define __ADC_H

#include "main.h"
#include "common.h"

#define ADC_POWER_VOL 5000  //ADC基准电压

void adc_Init(void);
void adc_gather_handle(void);
uint32_t get_pump_adc_value(void);

#endif
