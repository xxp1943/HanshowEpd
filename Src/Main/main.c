#include <msp430.h> 
#include "softuart.h"
#include "def.h"
#include "epd.h"
#include "softspi3w.h"
#include "common.h"
#include "font.h"
#include "RTC_Calendar.h"
#include "flash.h"
#include "shell.h"


typedef struct 
{
    uint8_t sta;
    uint8_t mode;
}t_clock_disp_update;

void InitClk(void);
void InitGpio(void);
uint8_t uart_callback(void *d, uint8_t sta, uint8_t rx);


static uint8_t rxbuf[10];
static uint8_t rxidx=0;

static const uint8_t c_Test[]= {0xFF};


uint8_t LPM3Prohibit = 0;
t_EPD_InitUpdate m_EPD_InitUpdate = {0xFF};
t_clock_disp_update m_ClockDispUpdate = {0xFF};

void clock_disp(uint8_t mode);
void clock_disp_start(t_clock_disp_update *d, uint8_t mode);
void clock_disp_update(t_clock_disp_update *d);
void LPM3_Process(void);

void printtest(uint8_t x, uint8_t y, uint8_t const *str)
{
    uint8_t const *p = str;
    uint8_t ytmp = y;
    while(*p != '\0')
    {
        EPD_DrawPic(x,ytmp,8,5, nAsciiDot5x8[*p-' ']);
        ytmp += 6;
        p++;
    }
    EPD_Disp(EPD_PART);
    EPD_WaitBusy();
    p = str;
    ytmp = y;
    while(*p != '\0')
    {
        EPD_DrawPic(x,ytmp,8,5, nAsciiDot5x8[*p-' ']);
        ytmp += 6;
        p++;
    }
}

/**
 * main.c
 */
int main(void)
{
    int seconds, minutes;
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	InitClk();
	InitGpio();
    
	__enable_interrupt();

	//SWUART_Init();
	//SWUART_SetCallBack(uart_callback, 0);
	
	//SWUART_Send("Hello World!", sizeof("Hello World!")-1);

    shell_init();
    
    //Flash Init
    FLASH_Init();


    // Init EDP power and SPI
    EPD_POW_ON();
    DelayMs(200);
    EPD_CS_HIGH();
    SWSPI3W_IOInit();
    DelayMs(10);

    // Init RTC
    setTime(0x11, 0x00, 0x00, 1);
    setDate(2021, 5, 6);
    TimerA0_32768Init();

    // Start EDP 
    clock_disp_start(&m_ClockDispUpdate, EPD_FULL);

    seconds = g_SYSSEC;
    minutes = TI_minute;
	for(;;)
	{
        EPD_BusyUpdate();

        if ((g_SYSSEC - seconds) >= 1)
        {
            seconds ++;
            incrementSeconds();
            if (TI_minute != minutes)
            {
                minutes = TI_minute;
                if(minutes)
                {
                    //clock_disp(EPD_PART);
                    clock_disp_start(&m_ClockDispUpdate, EPD_PART);
                }
                else
                {
                    //clock_disp(EPD_FULL);
                    clock_disp_start(&m_ClockDispUpdate, EPD_FULL);
                }
                    
            }
        }

	    //if (rxidx >= 10)
	    //{
	    //    rxidx = 0;
	   //     SWUART_Send(rxbuf, 10);
	   // }
        shell_process();

        EPD_InitUpdate(&m_EPD_InitUpdate);
        clock_disp_update(&m_ClockDispUpdate);

        

        LPM3_Process();
	}
}

void InitClk(void)
{
  BCSCTL1 = CALBC1_16MHZ;                    // Set range
  DCOCTL = CALDCO_16MHZ;

  BCSCTL3 = LFXT1S_0 | XCAP_3;                   /* Mode 2 for LFXT1 : 32768 crystal ,12.5pF*/
  while (BCSCTL3 & LFXT1OF);
  IFG1 &= ~(OFIFG);
}

