#ifndef __EPD_H__
#define __EPD_H__
#include "msp430.h"
#include "stdint.h"

#define EPD_CS_LOW()                P3OUT &= ~BIT4
#define EPD_CS_HIGH()                P3OUT |= BIT4
#define EPD_DC_LOW()                P3OUT &= ~BIT5
#define EPD_DC_HIGH()                P3OUT |= BIT5
#define EPD_RST_LOW()                P3OUT &= ~BIT6
#define EPD_RST_HIGH()                P3OUT |= BIT6
#define EPD_BUSY_GET()                (P2IN & BIT5)

#define EPD_POW_ON()                    P3OUT &= ~BIT2
#define EPD_POW_OFF()                   P3OUT |= BIT2


#define EPD_WIDTH       122
#define EPD_HEIGHT      250

#define EPD_FULL			0
#define EPD_PART			1

#define EPD_DEEPSLEEP           1
#define EPD_NORMAL              0

void EPD_Init(void);

uint8_t EPD_DrawPic(uint8_t x, uint8_t y, uint8_t xsize, uint8_t ysize, uint8_t const *dat);
uint8_t EPD_Fill(uint8_t x, uint8_t y, uint8_t xsize, uint8_t ysize, uint8_t pattern);
uint8_t EPD_Clear(uint8_t pattern);
uint8_t EPD_Disp(uint8_t mode);

void EPD_WaitBusy(void);
void EPD_PowerOff(void);
void EPD_BusyUpdate(void);
void EPD_SetCallBack(void (*cb)(void));

typedef struct 
{
    uint8_t sta;
    int tick;
}t_EPD_InitUpdate;
void EPD_InitStart(t_EPD_InitUpdate *d);
void EPD_InitUpdate(t_EPD_InitUpdate *d);


void EPD_SetDeepSleep(uint8_t mode);
#endif
