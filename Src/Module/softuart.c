/*
 * softuart.c
 *
 *  Created on: 2021Äê4ÔÂ21ÈÕ
 *      Author: LaoGao
 */
#include "msp430.h"
#include "softuart.h"

#define SWUART_TBIT             (SWUART_SMCLK_FREQ / SWUART_BAUD_RATE)
#define SWUART_TBIT_DIV2        (SWUART_SMCLK_FREQ / SWUART_BAUD_RATE / 2)

typedef struct
{
    t_SWUART_CALLBACK pfunc;
    void * pdat;

    uint8_t sta;        //bit0: Enable Rx

    uint8_t const *tx_buf;
    uint16_t tx_size;
    uint16_t tx_shifter;

    uint8_t tx_bit;
    uint8_t rx_bit;

    uint8_t rx_buf;

}t_SWUART_HANDLE;


t_SWUART_HANDLE m_SWUART;


void SWUART_Init(void)
{
    m_SWUART.pdat = 0;
    m_SWUART.pfunc = 0;
    m_SWUART.rx_bit = 8;
    m_SWUART.rx_buf = 0;
    m_SWUART.sta = 0;
    m_SWUART.tx_bit = 10;
    m_SWUART.tx_buf = 0;
    m_SWUART.tx_shifter = 0;
    m_SWUART.tx_size = 0;

    //Init TA1
    TA1CCTL0 = OUT;                          // Set TXD Idle as Mark = '1'
    TA1CCTL1 = SCS + CM1 + CAP + CCIE + CCIS0;       // Sync, Neg Edge, Capture, Int, CCI1B
    TA1CTL = TASSEL_2 + MC_2;                // SMCLK, start in continuous mode

}

uint8_t SWUART_Send(uint8_t const * p, uint16_t size)
{
    if (m_SWUART.tx_buf)
    {
        return 1;
    }

    m_SWUART.tx_buf = p+1;
    m_SWUART.tx_size = size-1;
    m_SWUART.tx_bit = 10;

    TA1CCR0 = TAR;                           // Current state of TA counter
    TA1CCR0 += SWUART_TBIT;                    // One bit time till first bit
    TA1CCTL0 = OUTMOD0 + CCIE;               // Set TXD on EQU0, Int
    m_SWUART.tx_shifter = *p;                // Load global variable
    m_SWUART.tx_shifter |= 0x100;                        // Add mark stop bit to TXData
    m_SWUART.tx_shifter <<= 1;                           // Add space start bit
    return 0;
}

void SWUART_SetCallBack(t_SWUART_CALLBACK pfunc, void * pdat)
{
    __disable_interrupt();
    m_SWUART.pfunc = pfunc;
    m_SWUART.pdat = pdat;
    __enable_interrupt();
}

//------------------------------------------------------------------------------
// Timer_A UART - Transmit Interrupt Handler
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) Timer_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    //static unsigned char txBitCnt = 10;
    if (m_SWUART.tx_buf)
    {
        TA1CCR0 += SWUART_TBIT;                    // Add Offset to CCRx
        if (m_SWUART.tx_bit == 0) {                    // All bits TXed?
            if (m_SWUART.tx_size-- == 0)
            {
                TA1CCTL0 &= ~CCIE;                   // All bits TXed, disable interrupt
                m_SWUART.tx_buf = 0;
                m_SWUART.tx_size = 0;
                if (m_SWUART.pfunc)
                {
                    m_SWUART.pfunc(m_SWUART.pdat, 0, 0);
                }
                return;
            }
            else
            {
                m_SWUART.tx_shifter = *m_SWUART.tx_buf;
                m_SWUART.tx_shifter |= 0x100;
                m_SWUART.tx_shifter <<= 0x1;
                m_SWUART.tx_buf ++;
            }
            m_SWUART.tx_bit = 10;                      // Re-load bit counter
        }

        if (m_SWUART.tx_shifter & 0x01) {
          TA1CCTL0 &= ~OUTMOD2;              // TX Mark '1'
        }
        else {
          TA1CCTL0 |= OUTMOD2;               // TX Space '0'
        }
        m_SWUART.tx_shifter >>= 1;
        m_SWUART.tx_bit--;
    }
    else
    {
        TA1CCTL0 &= ~CCIE;                   // All bits TXed, disable interrupt
    }

}
//------------------------------------------------------------------------------
// Timer_A UART - Receive Interrupt Handler
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_A1_VECTOR
__interrupt void Timer_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A1_VECTOR))) Timer_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{

    switch (__even_in_range(TA1IV, TA1IV_TAIFG)) { // Use calculated branching
        case TA1IV_TACCR1:                        // TACCR1 CCIFG - UART RX
            TA1CCR1 += SWUART_TBIT;                 // Add Offset to CCRx
            if (TA1CCTL1 & CAP) {                 // Capture mode = start bit edge
                TA1CCTL1 &= ~CAP;                 // Switch capture to compare mode
                TA1CCR1 += SWUART_TBIT_DIV2;       // Point CCRx to middle of D0
            }
            else {
                m_SWUART.rx_buf >>= 1;
                if (TA1CCTL1 & SCCI) {            // Get bit waiting in receive latch
                    m_SWUART.rx_buf |= 0x80;
                }
                m_SWUART.rx_bit--;
                if (m_SWUART.rx_bit == 0) {             // All bits RXed?
                    m_SWUART.rx_bit = 8;                // Re-load bit counter
                    TA1CCTL1 |= CAP;              // Switch compare to capture mode
                    if (m_SWUART.pfunc)
                    {
                        m_SWUART.pfunc(m_SWUART.pdat, 1, m_SWUART.rx_buf);
                    }
                }
            }
            break;
    }
}
