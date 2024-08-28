#include "ICM42670_imu.h"
#include "i2c_driver_STM8S007.h"

#define IMU_CHIP_ADDR 0b11010000 // Actaul address << 1 as SPL expects it in this form

// Register addresses
#define IMU_PWR_MGMT0_REG_ADDR      0x1F
#define IMU_GYRO_CONFIG0_REG_ADDR   0x20
#define IMU_ACCEL_CONFIG0_REG_ADDR  0x21
#define IMU_GYRO_CONFIG1_REG_ADDR   0x23
#define IMU_ACCEL_CONFIG1_REG_ADDR  0x24
#define IMU_INTF_CONFIG1_REG_ADDR   0x36

// Reguister write values and masks
#define IDLE_LN_GYRO_ACCEL          0b00011111
#define IDLE_LN_GYRO_ACCEL_WMASK    0b00011111

#define _2000_DPS_100_HZ            0b00001001
#define _2000_DPS_100_HZ_WMASK      0b01101111

#define _16_G_100_HZ                0b00001001
#define _16_G_100_HZ_WMASK          0b01101111

#define GYRO_LPF_BYPASS             0b00000000
#define GYRO_LPF_BYPASS_WMASK       0b00000111

#define ACCEL_LPF_BYPASS            0b00000000
#define ACCEL_LPF_BYPASS_WMASK      0b01110111

#define PURE_I2C_DEFAULT_CLK        0b00000001
#define PURE_I2C_DEFAULT_CLK_WMASK  0b00001111

/* Init:

- Gyroscope Low-Noise Mode (gyro + accel on)
- no ints
- ACCEL_MODE low noise
- 1st order LPF bandwidth ACCEL_UI_FILT_BW and GYRO_UI_FILT_BW
- Data rates: GYRO_ODR and ACCEL_ODR
- Full scale range selection (- +/- 16g for accelerometer, something for gyro)
- Fifo? probs not

*/

void imu_init(void)
{
    radio_print_debug("Initializing IMU\r\n");
    uint8_t res = 0;

    // Accel config
    res = i2c_write_and_verify_byte(IMU_CHIP_ADDR, IMU_ACCEL_CONFIG0_REG_ADDR, _16_G_100_HZ, _16_G_100_HZ_WMASK);
    if (res==I2C_FAILURE) radio_print_debug("Failure writing and verifying IMU_ACCEL_CONFIG0_REG_ADDR\r\n");

    res = i2c_write_and_verify_byte(IMU_CHIP_ADDR, IMU_ACCEL_CONFIG1_REG_ADDR, ACCEL_LPF_BYPASS, ACCEL_LPF_BYPASS_WMASK);
    if (res==I2C_FAILURE) radio_print_debug("Failure writing and verifying IMU_ACCEL_CONFIG1_REG_ADDR\r\n");    
    
    // Gyro config
    res = i2c_write_and_verify_byte(IMU_CHIP_ADDR, IMU_GYRO_CONFIG0_REG_ADDR, _2000_DPS_100_HZ, _2000_DPS_100_HZ_WMASK);
    if (res==I2C_FAILURE) radio_print_debug("Failure writing and verifying IMU_GYRO_CONFIG0_REG_ADDR\r\n");

    res = i2c_write_and_verify_byte(IMU_CHIP_ADDR, IMU_GYRO_CONFIG1_REG_ADDR, GYRO_LPF_BYPASS, GYRO_LPF_BYPASS_WMASK);
    if (res==I2C_FAILURE) radio_print_debug("Failure writing and verifying IMU_GYRO_CONFIG1_REG_ADDR\r\n");
    
    // INTF_CONFIG1
    res = i2c_write_and_verify_byte(IMU_CHIP_ADDR, IMU_INTF_CONFIG1_REG_ADDR, PURE_I2C_DEFAULT_CLK, PURE_I2C_DEFAULT_CLK_WMASK);
    if (res==I2C_FAILURE) radio_print_debug("Failure writing and verifying IMU_INTF_CONFIG1_REG_ADDR\r\n");

    // Power management (Mode selection for accel gyro)
    res = i2c_write_and_verify_byte(IMU_CHIP_ADDR, IMU_PWR_MGMT0_REG_ADDR, IDLE_LN_GYRO_ACCEL, IDLE_LN_GYRO_ACCEL_WMASK);
    if (res==I2C_FAILURE) radio_print_debug("Failure writing and verifying IMU_PWR_MGMT0_REG_ADDR\r\n");
    delay_us(200); // 200 us delay required after transitioning mode from OFF

    radio_print_debug("IMU init complete\r\n");
    // Should do self test here (SELFTEST, ST_BUSY, ST_DONE, ST_STATUS1, ST_STATUS2. All part of MREG1 map (requires special shit to write))
}