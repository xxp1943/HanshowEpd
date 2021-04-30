#include <msp430.h> 
#include "softuart.h"

void InitClk(void);
void InitGpio(void);
uint8_t uart_callback(void *d, uint8_t sta, uint8_t rx);

static uint8_t rxbuf[10];
static uint8_t rxidx=0;

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	InitClk();
	InitGpio();
	__enable_interrupt();

	SWUART_Init();
	SWUART_SetCallBack(uart_callback, 0);
	
	SWUART_Send("Hello World!", sizeof("Hello World!")-1);
	for(;;)
	{
	    if (rxidx >= 10)
	    {
	        rxidx = 0;
	        SWUART_Send(rxbuf, 10);
	    }
	}
}

void InitClk(void)
{
  BCSCTL1 = CALBC1_16MHZ;                    // Set range
  DCOCTL = CALDCO_16MHZ;

  BCSCTL3 |= LFXT1S_2;                   /* Mode 2 for LFXT1 : VLO */
  while (BCSCTL3 & LFXT1OF);
  IFG1 &= ~(OFIFG);
}

void InitGpio(void)
{
    P1OUT = 0x00;                           //
    P1SEL = 0x00;            // Timer function for TXD/RXD pins
    P1DIR = 0xFF;               // Set all pins but RXD to output
    P2OUT = 0x00;
    P2SEL = SWUART_TXD | SWUART_RXD;
    P2DIR = 0xFF & ~SWUART_RXD;
}

uint8_t uart_callback(void *d, uint8_t sta, uint8_t rx)
{
    if (sta == 1)// rx
    {
        if (rxidx < 10)
        {
            rxbuf[rxidx++] = rx;
        }
    }
    return 0;
}

