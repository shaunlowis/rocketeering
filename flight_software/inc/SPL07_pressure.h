#ifndef SPL07_PRESSURE_H
#define SPL07_PRESSURE_H
#include "common.h"

void spl07_init(void);
void spl07_test(void);
void spl07_update_baro(void);
float get_baro_pressure(void);


#endif // SPL07_PRESSURE_H