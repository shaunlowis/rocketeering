#ifndef BATTERY_H
#define BATTERY_H

#include "common.h"

void init_battery_measurements(void);
void read_batt_voltage(void);
void read_batt_current(void);
float get_batt_voltage(void);
float get_batt_current(void);



#endif // BATTERY_H