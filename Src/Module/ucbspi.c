#include "ucbspi.h"



void UCBSPI_Init(void)
{
    UCB0CTL1 = UCSSEL_2 | UCSWRST;  //reset state
    UCB0CTL0 = UCCKPH | UCMSB | UCMST | UCMODE_0 | UCSYNC;  //8bit, mode 0

    UCB0BR0 = 1;    //prescal = 1, Fclk = SMCLK
    UCB0BR1 = 0;
    UCB0STAT = 0;   //clear stat;
    IE2 &= ~(UCB0TXIE | UCB0RXIE);
    IFG2 &= ~(UCB0RXIFG |UCB0TXIFG);

    UCB0CTL1 = UCSSEL_2;    //clear reset state
}

uint8_t UCBSPI_Transfer(const uint8_t *txbuf, uint8_t *rxbuf, uint16_t size)
{
    IFG2 &= ~UCB0RXIFG;
    while(size --)
    {
        if(txbuf)
        {
            UCB0TXBUF = *txbuf;
        }
        else
        {
            UCB0TXBUF = 0xFF;
        }
        while(!(IFG2 & UCB0RXIFG));
        if (rxbuf)
        {
            *rxbuf = UCB0RXBUF;
        }
        IFG2 &= ~UCB0RXIFG;
    }
    return 0;
}
