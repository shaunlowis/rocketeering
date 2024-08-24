#include "common.h"
#include "ebyte_radio.h"

void radio_uart_init(void)
{
    UART1_Cmd(ENABLE);
    UART1_Init(115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
}


void radio_print_debug(char buff[])
{
    #ifdef DEBUG_ENABLE
    int i = 0;
    while (buff[i] != '\0')
    {
        UART1_SendData8(buff[i]);
        // Blocks until transmit data register is empty, and ready for another one
        while(!UART1_GetFlagStatus(UART1_FLAG_TXE)) continue;
        i++;
    }
    // Now wait for transmission to be complete
    while(!UART1_GetFlagStatus(UART1_FLAG_TC)) continue;
    delay_ms(100); // Needed to send one after another
    #endif
}