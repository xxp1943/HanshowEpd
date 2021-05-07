#ifndef __SOFTSPI3W_H__
#define __SOFTSPI3W_H__
#include "stdint.h"
#include "msp430.h"

#define SWSPI3W_CPHA  0  
#define SWSPI3W_CPOL  0 


#define SWSPI3W_DA_LOW()          P2OUT &= ~0x10
#define SWSPI3W_DA_HIGH()         P2OUT |= 0x10
#define SWSPI3W_DA_GET()          (P2IN & 0x10)
#define SWSPI3W_DA_SETIN()          P2DIR &= ~0x10;
#define SWSPI3W_DA_SETOUT()          P2DIR |= 0x10;
#define SWSPI3W_SCK_LOW()          P2OUT &= ~BIT3
#define SWSPI3W_SCK_HIGH()         P2OUT |= BIT3

void SWSPI3W_IOInit(void);
void SWSPI3W_IOLowPower(void);
//Single byte transfer
void SWSPI3W_TransmitByte(uint8_t txbyte);
uint8_t SWSPI3W_ReceiveByte(void);
//Multibyte transfer
void SWSPI3W_TransmitMultByte(uint8_t const *txbuf, uint16_t size);
void SWSPI3W_ReceiveMultByte(uint8_t *rxbuf, uint16_t size);

#endif
