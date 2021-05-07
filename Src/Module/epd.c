#include "epd.h"
#include "softspi3w.h"
#include "common.h"
/*
const unsigned char EPD_2IN13_lut_full_update[] = {
    0x22, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x11,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char EPD_2IN13_lut_partial_update[] = {
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};*/

const unsigned char EPD_2IN13_lut_update[][30] = {
    /*{
    0x22, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x11,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00
    },*/
    {
    0x66, 0x66, 0x26, 0x04, 0x55, 0xaa, 0x08, 0x91,
    0x11, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x19, 0x19, 0x0a, 0x0a, 0x5e, 0x1e, 0x1e, 0x0a,
    0x39, 0x14, 0x00, 0x00, 0x00, 0x02},
    /*{
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }*/
    {
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }
};

static uint8_t m_Current_Mode = 0; //0 -- Full, 1-- Partial
static uint8_t m_BUSY = 0;
static void (*m_EPD_CallBack)(void);
static uint32_t m_EPD_Buffer[4];


static inline void EPD_SendCmd(uint8_t cmd)
{
    EPD_DC_LOW();
    EPD_CS_LOW();
    SWSPI3W_TransmitByte(cmd);
    EPD_CS_HIGH();
}

static inline void EPD_SendSigData(uint8_t dat)
{
    EPD_DC_HIGH();
    EPD_CS_LOW();
    SWSPI3W_TransmitByte(dat);
    EPD_CS_HIGH();
}

static inline void EPD_SendMultData(uint8_t const *dat, uint16_t size)
{
    EPD_DC_HIGH();
    EPD_CS_LOW();
    SWSPI3W_TransmitMultByte(dat, size);
    EPD_CS_HIGH();
}

void EPD_WaitBusy(void)
{
    while (EPD_BUSY_GET())
    {
        DelayMs(10);
    }
}

static uint8_t EPD_Bits2BytesInc(uint8_t bits)
{
    return (bits+7)>>3;
}

static void EPD_SetWindows(int x_start, int y_start, int x_end, int y_end)
{
    EPD_SendCmd(0x44);
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    EPD_SendSigData((x_start >> 3) & 0xFF);
    EPD_SendSigData(EPD_Bits2BytesInc(x_end));
    EPD_SendCmd(0x45);
    EPD_SendSigData(y_start & 0xFF);
    EPD_SendSigData(y_end & 0xFF);

}

static void EPD_SetCursor(int x, int y)
{
    EPD_SendCmd(0x4E);
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    EPD_SendSigData((x >> 3) & 0xFF);
    EPD_SendCmd(0x4F);
    EPD_SendSigData(y & 0xFF);
}

void EPD_BusyUpdate(void)
{
    if(m_EPD_CallBack && EPD_BUSY_GET() == 0)
    {
        m_EPD_CallBack();
    }
}

void EPD_SetCallBack(void (*cb)(void))
{
    m_EPD_CallBack = cb;
}

#if 0
extern const unsigned char epdinit[]=
{
0x01,0xf9,0x00,0x3a,0x06,0x3b,0x0b,0x11,0x03,0x44,0x00,0x0f,0x45,
0x00,0xf9,0x2c,0xa0,0x3c,0x33,0x32,0x66,0x66,0x26,0x04,0x55,0xaa,
0x08,0x91,0x11,0x88,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x19,0x0a,
0x0a,0x5e,0x1e,0x1e,0x0a,0x39,0x14,0x00,0x00,0x00,0x02,0x21,0x83,
 
 };

