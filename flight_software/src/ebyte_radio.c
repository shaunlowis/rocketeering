#include "common.h"
#include "ebyte_radio.h"

void radio_uart_init(void)
{
  UART1_Init(115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
}


void radio_transmit_string(char buff[], int len)
{
    for (int i=0; i<len; i++)
    {
        UART1_SendData8(buff[i]);
        // Blocks until transmit data register is empty, and ready for another one
        while(UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
        {
            continue;
        }
    }
}