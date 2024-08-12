#ifndef SPL07_PRESSURE_H
#define SPL07_PRESSURE_H
#include "common.h"

#define I2C_SUCCESS 1
#define I2C_FAILURE 0

void spl07_init(void);
void spl07_test(void);
int spl07_read_pressure();


#endif // SPL07_PRESSURE_H