void EPD_Init(void)
{  
        unsigned char i;
        EPD_POW_ON();
        SWSPI3W_IOInit();
        EPD_RST_LOW();
    DelayMs(1000);
    EPD_RST_HIGH();
    DelayMs(1000);

        //epd_send_cmd(0x12);
        EPD_SendCmd(0x12);
        //Delaylong(1);
        DelayMs(1000);
        for ( i = 0; i <= 0x33; i++ )
        {
                if ( (i == 0x0) || (i == 0x3) || (i == 0x5) || (i == 0x7) || 
                    (i == 0x9) || (i == 0xc) || (i == 0xf) || (i == 0x11) ||
                    (i == 0x13) ||  (i == 0x32)) 
                {
                  //epddclow;
                  EPD_DC_LOW();
                }
              //epdcslow;
              EPD_CS_LOW();
              //sendbyte(epdinit[i]);
              SWSPI3W_TransmitByte(epdinit[i]);
              EPD_CS_HIGH();
              //epddchigh;
              EPD_DC_HIGH();
              //asm("nop");
              //asm("nop");
              _NOP();
              _NOP();
         }
  
}
#endif

#if 1
void EPD_Init(void)
{
    m_EPD_CallBack = 0;
    EPD_POW_ON();
    DelayMs(10);
    EPD_CS_HIGH();
    SWSPI3W_IOInit();

    EPD_RST_LOW();
    DelayMs(10);
    EPD_RST_HIGH();
    DelayMs(10);

    EPD_WaitBusy();

    EPD_SendCmd(0x01); // DRIVER_OUTPUT_CONTROL
    EPD_SendSigData((EPD_HEIGHT - 1) & 0xFF);
    EPD_SendSigData(0x00);			// GD = 0; SM = 0; TB = 0;

    EPD_SendCmd(0x3A);	// 
    EPD_SendSigData(0x06);

    EPD_SendCmd(0x3B);	// 
    EPD_SendSigData(0x0B);

    EPD_SendCmd(0x11);	// 
    EPD_SendSigData(0x02 | 0x4);// x decrement, y increment, y direction

    EPD_SendCmd(0x44);
    EPD_SendSigData(0);
    EPD_SendSigData(0x0F);

    EPD_SendCmd(0x45);
    EPD_SendSigData(0);
    EPD_SendSigData(0xF9);

    EPD_SendCmd(0x2C);	// 
    EPD_SendSigData(0xA0);

    EPD_SendCmd(0x3C);	// 
    EPD_SendSigData(0x33);

    EPD_SendCmd(0X21);
    EPD_SendSigData(0x0D);

    //set the look-up table register
    //EPD_SendCmd(0x32);
    //EPD_SendMultData(EPD_2IN13_lut_update[EPD_FULL], 29);
    m_Current_Mode = 0xFF;
}
#endif

void EPD_InitStart(t_EPD_InitUpdate *d)
{
    d->sta = 0;
}
void EPD_InitUpdate(t_EPD_InitUpdate *d)
{
    switch (d->sta)
    {
        case 0:
            EPD_RST_LOW();
            d->tick = g_SYSTICK;
            d->sta++;
        case 1:
            if ((g_SYSTICK - d->tick) < 10) return;
            EPD_RST_HIGH();
            //EPD_SendCmd(0x12); //sw reset
            d->sta++;
        case 2:
            if (EPD_BUSY_GET()) return;

            EPD_SendCmd(0x01); // DRIVER_OUTPUT_CONTROL
            EPD_SendSigData((EPD_HEIGHT - 1) & 0xFF);
            EPD_SendSigData(0x00);			// GD = 0; SM = 0; TB = 0;

            EPD_SendCmd(0x3A);	// 
            EPD_SendSigData(0x06);

            EPD_SendCmd(0x3B);	// 
            EPD_SendSigData(0x0B);

            EPD_SendCmd(0x11);	// 
            EPD_SendSigData(0x02 | 0x4);// x decrement, y increment, y direction

            EPD_SendCmd(0x44);
            EPD_SendSigData(0);
            EPD_SendSigData(0x0F);

            EPD_SendCmd(0x45);
            EPD_SendSigData(0);
            EPD_SendSigData(0xF9);

            EPD_SendCmd(0x2C);	// 
            EPD_SendSigData(0xA0);

            EPD_SendCmd(0x3C);	// 
            EPD_SendSigData(0x33);

            EPD_SendCmd(0X21);
            EPD_SendSigData(0x0D);

            m_Current_Mode = 0xFF;
            d->sta = 0xFF;
        default:
            return;
    }
}

