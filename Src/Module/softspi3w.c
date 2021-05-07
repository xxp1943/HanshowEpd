#include "softspi3w.h"
#include "msp430.h"


#define SEND_BIT(shifter)               do {if(shifter & 0x80)\
                                                SWSPI3W_DA_HIGH(); \
                                            else\
                                                SWSPI3W_DA_LOW();}while(0)
#define READ_BIT(shifter)               do{ if(SWSPI3W_DA_GET())\
                                                shifter |= 0x1;\
                                            else\
                                                 shifter &= ~0x1;}while(0);

#if SWSPI_CPOL == 0
#define FIRST_EDGE()            SWSPI3W_SCK_HIGH()
#define SECOND_EDGE()            SWSPI3W_SCK_LOW()
#else
#define FIRST_EDGE()            SWSPI3W_SCK_LOW()
#define SECOND_EDGE()            SWSPI3W_SCK_HIGH()
#endif

void SWSPI3W_IOInit(void)
{
    SECOND_EDGE();
    SWSPI3W_DA_HIGH();
    SWSPI3W_DA_SETOUT();
}

void SWSPI3W_IOLowPower(void)
{
    SWSPI3W_SCK_LOW();
    SWSPI3W_DA_HIGH();
    SWSPI3W_DA_SETOUT();
}

void SWSPI3W_TransmitByte(uint8_t txbyte)
{
    uint8_t i = 8;
    SWSPI3W_DA_SETOUT();
    while(i--)
    {

    #if SWSPI_CPHA == 0
        SEND_BIT(txbyte);
        FIRST_EDGE();
        SECOND_EDGE();
    #else
        FIRST_EDGE();
        SEND_BIT(txbyte);
        SECOND_EDGE();
    #endif
        txbyte <<= 1;
    }
}
void SWSPI3W_TransmitMultByte(uint8_t const *txbuf, uint16_t size)
{
    uint8_t i, txbyte = 0xFF;
    SWSPI3W_DA_SETOUT();
    while(size--)
    {
        if (txbuf)
        {
            txbyte = *(txbuf++);
        }
        i = 8;

        while(i--)
        {

        #if SWSPI_CPHA == 0
            SEND_BIT(txbyte);
            FIRST_EDGE();
            SECOND_EDGE();
        #else
            FIRST_EDGE();
            SEND_BIT(txbyte);
            SECOND_EDGE();
        #endif
            txbyte <<= 1;
        }
    }
}

uint8_t SWSPI3W_ReceiveByte(void)
{
    uint8_t i = 8, rxbyte;
    SWSPI3W_DA_SETIN();
    while(i--)
    {

    #if SWSPI_CPHA == 0
        FIRST_EDGE();
        READ_BIT(rxbyte);
        SECOND_EDGE();
    #else
        FIRST_EDGE();
        SECOND_EDGE();
        READ_BIT(rxbyte);
    #endif
        rxbyte <<= 1; 
    }
    return rxbyte;
}

void SWSPI3W_ReceiveMultByte(uint8_t *rxbuf, uint16_t size)
{
    uint8_t i, rxbyte;
    SWSPI3W_DA_SETIN();
    while(size--)
    {
        i = 8;
        while(i--)
        {

        #if SWSPI_CPHA == 0
            FIRST_EDGE();
            READ_BIT(rxbyte);
            SECOND_EDGE();
        #else
            FIRST_EDGE();
            SECOND_EDGE();
            READ_BIT(rxbyte);
        #endif
            rxbyte <<= 1; 
        }

        if (rxbuf)
        {
            *(rxbuf++) = rxbyte;
        }
    }
}


