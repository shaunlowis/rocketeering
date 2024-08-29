#ifndef YIC_GPS_H
#define YIC_GPS_H

#include "common.h"

#define UART3_RX_FULL_IRQ_NO 21 // Table 10 of STM8S007C8 datasheet


INTERRUPT_HANDLER(UART3_RXNE_IRQHandler, 21);
void gps_init(void);
void gps_test(void);

#endif // YIC_GPS_H