uint8_t EPD_DrawPic(uint8_t x, uint8_t y, uint8_t xsize, uint8_t ysize, uint8_t const *dat)
{
    uint16_t length;
    if (EPD_BUSY_GET())
    {
        return 1;
    }

    length = EPD_Bits2BytesInc(xsize) * ysize;

    EPD_SetWindows(x, y, x+xsize-1, y+ysize-1);
    EPD_SetCursor(x+xsize-1, y);

    EPD_SendCmd(0x24);
    EPD_SendMultData(dat, length);

    return 0;
}

/*
uint8_t EPD_DrawPicAutoRepeat(uint8_t x, uint8_t y, uint8_t xsize, uint8_t ysize, uint8_t const *dat)
{
    typedef struct 
    {
        uint8_t sta;
        uint8_t x;
        uint8_t y;
        uint8_t xsize;
        uint8_t ysize;
        uint8_t const *dat;
    }t_EPD_DrawPicAutoRepeat;
    t_EPD_DrawPicAutoRepeat * handle;
    handle = (t_EPD_DrawPicAutoRepeat*)m_EPD_Buffer;

    if (m_EPD_CallBack == EPD_DrawPicAutoRepeat)
    {
        
    }
    else if (m_EPD_CallBack == 0)
    {
        EPD_DrawPic(handle->x, handle->y, handle->xsize, handle->ysize, handle->dat);
    }

}*/

uint8_t EPD_Fill(uint8_t x, uint8_t y, uint8_t xsize, uint8_t ysize, uint8_t pattern)
{
    uint16_t length, i;
    if (EPD_BUSY_GET())
    {
        return 1;
    }

    length = EPD_Bits2BytesInc(xsize) * ysize;

    EPD_SetWindows(x, y, x+xsize-1, y+ysize-1);
    EPD_SetCursor(x+xsize-1, y);

    EPD_SendCmd(0x24);
    EPD_DC_HIGH();
    EPD_CS_LOW();
    for(i=0; i< length; i++)
    {
        //EPD_SendSigData(pattern);
        SWSPI3W_TransmitByte(pattern);
    }
    EPD_CS_HIGH();

    return 0;
}

uint8_t EPD_Clear(uint8_t pattern)
{
    uint16_t i;
    if (EPD_BUSY_GET())
    {
        return 1;
    }

    EPD_SetWindows(0, 0, EPD_WIDTH, EPD_HEIGHT);
    EPD_SetCursor(EPD_WIDTH, 0);

    EPD_SendCmd(0x24);
    EPD_DC_HIGH();
    EPD_CS_LOW();
    for(i=0; i< ((EPD_WIDTH + 7)/8)*EPD_HEIGHT; i++)
    {
        //EPD_SendSigData(pattern);
        SWSPI3W_TransmitByte(pattern);
    }
    EPD_CS_HIGH();

    return 0;
}

uint8_t EPD_Disp(uint8_t mode)
{
    if (EPD_BUSY_GET())
    {
        return 1;
    }

    if(mode != m_Current_Mode)
    {
        m_Current_Mode = mode;
        EPD_SendCmd(0x32);
        EPD_SendMultData(EPD_2IN13_lut_update[mode], 29);
    }

    //Turn on Display
    EPD_SendCmd(0x22); // DISPLAY_UPDATE_CONTROL_2
    EPD_SendSigData(0xC7);
    //EPD_SendSigData(0xFF);
    EPD_SendCmd(0X20);	// MASTER_ACTIVATION

    return 0;
}

void EPD_PowerOff(void)
{
    EPD_POW_OFF();
    EPD_RST_LOW();
    SWSPI3W_IOLowPower();
    EPD_CS_LOW();
}

void EPD_SetDeepSleep(uint8_t mode)
{
    EPD_SendCmd(0x10);
    EPD_SendSigData(mode);   

    SWSPI3W_DA_HIGH();
}
