#ifndef __UCBSPI_H__
#define __UCBSPI_H__
#include "stdint.h"
#include "common.h"


void UCBSPI_Init(void);
uint8_t UCBSPI_Transfer(const uint8_t *txbuf, uint8_t *rxbuf, uint16_t size);
//void UCBSPI_SetCallback(void (*cb_func)(void *d), void * handle);

#endif
