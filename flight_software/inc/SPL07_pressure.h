#ifndef SPL07_PRESSURE_H
#define SPL07_PRESSURE_H
#include "common.h"

#define I2C_SUCCESS 1
#define I2C_FAILURE 0

void spl07_init(void);
void spl07_test(void);

void i2c_write_byte(uint8_t device_address, uint8_t register_address, uint8_t byte);
void i2c_read(uint8_t device_address, uint8_t register_address, uint8_t bytes[], uint8_t num_bytes);
uint8_t i2c_write_and_verify_byte(uint8_t device_address, uint8_t register_address, uint8_t byte, uint8_t write_mask);

#endif // SPL07_PRESSURE_H