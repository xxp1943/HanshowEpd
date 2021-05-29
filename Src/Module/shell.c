#include "string.h"
#include "softuart.h"
#include "shell.h"
#include "def.h"
#include "RTC_Calendar.h"
#include "msp430.h"
#include "common.h"


#define ARGC_MAX	8
#define RXBUF_MAX	32
#define TXBUF_MAX	32


typedef void (*t_SHELL_FUNC)(void);
typedef struct 
{
	char const *cmd_str;
	t_SHELL_FUNC cmd_func;
}t_SHELL_ITEM;

void SetTime(void);
void ReadTime(void);

t_SHELL_ITEM const c_SHELL_TAB[] = 
{
	{"ST", SetTime},
	{"RT", ReadTime}
};

static uint8_t m_SHELL_LC = 0;
static uint8_t m_SHELL_RXBUF[RXBUF_MAX];
static uint8_t m_SHELL_RXFLAG = 0; //0--no string revceived, 1--string revceived
static uint8_t m_SHELL_TXFLAG = 0; //0--complete, 1-- transmiting
static uint8_t m_SHELL_RXIDX = 0;

static uint8_t m_SHELL_ECHO_IDX = 0;
static uint8_t m_SHELL_TXBUF[TXBUF_MAX];
static uint8_t m_SHELL_TXSIZE = 0;


static char *m_SHELL_ARGV[ARGC_MAX];
static uint8_t m_SHELL_ARGC;
static uint8_t m_SHELL_CMD_LC = 0xFF;

int m_SHELL_TIMEOUT_CNT;


static uint8_t uart_callback(void *d, uint8_t sta, uint8_t rx)
{
    if (sta == 1)// rx
    {
		if (m_SHELL_RXFLAG == 0)
		{
            if (m_SHELL_RXIDX == 0)
            {
                if (rx < ' ')
                {
                    return 0;
                }
            }
			if (m_SHELL_RXIDX < RXBUF_MAX)
			{
				m_SHELL_RXBUF[m_SHELL_RXIDX++] = rx;
			}
			if (rx == '\r')
			{
				m_SHELL_RXFLAG = 1;
			}
		}
		else if (rx == 0x1A) 
        {
            m_SHELL_LC = 0;
            m_SHELL_RXFLAG = 0;
            m_SHELL_RXIDX = 0;
        }
    }
	else// tx
	{
        m_SHELL_TXFLAG = 0;
	}

    m_SHELL_TIMEOUT_CNT = g_SYSTICK;
    return 0;
}




void shell_init(void)
{
    SWUART_Init();
	SWUART_SetCallBack(uart_callback, 0);
    LPM3Prohibit |= LPM3P_SHELL | LPM3P_UART;
    m_SHELL_TIMEOUT_CNT = g_SYSTICK;
	m_SHELL_TXFLAG = 1;
	SWUART_Send("\nEPD Clock\nV01 20210523 LaoGao\n>", sizeof("\nEPD Clock\nV01 20210523 LaoGao\n>")-1);
}



