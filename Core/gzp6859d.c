#include "gzp6859d.h"
#include "I2c.h"
#include "common.h"



// i2c读gzp6859d寄存器数据
static uint8_t gzp6859d_read_register(uint8_t reg_addr)
{
    uint8_t reg_data = 0;

    APP_MasterTransmit(GZP6859D_ADDR << 1,(uint8_t *)&reg_addr,sizeof(reg_addr));
    while (get_i2c_state() != I2C_STATE_READY);
    LL_mDelay(5);
    APP_MasterReceive(GZP6859D_ADDR << 1,(uint8_t *)&reg_data,sizeof(reg_data));
    while (get_i2c_state() != I2C_STATE_READY);
    LL_mDelay(5);
    return reg_data;
}

// 组合模式读取数据
int8_t GZP6859D_ReadCombinedModeData(uint8_t dev_addr, uint8_t dev_data)
{
    float tempature = 0;
    uint8_t result = 0;
    int32_t pressure_adc = 0, temp_adc = 0;
    float pressure_raw = 0.0f;
    uint8_t buf[2] = {dev_addr, dev_data};
    while ((tempature <= 0) || (tempature >= 65))
    {
        APP_MasterTransmit(GZP6859D_ADDR << 1, (uint8_t *)buf, sizeof(buf));
        while (get_i2c_state() != I2C_STATE_READY);
        LL_mDelay(5);
        APP_MasterReceive(GZP6859D_ADDR << 1, (uint8_t *)&result, sizeof(result));
        while (get_i2c_state() != I2C_STATE_READY);

        
        temp_adc = gzp6859d_read_register(PRESSURE_TEMP_MSB) << 8;
        temp_adc |= gzp6859d_read_register(PRESSURE_TEMP_LSB);
        pressure_adc = gzp6859d_read_register(PRESSURE_DATA_MSB) << 16;
        pressure_adc |= gzp6859d_read_register(PRESSURE_DATA_CSB) << 8;
        pressure_adc |= gzp6859d_read_register(PRESSURE_DATA_LSB);

        if (temp_adc & (1<<15))
            temp_adc = temp_adc - 65536;
        tempature = (float)temp_adc / 256;
        pressurePrintf(LOG_NOTIC,"tempature:%f \r\n",tempature);
    }
    
    if (pressure_adc & (1<<23))
        pressure_adc = pressure_adc - 16777216;
    pressure_raw = (float)pressure_adc / GZP6859D_K;
    pressurePrintf(LOG_NOTIC,"pressure_raw:%f \r\n",pressure_raw);
    return STATUS_SUCCESS;
}

// 单次模式读取压力数据
User_StatusTypeDef GZP6859D_ReadSingleModePressureData(float *pressure)
{
    float pressure_raw = 0.0f;
    int32_t pressure_adc = 0;
    uint8_t result = 0,buf[2] = {PRESSURE_CMD, 0x09};

    APP_MasterTransmit(GZP6859D_ADDR << 1, (uint8_t *)buf, sizeof(buf));
    while (get_i2c_state() != I2C_STATE_READY);
    LL_mDelay(5);
    APP_MasterReceive(GZP6859D_ADDR << 1, (uint8_t *)&result, sizeof(result));
    LL_mDelay(5);
    while (get_i2c_state() != I2C_STATE_READY);

    pressure_adc = gzp6859d_read_register(PRESSURE_DATA_MSB) << 16;
    pressure_adc |= gzp6859d_read_register(PRESSURE_DATA_CSB) << 8;
    pressure_adc |= gzp6859d_read_register(PRESSURE_DATA_LSB);
    if (pressure_adc & (1<<23))
        pressure_adc = pressure_adc - 16777216;
    pressure_raw = (float)pressure_adc / GZP6859D_K;
    *pressure = pressure_raw;

    return STATUS_SUCCESS;
}

uint8_t gzp6859d_init(void)
{
    i2c_init();
    LL_mDelay(100);  // 等待初始化稳定
    // 进行组合模式读取数据
    GZP6859D_ReadCombinedModeData(PRESSURE_CMD,0x0A);
    return STATUS_SUCCESS;
}

void read_pressureData(float *pressure_data)
{
    GZP6859D_ReadSingleModePressureData(pressure_data);
}
