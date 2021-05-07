#ifndef __COMMON_H__
#define __COMMON_H__

#include "msp430.h"
#include "stdint.h"

void DelayMs(int ms);

void TimerA0_32768Init(void);

extern volatile int g_SYSTICK;
extern volatile int g_SYSSEC;
#endif
