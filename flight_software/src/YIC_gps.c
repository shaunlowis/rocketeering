// Broadcasts messages in NMEA format
// Decoder: https://github.com/kosma/minmea


/* Change baud rate with PMKT251 
https://cdn.sparkfun.com/assets/parts/1/2/2/8/0/PMTK_Packet_User_Manual.pdf
https://forum.arduino.cc/t/changing-gps-baud-rate-with-minimal-nmeagps-and-gpsport/586590/2
*/

#include "YIC_gps.h"
#include "gpio.h"
#include "stm8s_uart3.h"

#define NMEA_MSG_MAX_LENGTH 256 // Guessed
#define NMEA_CIRCBUFF_SIZE 12 // x2 number of messsages GPS sends
#define NMEA_START_CHAR '$'
#define NMEA_END_CHAR '\n'


typedef enum nmea_rx_msg_state{
    IDLE=0,
    DECODING=1,
} nmea_rx_msg_state_t;

typedef struct nmea_msg {
    nmea_rx_msg_state_t state;
    char msg_buff[NMEA_MSG_MAX_LENGTH];
    uint16_t length;
} nmea_msg_t;

typedef struct nmea_msg_circbuff {
    uint8_t current_length;
    uint8_t ri;
    uint8_t wi;
    nmea_msg_t buffer[NMEA_CIRCBUFF_SIZE];
} nmea_msg_circbuff_t;

nmea_msg_circbuff_t nmea_circbuff = {.current_length=0,
                                     .ri=0,
                                     .wi=0};


static void gps_uart_send_string(char buff[]);
static void nmea_parse_char(char c);
static void write_nmea_circbuff(void);


// See stm8s.h INTERRUPT_HANDLER for how to define interrupt handler.
// Look at interrupt vector mapping table 10 of the datasheet for the IRQ no.
INTERRUPT_HANDLER(UART3_RXNE_IRQHandler, ITC_IRQ_UART3_RX)
{
    char c = UART3_ReceiveData8(); 
    // Call nmea parse char
    nmea_parse_char(c);
}

static void nmea_circbuff_write_complete(void)
{
    // Not really the writing function, just handles incrementing the write index
    if (nmea_circbuff.current_length == NMEA_CIRCBUFF_SIZE)
    {
        GPIO_WriteHigh(RED_LED_PORT, RED_LED_PIN);
        nmea_circbuff.current_length = 0; // Reset buffer (will lose all unread values
    }
    nmea_circbuff.wi++;
    nmea_circbuff.current_length++;
    if (nmea_circbuff.wi == NMEA_CIRCBUFF_SIZE)
    {
        nmea_circbuff.wi = 0;
    }
}

static void nmea_circbuff_read_complete(void)
{
    nmea_circbuff.ri++;
    nmea_circbuff.current_length--;
    if (nmea_circbuff.ri == NMEA_CIRCBUFF_SIZE)
    {
        nmea_circbuff.ri = 0;
    }
}

static void nmea_parse_char(char c)
{
    nmea_msg_t* curr_msg_p = &nmea_circbuff.buffer[nmea_circbuff.wi];
    // Check if we have got a new message
    switch (curr_msg_p->state) {
        case IDLE:
            if (c == NMEA_START_CHAR)
            {
                curr_msg_p->msg_buff[curr_msg_p->length++] = c;
                curr_msg_p->state = DECODING;
            }
            break;
        case DECODING:
            if (c == NMEA_START_CHAR)
            {
                // Did not recieve stop, discard the packet and reset the nmea_msg
                curr_msg_p->length = 0;
                curr_msg_p->msg_buff[curr_msg_p->length++] = c;
                curr_msg_p->state = DECODING;
            } else if (c == NMEA_END_CHAR)
            {
                curr_msg_p->msg_buff[curr_msg_p->length++] = c;
                nmea_circbuff_write_complete();
                curr_msg_p->state = IDLE;              
            } else 
            {
                curr_msg_p->msg_buff[curr_msg_p->length++] = c;
            }
            break;
    }
    // Ensure there wont be memory overrun of msg buffer
    if (curr_msg_p->length == NMEA_MSG_MAX_LENGTH - 1)
    {
        curr_msg_p->length = 0;
        curr_msg_p->state = IDLE;
    }
}


static void gps_uart_send_string(char buff[])
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
    // init the nmea circbuff to default values
    for(int i=0; i<NMEA_CIRCBUFF_SIZE; i++)
    {
        nmea_circbuff.buffer[i].length = 0;
        nmea_circbuff.buffer[i].state = IDLE;
    }

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

        delay_ms(100);
        if (nmea_circbuff.current_length > 0)
        {
            nmea_circbuff_read_complete();
            
        }
    }
    
}