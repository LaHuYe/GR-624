#ifndef __GZP6859D_H
#define __GZP6859D_H

#include "main.h"

#define PRESSURE_TOTAL_SAMPLES  10  // 總樣本量
#define PRESSURE_STAGE1_SAMPLES 5
#define PRESSURE_STAGE3_SAMPLES 5
#define PRESSURE_DIR_DELTA_LIMIT 100  // 压力产生向上或向下趋势时的阈值

// 芯片参数
#define GZP6859D_K          128
#define GZP6859D_ADDR       0x6D
// 压力数据寄存器
#define PRESSURE_DATA_MSB   0x06
#define PRESSURE_DATA_CSB   0x07
#define PRESSURE_DATA_LSB   0x08
// 温度数据寄存器
#define PRESSURE_TEMP_MSB   0x09
#define PRESSURE_TEMP_LSB   0x0A
// 测量命令寄存器
#define PRESSURE_CMD        0x30
#define PRESSURE_SYS_CONFIG 0xA5
#define PRESSURE_P_CONFIG   0xA6


uint8_t gzp6859d_init(void);
User_StatusTypeDef GZP6859D_ReadSingleModePressureData(float *pressure);
void read_pressureData(float *pressure_data);
#endif
