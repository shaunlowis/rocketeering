#ifndef I2C_DRIVER_STM8S007_H
#define I2C_DRIVER_STM8S007_H

#define I2C_SUCCESS 1
#define I2C_FAILURE 0

#include "common.h"

void i2c_init(void);
void i2c_write_byte(uint8_t device_address, uint8_t register_address, uint8_t byte);
void i2c_read(uint8_t device_address, uint8_t register_address, uint8_t bytes[], uint8_t num_bytes);
uint8_t i2c_write_and_verify_byte(uint8_t device_address, uint8_t register_address, uint8_t byte, uint8_t write_mask);


#endif /* I2C_DRIVER_STM8S007_H */