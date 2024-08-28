// Broadcasts messages in NMEA format
// Decoder: https://github.com/kosma/minmea


/* Change baud rate with PMKT251 
https://cdn.sparkfun.com/assets/parts/1/2/2/8/0/PMTK_Packet_User_Manual.pdf
https://forum.arduino.cc/t/changing-gps-baud-rate-with-minimal-nmeagps-and-gpsport/586590/2
*/

#include "YIC_gps.h"
#include "gpio.h"
#include "stm8s_uart3.h"

void gps_uart_send_string(char buff[]);


void gps_uart_send_string(char buff[])
{
    int i = 0;
    while (buff[i] != '\0')
    {
        UART3_SendData8(buff[i]);
        // Blocks until transmit data register is empty, and ready for another one
        while(!UART3_GetFlagStatus(UART3_FLAG_TXE)) continue;
        i++;
    }
    // Now wait for transmission to be complete
    while(!UART3_GetFlagStatus(UART3_FLAG_TC)) continue;
    delay_ms(100); // Needed to send one after another
}

void gps_init(void)
{
    UART3_Cmd(ENABLE);
    UART3_Init(9600, UART3_WORDLENGTH_8D, UART3_STOPBITS_1, UART3_PARITY_NO, UART3_MODE_TXRX_ENABLE);
    GPIO_WriteReverse(LED_PORT, LED_PIN);
    gps_uart_send_string("$PGKC147,115200*06\r\n"); // Change baud rate to 115200
    GPIO_WriteReverse(LED_PORT, LED_PIN);
    UART3_DeInit();
    delay_ms(1000);
    UART3_Init(115200, UART3_WORDLENGTH_8D, UART3_STOPBITS_1, UART3_PARITY_NO, UART3_MODE_TXRX_ENABLE);

}

void gps_test(void)
{
    char buff[256];
    int count = 0;
    while(1)
    {
        if(UART3_GetFlagStatus(UART3_FLAG_RXNE))
        {
            char c =  UART3_ReceiveData8();
            buff[count++] = c;
        }
        if (count == 255)
        {
            radio_print_debug(buff);
            count = 0;
        }
    }
    
}