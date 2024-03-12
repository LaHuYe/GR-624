#ifndef __TM1652_H
#define __TM1652_H
#include "main.h"

void tm1652_Init(void);
void tm1652_seg_show(uint8_t tens, uint8_t ones, uint8_t decimal);

#endif
