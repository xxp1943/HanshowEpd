#include "flash.h"
#include "ucbspi.h"

uint8_t test_buf[6];

uint8_t const c_READ_STATUS[2] = {0x05, 0x35};

static void FLASH_Addr2String(uint32_t addr, uint8_t *str)
{
    str[0] = (addr>>16) & 0xff;
    str[1] = (addr>>8) & 0xff;
    str[2] = addr & 0xff;
}
uint8_t FLASH_ReadStatus(uint8_t index)
{
    uint8_t buf[2];
    buf[0] = c_READ_STATUS[index];
    FLASH_CS_LOW();
    UCBSPI_Transfer(buf, buf, 2);
    FLASH_CS_HIGH();
    return buf[1];
}
void FLASH_WriteStatus(uint16_t status)
{
    uint8_t buf[3];
    buf[0] = 0x01;
    buf[1] = (status) & 0xFF;
    buf[2] = (status>>8) & 0xFF;
    FLASH_CS_LOW();
    UCBSPI_Transfer(buf, 0, 3);
    FLASH_CS_HIGH();
}
void FLASH_Read(uint32_t addr, uint8_t *rxbuf, uint16_t size)
{
    uint8_t buf[4];
    buf[0] = 0x03;
    FLASH_Addr2String(addr, buf+1);
    FLASH_CS_LOW();
    UCBSPI_Transfer(buf, 0, 4);
    UCBSPI_Transfer(0, rxbuf, size);
    FLASH_CS_HIGH();
}
void FLASH_Write(uint32_t addr, uint8_t const *txbuf, uint16_t size)
{
    uint8_t buf[4], status;
    uint16_t txcnt;
    uint32_t addr_end = addr + size - 1;
    buf[0] = 0x02;
    buf[1] = (addr>>16) & 0xff;
    buf[2] = (addr>>8) & 0xff;
    buf[3] = addr & 0xff;
    
    while(size)
    {
        FLASH_Addr2String(addr, buf+1);
        FLASH_CS_LOW();
        UCBSPI_Transfer(buf, 0, 4);
        if ((addr_end & 0xFFFFFF00) != (addr & 0xFFFFFF00))
        {
            txcnt = 0x100 - (addr & 0xFF);
        }
        else 
        {
            txcnt = addr_end - addr + 1;
        }
        UCBSPI_Transfer(txbuf, 0,txcnt);
        FLASH_CS_HIGH();
        txbuf += txcnt;
        size -= txcnt;

        do{
            DelayMs(1);
            status = FLASH_ReadStatus(1);
        }while(status & 0x80);
    }
}

void FLASH_Init(void)
{
    UCBSPI_Init();

    //Test 

    /*test_buf[0] = 0xAB;
    FLASH_CS_LOW();
    UCBSPI_Transfer(test_buf, test_buf, 5);
    FLASH_CS_HIGH();

    test_buf[0] = 0x9F;
    FLASH_CS_LOW();
    UCBSPI_Transfer(test_buf, test_buf, 4);
    FLASH_CS_HIGH();
*/
    FLASH_CS_LOW();
    UCBSPI_Transfer("\xB9", 0, 1);
    FLASH_CS_HIGH();

}




