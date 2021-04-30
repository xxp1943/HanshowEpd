/*
 * softuart.h
 *
 *  Created on: 2021Äê4ÔÂ21ÈÕ
 *      Author: LaoGao
 */

#ifndef SOFTUART_H_
#define SOFTUART_H_
#include "stdint.h"


#define SWUART_TXD        BIT0
#define SWUART_RXD        BIT2       //P2.2

#define SWUART_BAUD_RATE        (9600UL)
#define SWUART_SMCLK_FREQ       (16000000UL)


//typedef uint8_t (*t_RxCallBack)(void *pdat, uint8_t rxdata);// return 0 -- continue to receive; 1 -- disable to receive
// sta: 0: Tx complete; 1: Rx Ready;
typedef uint8_t (*t_SWUART_CALLBACK)(void *pdat, uint8_t sta, uint8_t rxdata);




void SWUART_Init(void);
uint8_t SWUART_Send(uint8_t const * p, uint16_t size);
void SWUART_SetCallBack(t_SWUART_CALLBACK pfunc, void * pdat);


#endif /* SOFTUART_H_ */
