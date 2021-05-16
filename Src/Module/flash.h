#ifndef __FLASH_H__
#define __FLASH_H__
#include "msp430.h"
#include "def.h"

#define FLASH_CS_LOW()  P3OUT &= ~FLASH_CS
#define FLASH_CS_HIGH()  P3OUT |= FLASH_CS



void FLASH_Init(void);


#endif
