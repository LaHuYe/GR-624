#ifndef __IWDG_H
#define __IWDG_H

#include "main.h"

#define IWDG_Reload 1000

void iwdg_Init(void);
void iwdg_FeedDog(void);

#endif
