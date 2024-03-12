#include "gzp6859d.h"
#include "I2c.h"
#include "common.h"

// static struct ma_t *pressure_ma;


// i2c读gzp6859d寄存器数据
static uint8_t gzp6859d_read_register(uint8_t reg_addr)
{
    uint8_t reg_data = 0;
    // 发送寄存器地址
    if (HAL_I2C_Master_Transmit(&I2cHandle, GZP6859D_ADDR << 1, (uint8_t *)&reg_addr, sizeof(reg_addr), 1000) != HAL_OK)
    {
        return STATUS_TIMEOUT;
    }
    //等待发送完成
    while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY);
    LL_mDelay(1);   //加个小延时，不然会卡住
    // 读取寄存器数据
    if (HAL_I2C_Master_Receive(&I2cHandle, GZP6859D_ADDR << 1, (uint8_t *)&reg_data, sizeof(reg_data), 1000) != HAL_OK)
    {
        return STATUS_TIMEOUT;
    }
    while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY);
    //等待接收完成
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
        if (HAL_I2C_Master_Transmit(&I2cHandle, GZP6859D_ADDR << 1, (uint8_t *)buf, sizeof(buf), 5000) != HAL_OK)
        {
            logAssert();
            return STATUS_TIMEOUT;
        }
        //等待发送完成
        while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY);
        LL_mDelay(1);
        if (HAL_I2C_Master_Receive(&I2cHandle, GZP6859D_ADDR << 1, (uint8_t *)&result, sizeof(result), 1000) != HAL_OK)
        {
            logAssert();
            return STATUS_TIMEOUT;
        }
        while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY);
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
    // static uint8_t skit = 5;
    float pressure_raw = 0.0f;
    int32_t pressure_adc = 0;
    uint8_t result = 0,buf[2] = {PRESSURE_CMD, 0x09};;
    if (HAL_I2C_Master_Transmit(&I2cHandle, GZP6859D_ADDR << 1, (uint8_t *)buf, sizeof(buf), 5000) != HAL_OK)
    {
        return STATUS_TIMEOUT;
    }
    //等待发送完成
    while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY);
    LL_mDelay(1);   //加个小延时，不然会卡住
    if (HAL_I2C_Master_Receive(&I2cHandle, GZP6859D_ADDR << 1, (uint8_t *)&result, sizeof(result), 1000) != HAL_OK)
    {
        return STATUS_TIMEOUT;
    }
    while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY);
    pressure_adc = gzp6859d_read_register(PRESSURE_DATA_MSB) << 16;
    pressure_adc |= gzp6859d_read_register(PRESSURE_DATA_CSB) << 8;
    pressure_adc |= gzp6859d_read_register(PRESSURE_DATA_LSB);
    // //丢弃傳感器剛启动时前5个数据
    // if (skit > 0) {
    //     skit--;
    //     return -1;
    // }
    if (pressure_adc & (1<<23))
        pressure_adc = pressure_adc - 16777216;
    pressure_raw = (float)pressure_adc / GZP6859D_K;
    *pressure = pressure_raw;
    // printf("pressure_raw:%f \r\n",pressure_raw);
    // calculate_moving_average_range(pressure_ma, pressure_raw);   //会卡死，后续排查
    // printf("%s %d \r\n",__FUNCTION__,__LINE__);
    // *pressure = pressure_ma->range2_avg;
    // // 氣壓越來越大，吹氣
    // if (pressure_ma->range2_avg >= pressure_ma->range1_avg + PRESSURE_DIR_DELTA_LIMIT)
    //     return 1;
    // // 氣壓越來越小，抽氣
    // else if (pressure_ma->range1_avg >= pressure_ma->range2_avg + PRESSURE_DIR_DELTA_LIMIT)
    //     return 0;
    // // 无进出氣
    // else
    //     return -1;
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
    // for (size_t i = 0; i < 10; i++)
    // {
        GZP6859D_ReadSingleModePressureData(pressure_data);
    // }
}
