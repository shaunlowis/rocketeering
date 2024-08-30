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

char gps_read_buff[500];
int count = 0;


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
    radio_print_debug("Initialising GPS\r\n");
    // Enable Uart at default GPS baud rate of 9600
    
    UART3_Init(9600, UART3_WORDLENGTH_8D, UART3_STOPBITS_1, UART3_PARITY_NO, UART3_MODE_TXRX_ENABLE);
    UART3_Cmd(ENABLE);

    // Increase baud rate of GPS
    gps_uart_send_string("$PGKC147,115200*06\r\n"); // Change baud rate to 115200
    UART3_Cmd(DISABLE);
    UART3_DeInit();
    delay_ms(1000);


    UART3_Init(115200, UART3_WORDLENGTH_8D, UART3_STOPBITS_1, UART3_PARITY_NO, UART3_MODE_TXRX_ENABLE);

    // // Setup interrups for rx
    radio_print_debug("Setting up interrupts\r\n");
    disableInterrupts(); // Must do before ITC_SetSoftwarePriority
    UART3_ClearFlag(UART3_FLAG_RXNE); // Clear flag in case it is set
    ITC_SetSoftwarePriority(ITC_IRQ_UART3_RX, ITC_PRIORITYLEVEL_2); // Need lower priority than delay interrupt
    UART3_ITConfig(UART3_IT_RXNE_OR, ENABLE);
    enableInterrupts();
    UART3_Cmd(ENABLE); 
}

void gps_test(void)
{
    while(1)
    {
        
        // if(UART3_GetFlagStatus(UART3_FLAG_RXNE))
        // {
        //     GPIO_WriteReverse(LED_PORT, LED_PIN);
        //     char c =  UART3_ReceiveData8();
        //     //buff[count++] = c;
        // }
        if (count >= 255)
        {
            radio_print_debug(gps_read_buff);
            count = 0;
        }
        delay_ms(100);
    }
    
}


// See stm8s.h INTERRUPT_HANDLER for how to define interrupt handler.
// Look at interrupt vector mapping table 10 of the datasheet for the IRQ no.
INTERRUPT_HANDLER(UART3_RXNE_IRQHandler, ITC_IRQ_UART3_RX)
{
    char c = UART3_ReceiveData8();
    gps_read_buff[count++] = c;
    UART3_ClearITPendingBit(UART3_IT_RXNE); //Clear interrupt (not needed as already cleared by reading the UART_DR reg)

}
