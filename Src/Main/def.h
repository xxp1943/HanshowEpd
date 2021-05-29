#ifndef __DEF_H__
#define __DEF_H__
#include "stdint.h"
#define LPM3P_RTC (1)
#define LPM3P_DISP (2)
#define LPM3P_SHELL (4)
#define LPM3P_UART  (8)

extern uint8_t LPM3Prohibit;

// P1
#define RF_RFIO02       BIT0
#define RF_RFIO01       BIT1
#define RF_DA           BIT2
#define RF_CS           BIT3
#define RF_CK           BIT4
#define FLASH_CK        BIT5
#define FLASH_SO        BIT6
#define FLASH_SI        BIT7

// P2
#define BP_CS           BIT0
#define SWUART_TXD      BP_CS

#define FP_csRF         BIT1
#define RF_POW          FP_csRF

#define BP_CLK          BIT2
#define SWUART_RXD      BP_CLK

#define EPD_CK          BIT3
#define EPD_DA          BIT4
#define EPD_BUSY        BIT5
//#define EPD_POW         BIT6
//#define RF_POW          BIT7


//P3
#define FLASH_CS        BIT0
#define EPD_BS          BIT1
#define FP_csFLASH      BIT2
#define EPD_POW         FP_csFLASH

#define BP_DA           BIT3
#define EPD_CS          BIT4
#define EPD_DC          BIT5
#define EPD_RST         BIT6
#define EPD_NC          BIT7

#endif 
