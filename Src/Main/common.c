#include "common.h"

volatile int g_SYSTICK = 0; //ms
volatile int g_SYSSEC   = 0; //s


void DelayMs(int ms)
{
    int timer = g_SYSTICK;
    while((g_SYSTICK - timer) < ms);
}




void SysTickUpdate(void)
{
    g_SYSTICK++;
}


void TimerA0_32768Init(void)
{
    TA0CCR0 = 32768;
    TA0CCTL0 = CCIE;

    TA0CTL = TASSEL_1 + MC_2;                // ACLK, start in continuous mode
}


//------------------------------------------------------------------------------
// Timer_A UART - Transmit Interrupt Handler
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) Timer0_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    //__enable_interrupt();
    TA0CCR0 += 32768;
    g_SYSSEC++;
    LPM3_EXIT;
    //__disable_interrupt();
}
//------------------------------------------------------------------------------
// Timer_A UART - Receive Interrupt Handler
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) Timer0_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{

    switch (__even_in_range(TA0IV, TA0IV_TAIFG)) { // Use calculated branching
        case TA0IV_TACCR1:                        // TACCR1 CCIFG - UART RX

            break;
        /**** SysTIck ****/
        case TA0IV_TACCR2:

            break;
    }
}
