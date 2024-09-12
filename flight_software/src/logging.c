// #include "logging.h"
// #include "common.h"
// #include "YIC_gps.h"
// #include "ICM42670_imu.h"
// #include "SPL07_pressure.h"
// #include "microsd.h"

// #define LOG_CIRCBUFF_LEN 750

// uint32_t current_SD_addr = STARTING_LOG_ADDR;


// typedef struct char_circ_buffer {
//     uint16_t current_length;
//     uint16_t ri;
//     uint16_t wi;
//     char buffer[LOG_CIRCBUFF_LEN];
// } char_circbuff_t;

// char_circbuff_t circ_buff = {.ri=0, .wi=0, .current_length=0};


// static char char_circbuff_read(void)
// {
//     char retval = circ_buff.buffer[circ_buff.ri++];
//     circ_buff.current_length--;
//     if (circ_buff.ri == LOG_CIRCBUFF_LEN)
//     {
//         circ_buff.ri = 0;
//     }
//     return retval;
// }

// static void char_circbuff_write(char c)
// {
//     if (circ_buff.current_length == LOG_CIRCBUFF_LEN)
//     {
//         GPIO_WriteReverse(RED_LED_PORT, RED_LED_PIN);
//         circ_buff.current_length = 0; // Reset buffer (will lose all unread values)
//     }
//     circ_buff.buffer[circ_buff.wi++] = c;
//     circ_buff.current_length++;
//     if (circ_buff.wi == LOG_CIRCBUFF_LEN)
//     {
//         circ_buff.wi = 0;
//     }   
// }


// void add_val_to_circbuff(void* ptr, uint8_t num_bytes)
// {
//     char *byte_ptr = (char *)ptr;
//     for (int i=0; i<num_bytes; i++)
//     {
//         char_circbuff_write(byte_ptr[i]);
//     }
// }

// void test_log_buff(void)
// {
//     uint16_t num = 43981;
//     add_val_to_circbuff(&num, sizeof(num));
//     char res1 = char_circbuff_read();
//     char res2 = char_circbuff_read();
//     char pbuf[50];
//     sprintf(pbuf, "%x %x\r\n", res1, res2);
//     radio_print_debug(pbuf);
//     while(1) continue;
// }

// void add_data_to_log_buff(void)
// {
//     while(1)
//     {
    
        
//         char startbyte = 0xAA;
//         add_val_to_circbuff(&startbyte, sizeof(startbyte));

//         uint32_t timestamp = millis();
//         //add_val_to_circbuff(&timestamp, sizeof(timestamp));

//         // // Get GPS data
//         // float lati = gps_get_lat_float();
//         // float longi = gps_get_long_float();
//         // float speed = gps_get_speed_float();
//         // char mode = gps_get_mode();
//         // int fix_type = gps_get_fix_type();
//         // int fix_quality = gps_get_fix_quality();
//         // int sats_tracked = gps_get_satellites_tracked();
//         // float gps_alt = gps_get_altitude();
//         // float height = gps_get_height();

//         // // get imu state
//         // imuState_t imu_state;
//         // imu_state = get_imu_state();

//         // Get pressure state
//         float pressure = get_baro_pressure();
//         //add_val_to_circbuff(&pressure, sizeof(pressure));

//         uint16_t test = 48254;
//         add_val_to_circbuff(&test, sizeof(pressure));

//         char endbyte = '\n';
//         add_val_to_circbuff(&endbyte, sizeof(endbyte));




//         // Check if we have a sector to write
//         if (circ_buff.current_length >= 512)
//         {
//             char buff[512];
//             for (int i=0; i<512; i++)
//             {
//                 buff[i] = char_circbuff_read();
//             }

//             MSD_WriteBlock(buff, current_SD_addr++, 512);
//             GPIO_WriteReverse(GREEN_LED_PORT, GREEN_LED_PIN);
//             while(1) continue;
//         }
//     }
// }


// void print_sd_to_radio(void)
// {
//     char bytes_read[512];
//     MSD_ReadBlock(bytes_read, STARTING_LOG_ADDR, 512);
//     char pbuff[10];
//     for (int i=0; i<512; i++)
//     {
//         sprintf(pbuff, "%x ", bytes_read[i]);
//         radio_print_debug(pbuff);
//     }

//     while (1){
//     GPIO_WriteReverse(GREEN_LED_PORT, GREEN_LED_PIN);
//     delay_ms(500);
//   }
// }