void shell_process(void)
{
    uint8_t *buf; 
    uint8_t len;
    uint8_t shell_idx;
    static uint8_t tmp;
	char *cmd;
    //check the UART_RXD GPIO interrupt flag
    if (P2IFG & SWUART_RXD)
    {
        P2IFG &= ~SWUART_RXD;
        LPM3Prohibit |= LPM3P_UART;
        m_SHELL_TIMEOUT_CNT = g_SYSTICK;
    }
    else if ((g_SYSTICK - m_SHELL_TIMEOUT_CNT) >= 1000)
    {
        LPM3Prohibit &= ~LPM3P_UART;
    }

	switch(m_SHELL_LC)
	{
		case 0:
        for(;;)
        {
            m_SHELL_LC = 4; case 4:
            if (m_SHELL_TXFLAG == 1)
                return;

            tmp = m_SHELL_RXIDX;
            if (tmp > m_SHELL_ECHO_IDX)
            {
                m_SHELL_TXFLAG = 1;
                SWUART_Send(m_SHELL_RXBUF + m_SHELL_ECHO_IDX, tmp - m_SHELL_ECHO_IDX);
                m_SHELL_ECHO_IDX = tmp;
                m_SHELL_LC = 5; case 5:
                if (m_SHELL_TXFLAG == 1)
                    return;
            }
            if (m_SHELL_RXIDX == 0)
                m_SHELL_ECHO_IDX = 0;
            

            LPM3Prohibit &= ~LPM3P_SHELL;
            if (m_SHELL_RXFLAG == 1)
            {
                LPM3Prohibit |= LPM3P_SHELL;

                tmp = m_SHELL_RXIDX;
                if (tmp > m_SHELL_ECHO_IDX)
                {
                    m_SHELL_TXFLAG = 1;
                    SWUART_Send(m_SHELL_RXBUF + m_SHELL_ECHO_IDX, tmp - m_SHELL_ECHO_IDX);
                    m_SHELL_ECHO_IDX = tmp;
                    m_SHELL_LC = 7; case 7:
                    if (m_SHELL_TXFLAG == 1)
                        return;
                }

                m_SHELL_TXFLAG = 1;
                SWUART_Send("\n", 1);
                m_SHELL_LC = 8; case 8:
                if (m_SHELL_TXFLAG == 1)
                    return;
                
                buf = m_SHELL_RXBUF;
                len = m_SHELL_RXIDX;

                //check shell cmd
                if (*buf < 'A' && *buf > 'z')
                {//Error
                    
                }
                cmd = (char*)buf;

                // check parameter
                m_SHELL_ARGC = 0;
                tmp = 0;
                while(len--)
                {
                    if (*buf <= ' ')
                    {
                        *(buf++) = '\0';
                        tmp = 1;
                        continue;
                    }
                    
                    if (tmp)
                    {
                        tmp = 0;
                        if (m_SHELL_ARGC >= ARGC_MAX)
                        {
                            //ERROR
                            strcpy((char*)m_SHELL_TXBUF, "Too many parameters");
                            m_SHELL_TXSIZE = sizeof("Too many parameters") -1;
                            goto CMD_END;
                        }
                        m_SHELL_ARGV[m_SHELL_ARGC++] = (char*)buf;
                    }
                    buf++;
                }

                for (shell_idx = 0; shell_idx < sizeof(c_SHELL_TAB)/sizeof(t_SHELL_ITEM); shell_idx++)
                {
                    if (strcmp(c_SHELL_TAB[shell_idx].cmd_str, cmd) == 0)
                    {
                        m_SHELL_CMD_LC = 0;
                        m_SHELL_LC = 1; case 1:   // case 1
                        c_SHELL_TAB[shell_idx].cmd_func();
                        if (m_SHELL_TXSIZE)
                        {
                            
                            m_SHELL_TXFLAG = 1;
                            SWUART_Send(m_SHELL_TXBUF, m_SHELL_TXSIZE);
                            m_SHELL_TXSIZE = 0;
                            m_SHELL_LC = 2; case 2:
                            if (m_SHELL_TXFLAG == 1)
                                return;
                        }
                        if (m_SHELL_CMD_LC != 0xFF)
                        {
                            return;
                        }
                        goto CMD_END;
                    }
                }
                //ERROR
                strcpy((char*)m_SHELL_TXBUF, "Command Error");
                m_SHELL_TXSIZE = sizeof("Command Error") - 1;

CMD_END:
                m_SHELL_TXBUF[m_SHELL_TXSIZE++] = '\n';
                m_SHELL_TXBUF[m_SHELL_TXSIZE++] = '>';
                m_SHELL_LC = 3; case 3:
                if (m_SHELL_TXFLAG == 1)
                    return;
                m_SHELL_TXFLAG = 1;
                SWUART_Send(m_SHELL_TXBUF, m_SHELL_TXSIZE);
                
                m_SHELL_TXSIZE = 0;
                m_SHELL_RXFLAG = 0;
                m_SHELL_RXIDX = 0;
            }
            m_SHELL_LC = 6; return; case 6: 
            __no_operation();
        }


	}
}

static uint16_t str_to_bcd(char const *str)
{
    uint16_t ret = 0;
    while (*str >= '0' && *str<='9')
    {
        ret = ret << 4;
        ret |= *str -'0';
        str ++;
    }
    return ret;
}
static uint16_t str_to_int(char const *str)
{
    uint16_t ret = 0;
    while (*str >= '0' && *str<='9')
    {
        ret = ret * 10;
        ret += *str -'0';
        str ++;
    }
    return ret;
}
void update_time_full(void);
void SetTime(void)
{
    uint16_t buf[6];
    uint8_t i, hour12, ampm;

    m_SHELL_CMD_LC = 0xFF;

    if (m_SHELL_ARGC != 6)
    {
        strcpy((char*)m_SHELL_TXBUF, "Parameter Error");
        m_SHELL_TXSIZE = sizeof("Parameter Error") -1;
        return;
    }

    for (i = 0; i<4; i++)
    {
        buf[i] = str_to_int(m_SHELL_ARGV[i]);
    }
    for (i = 4; i<6; i++)
    {
        buf[i] = str_to_bcd(m_SHELL_ARGV[i]);
    }

    setDate(buf[0], buf[1], buf[2]);
    if (buf[3] >= 12)
    {
        ampm = 1;
        hour12 = buf[3]%12;
    }
    else
    {
        ampm = 0;
    }
    if (hour12 == 0)
    {
        hour12 = 12;
    }
    hour12 = (hour12 & 0xf0) * 10 + (hour12 & 0xf);
    setTime(hour12, buf[4], buf[5], ampm);
    
    strcpy((char*)m_SHELL_TXBUF, "OK");
    m_SHELL_TXSIZE = sizeof("OK") -1;
    
	
}

void ReadTime(void)
{
	m_SHELL_CMD_LC = 0xFF;
}
