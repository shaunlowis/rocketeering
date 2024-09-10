#ifndef YIC_GPS_H
#define YIC_GPS_H

#include "common.h"


INTERRUPT_HANDLER(UART3_RXNE_IRQHandler, 21);
void gps_init(void);
void gps_test(void);

#endif // YIC_GPS_H