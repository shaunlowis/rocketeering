// Broadcasts messages in NMEA format
// Decoder: https://github.com/kosma/minmea


/* Change baud rate with PMKT251 
https://cdn.sparkfun.com/assets/parts/1/2/2/8/0/PMTK_Packet_User_Manual.pdf
https://forum.arduino.cc/t/changing-gps-baud-rate-with-minimal-nmeagps-and-gpsport/586590/2
*/

#include "YIC_gps.h"
#include "gpio.h"
#include "stm8s_uart3.h"
#include <math.h>
#include "minmea.h"

#define NMEA_CIRCBUFF_SIZE 12 // x2 number of messsages GPS sends
#define NMEA_START_CHAR '$'
#define NMEA_END_CHAR1 '\n' // '\r'
#define NMEA_END_CHAR2 '\n'


typedef enum nmea_rx_msg_state{
    IDLE=0,
    DECODING=1,
} nmea_rx_msg_state_t;

typedef struct nmea_msg {
    nmea_rx_msg_state_t state;
    char msg_buff[MINMEA_MAX_SENTENCE_LENGTH];
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
        GPIO_WriteReverse(RED_LED_PORT, RED_LED_PIN);
        nmea_circbuff.current_length = 0; // Reset buffer (will lose all unread values)
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
            } else if ((c == NMEA_END_CHAR1) || (c == NMEA_END_CHAR2))
            {
                curr_msg_p->msg_buff[curr_msg_p->length++] = c;
                // add a null char
                curr_msg_p->msg_buff[curr_msg_p->length] = '\0';
                curr_msg_p->state = IDLE;
                curr_msg_p->length = 0;
                nmea_circbuff_write_complete();             
            } else 
            {
                curr_msg_p->msg_buff[curr_msg_p->length++] = c;
            }
            break;
    }
    // Ensure there wont be memory overrun of msg buffer
    if (curr_msg_p->length == MINMEA_MAX_SENTENCE_LENGTH - 1)
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

void minmea_decode(char* line)
{
    enum minmea_sentence_id id = minmea_sentence_id(line, false);
    char buf1[50];
    sprintf(buf1, "ID: %"PRId16"\r\n", (int16_t)id);
    radio_print_debug(buf1);
    switch (id) 
        {
        // case MINMEA_SENTENCE_RMC: {
        //     struct minmea_sentence_rmc frame;
        //     if (minmea_parse_rmc(&frame, line)) {
        //         char buff[256];
        //         sprintf(buff, "$xxRMC floating point degree coordinates and speed: (%f,%f) %f\r\n",
        //                 minmea_tocoord(&frame.latitude),
        //                 minmea_tocoord(&frame.longitude),
        //                 minmea_tofloat(&frame.speed));
        //         radio_print_debug(buff);
        //     }
        //     else {
        //         radio_print_debug("$xxRMC sentence is not parsed\r\n");
        //     }
        // } break;

        case MINMEA_SENTENCE_GGA:
            struct minmea_sentence_gga frame;
            if (minmea_parse_gga(&frame, line)) {
                char buff[256];
                sprintf(buff, "$xxGGA: fix quality: %d\r\n", frame.fix_quality);
            }
            else {
                radio_print_debug("$xxGGA sentence is not parsed\r\n");
            }
            break;

        // case MINMEA_SENTENCE_GST: {
        //     struct minmea_sentence_gst frame;
        //     if (minmea_parse_gst(&frame, line)) {
        //         radio_print_debug("$xxGST: raw latitude,longitude and altitude error deviation: (%d/%d,%d/%d,%d/%d)\n",
        //                 frame.latitude_error_deviation.value, frame.latitude_error_deviation.scale,
        //                 frame.longitude_error_deviation.value, frame.longitude_error_deviation.scale,
        //                 frame.altitude_error_deviation.value, frame.altitude_error_deviation.scale);
        //         radio_print_debug("$xxGST fixed point latitude,longitude and altitude error deviation"
        //                " scaled to one decimal place: (%d,%d,%d)\n",
        //                 minmea_rescale(&frame.latitude_error_deviation, 10),
        //                 minmea_rescale(&frame.longitude_error_deviation, 10),
        //                 minmea_rescale(&frame.altitude_error_deviation, 10));
        //         radio_print_debug("$xxGST floating point degree latitude, longitude and altitude error deviation: (%f,%f,%f)",
        //                 minmea_tofloat(&frame.latitude_error_deviation),
        //                 minmea_tofloat(&frame.longitude_error_deviation),
        //                 minmea_tofloat(&frame.altitude_error_deviation));
        //     }
        //     else {
        //         radio_print_debug("$xxGST sentence is not parsed\n");
        //     }
        // } break;

        // case MINMEA_SENTENCE_GSV: {
        //     struct minmea_sentence_gsv frame;
        //     if (minmea_parse_gsv(&frame, line)) {
        //         radio_print_debug("$xxGSV: message %d of %d\n", frame.msg_nr, frame.total_msgs);
        //         radio_print_debug("$xxGSV: satellites in view: %d\n", frame.total_sats);
        //         for (int i = 0; i < 4; i++)
        //             radio_print_debug("$xxGSV: sat nr %d, elevation: %d, azimuth: %d, snr: %d dbm\n",
        //                 frame.sats[i].nr,
        //                 frame.sats[i].elevation,
        //                 frame.sats[i].azimuth,
        //                 frame.sats[i].snr);
        //     }
        //     else {
        //         radio_print_debug("$xxGSV sentence is not parsed\n");
        //     }
        // } break;

        // case MINMEA_SENTENCE_VTG: {
        //    struct minmea_sentence_vtg frame;
        //    if (minmea_parse_vtg(&frame, line)) {
        //         radio_print_debug("$xxVTG: true track degrees = %f\n",
        //                minmea_tofloat(&frame.true_track_degrees));
        //         radio_print_debug("        magnetic track degrees = %f\n",
        //                minmea_tofloat(&frame.magnetic_track_degrees));
        //         radio_print_debug("        speed knots = %f\n",
        //                 minmea_tofloat(&frame.speed_knots));
        //         radio_print_debug("        speed kph = %f\n",
        //                 minmea_tofloat(&frame.speed_kph));
        //    }
        //    else {
        //         radio_print_debug("$xxVTG sentence is not parsed\n");
        //    }
        // } break;

        // case MINMEA_SENTENCE_ZDA: {
        //     struct minmea_sentence_zda frame;
        //     if (minmea_parse_zda(&frame, line)) {
        //         radio_print_debug("$xxZDA: %d:%d:%d %02d.%02d.%d UTC%+03d:%02d\n",
        //                frame.time.hours,
        //                frame.time.minutes,
        //                frame.time.seconds,
        //                frame.date.day,
        //                frame.date.month,
        //                frame.date.year,
        //                frame.hour_offset,
        //                frame.minute_offset);
        //     }
        //     else {
        //         radio_print_debug("$xxZDA sentence is not parsed\n");
        //     }
        // } break;

        // case MINMEA_INVALID: {
        //     radio_print_debug("$xxxxx sentence is not valid\r\n");
        // } break;

        default: {
            // radio_print_debug("$xxxxx sentence is not parsed\n");
        } break;
    }
}

void gps_test(void)
{
    while(1)
    {
        if (nmea_circbuff.current_length > 0)
        {
            nmea_msg_t* nmea_read_msg_ptr = &nmea_circbuff.buffer[nmea_circbuff.ri];
            char* line = nmea_read_msg_ptr->msg_buff;
            radio_print_debug(line);
            minmea_decode(line);
            nmea_circbuff_read_complete();
        }
    }
    
}