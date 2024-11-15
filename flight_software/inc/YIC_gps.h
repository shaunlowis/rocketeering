#ifndef YIC_GPS_H
#define YIC_GPS_H

#include "common.h"


INTERRUPT_HANDLER(UART3_RXNE_IRQHandler, 21);
void gps_init(void);
void read_gps_buffer(void);

float gps_get_lat_float(void);
float gps_get_long_float(void);
float gps_get_speed_float(void);
char gps_get_mode(void);
int gps_get_fix_type(void);
int gps_get_fix_quality(void);
int gps_get_satellites_tracked(void);
float gps_get_altitude(void);
float gps_get_height(void);
float gps_get_pdop(void);
float gps_get_hdop(void);
float gps_get_vdop(void);

#endif // YIC_GPS_H