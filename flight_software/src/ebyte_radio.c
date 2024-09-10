#include "common.h"
#include "stm8s_uart1.h"
#include "ebyte_radio.h"
#include "YIC_gps.h"
#include "ICM42670_imu.h"
#include "SPL07_pressure.h"

INTERRUPT_HANDLER(UART1_RXNE_IRQHandler, ITC_IRQ_UART1_RX)
{
    char c = UART1_ReceiveData8();
    GPIO_WriteReverse(RED_LED_PORT, RED_LED_PIN);
}

void radio_uart_init(void)
{
    UART1_Cmd(ENABLE);
    UART1_Init(115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
    disableInterrupts(); // Must do before ITC_SetSoftwarePriority
    UART1_ClearFlag(UART1_FLAG_RXNE); // Clear flag in case it is set
    ITC_SetSoftwarePriority(ITC_IRQ_UART1_RX, ITC_PRIORITYLEVEL_2); // Need lower priority than delay interrupt
    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
    enableInterrupts();
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
    //delay_ms(100); // Needed to send one after another
    #endif
}

// void send_telemetry(void)
// {
//     // Get GPS data
//     float lati = gps_get_lat_float();
//     float longi = gps_get_long_float();
//     float speed = gps_get_speed_float();
//     char mode = gps_get_mode();
//     int fix_type = gps_get_fix_type();
//     int fix_quality = gps_get_fix_quality();
//     int sats_tracked = gps_get_satellites_tracked();
//     float gps_alt = gps_get_altitude();
//     float height = gps_get_height();

//     // get imu state
//     imuState_t imu_state;
//     imu_state = get_imu_state();

//     // Get pressure state
//     float pressure = get_baro_pressure();
    
//     char buf[1000];
//     sprintf(buf, "%f %f %f %c %u %u %u %f %f\r\n", lati,
//                                                 longi,
//                                                 speed,
//                                                 mode,
//                                                 fix_type,
//                                                 fix_quality,
//                                                 sats_tracked,
//                                                 gps_alt,
//                                                 height);
//     radio_print_debug(buf);
// }