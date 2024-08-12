#ifndef SPL07_PRESSURE_H
#define SPL07_PRESSURE_H
#include "common.h"

void spl07_init(void);
void spl07_test(void);

void i2c_write_byte(uint8_t device_address, uint8_t register_address, uint8_t byte);
void i2c_read(uint8_t device_address, uint8_t register_address, uint8_t bytes[], uint8_t num_bytes);


#endif // SPL07_PRESSURE_H