void InitGpio(void)
{
    P1OUT = 0x00;               //
    P1SEL = FLASH_CK | FLASH_SI | FLASH_SO;               // 
    P1SEL2 = FLASH_CK | FLASH_SI | FLASH_SO;
    P1DIR = 0xFF;               // Set all pins to output
    

    P2OUT = RF_POW | SWUART_RXD;   // RF_POW off, SWUART pull up resistor
    P2SEL = SWUART_TXD | SWUART_RXD | BIT6 | BIT7;    //Timer function for TXD/RXD pins
    P2DIR = 0xFF & ~SWUART_RXD & ~EPD_BUSY & ~BIT6;
    P2REN = SWUART_RXD;

    P2IES |= SWUART_RXD;

    P3OUT = EPD_POW | FLASH_CS;               // EPD_POW off
    P3SEL = 0x00;               // 
    P3DIR = 0xFF;               // Set all pins to output
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


static void draw_hex(uint8_t x, uint8_t y, uint8_t xsize, uint8_t ysize, uint8_t interval, uint8_t count, uint8_t const *pNumBase, uint16_t Num)
{
    uint8_t signum;
    uint16_t datlen;
    datlen = ((xsize+7)>>3) * ysize;
    while(count--)
    {
        signum = (Num >> (count<<2)) & 0xF;
        EPD_DrawPic(x, y, xsize, ysize, pNumBase + datlen * signum);
        y += ysize + interval;
    }
}

void clock_disp(uint8_t mode)
{
    uint8_t mon;
    draw_hex(56, 24, 56, 42, 4, 2, Font42x56[0], get24Hour());
    EPD_DrawPic(56, 116, 56, 10, Font10x56[0]);
    draw_hex(56, 130, 56, 42, 4, 2, Font42x56[0], TI_minute);

    draw_hex(24, 24, 16, 8, 0, 4, Font8x16[0], TI_year);
    EPD_DrawPic(24, 56, 16, 16, Font16x16[Font16x16_YEAR]);

    draw_hex(24, 72, 16, 8, 0, 2, Font8x16[0], __bcd_add_short(TI_month, 1));
    EPD_DrawPic(24, 88, 16, 16, Font16x16[Font16x16_MONTH]);

    draw_hex(24, 104, 16, 8, 0, 2, Font8x16[0], TI_day);
    EPD_DrawPic(24, 120, 16, 16, Font16x16[Font16x16_DAY]);

    EPD_DrawPic(24, 174, 16, 16, Font16x16[Font16x16_WEEK]);
    EPD_DrawPic(24, 174+16, 16, 16, Font16x16[Font16x16_WEEK+1]);
    draw_hex(24, 174+32, 16, 16, 0, 1, Font16x16[0], TI_dayOfWeek);

    EPD_Disp(mode);
}


void clock_disp_start(t_clock_disp_update *d, uint8_t mode)
{
    if (d->sta != 0xFF)
    {
        return;
    }
    d->sta = 0;
    d->mode = mode;
    LPM3Prohibit |= LPM3P_DISP;
}
void clock_disp_update(t_clock_disp_update *d)
{
    switch (d->sta)
    {
        case 0:
            EPD_InitStart(&m_EPD_InitUpdate);
            d->sta = 1;
        case 1:
            if (m_EPD_InitUpdate.sta != 0xFF) return;

            EPD_Clear(0);

            draw_hex(56, 24, 56, 42, 4, 2, Font42x56[0], get24Hour());
            EPD_DrawPic(56, 116, 56, 10, Font10x56[0]);
            draw_hex(56, 130, 56, 42, 4, 2, Font42x56[0], TI_minute);

            draw_hex(24, 24, 16, 8, 0, 4, Font8x16[0], TI_year);
            EPD_DrawPic(24, 56, 16, 16, Font16x16[Font16x16_YEAR]);

            draw_hex(24, 72, 16, 8, 0, 2, Font8x16[0], __bcd_add_short(TI_month, 1));
            EPD_DrawPic(24, 88, 16, 16, Font16x16[Font16x16_MONTH]);

            draw_hex(24, 104, 16, 8, 0, 2, Font8x16[0], TI_day);
            EPD_DrawPic(24, 120, 16, 16, Font16x16[Font16x16_DAY]);

            EPD_DrawPic(24, 174, 16, 16, Font16x16[Font16x16_WEEK]);
            EPD_DrawPic(24, 174+16, 16, 16, Font16x16[Font16x16_WEEK+1]);
            draw_hex(24, 174+32, 16, 16, 0, 1, Font16x16[0], TI_dayOfWeek);

            EPD_Disp(d->mode);

            d->sta = 2;
        case 2:
            if (EPD_BUSY_GET()) return;
            d->sta = 0xFF;
            //EPD_PowerOff();
            EPD_SetDeepSleep(EPD_DEEPSLEEP);
            LPM3Prohibit &= ~LPM3P_DISP;
        default:
            return;
    }
}

/*void update_time_full(void)
{
    clock_disp_start(&m_ClockDispUpdate, EPD_FULL);
}*/

void LPM3_Process(void)
{
    if(LPM3Prohibit)
        return;
    //Enable uart rx pin interrupt
    P2SEL &= ~SWUART_RXD;
    P2IFG &= ~SWUART_RXD;
    P2IE |= SWUART_RXD;

    LPM3;

    //uart rx disable pin interrut, enablr timer interrupt
    
}

#pragma vector = PORT2_VECTOR
__interrupt void port2_isr(void)
{
    if (P2IFG & SWUART_RXD)
    {
        P2IE &= ~SWUART_RXD;
        P2SEL |= SWUART_RXD;
        //keep the flag. handle it in shell module

        LPM3_EXIT;
    }
}
