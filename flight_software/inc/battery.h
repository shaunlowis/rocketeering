#ifndef BATTERY_H
#define BATTERY_H

#include "common.h"

void init_battery_measurements(void);
float read_batt_voltage(void);
float read_batt_current(void);


#endif // BATTERY_H