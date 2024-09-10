// https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/4843/SPL07-003.pdf 
// https://github.com/betaflight/betaflight/blob/6dcc2689182aa22707dce16448c57b40af9dd957/src/main/drivers/barometer/barometer_dps310.c#L73

#include "SPL07_pressure.h"
#include "common.h"
#include "i2c_driver_STM8S007.h"

#define SPL07_CHIP_ADDR 0xEE // = (0x77 << 1), i.e left shifted by 1 as SPL expects it in this form


#define SPL07_PRS_B2_ADDR 0x00
#define SPL07_PRS_CFG_ADDR 0x06
#define SPL07_TMP_CFG_ADDR 0x07
#define SPL07_MEAS_CFG_ADDR 0x08
#define SPL07_CFG_ADDR 0x09

#define SPL07_MEAS_CFG_W_MASK 0x07


#define CONTINUOUS_PRESS_TEMP 0x07
#define PRS_4_SPS_64X_OVERSAMPLING 0b00100110
#define TMP_4_SPS_1X_OVERSAMPLING  0b00100000
#define P_SHIFT_EN 0b00000100
#define ALL_DISABLE 0x00

#define NUM_PRESSURE_AND_TEMP_BYTES 6
#define NUM_COEF_REGS 21


typedef struct {
    float c0;     // 12bit
    float c1;     // 12bit
    float c00;    // 20bit
    float c10;    // 20bit
    float c01;    // 16bit
    float c11;    // 16bit
    float c20;    // 16bit
    float c21;    // 16bit
    float c30;    // 16bit
    float c31;    // 12bit
    float c40;    // 12bit
} calibrationCoefficients_t;

typedef struct {
    calibrationCoefficients_t   calib;
    float                       pressure;       // Pa
    float                       temperature;    // DegC
} baroState_t;

static baroState_t  baroState;

void i2c_write_byte(uint8_t device_address, uint8_t register_address, uint8_t byte);
void i2c_read(uint8_t device_address, uint8_t register_address, uint8_t bytes[], uint8_t num_bytes);
uint8_t i2c_write_and_verify_byte(uint8_t device_address, uint8_t register_address, uint8_t byte, uint8_t write_mask);
void spl07_read_cal_coefs(void);
void spl07_print_cal_coefs(void);

void spl07_init(void)
{
    spl07_read_cal_coefs();

    uint8_t res = 0;
    
    // Configure pressure measurement PRS_CFG
    res = i2c_write_and_verify_byte(SPL07_CHIP_ADDR, SPL07_PRS_CFG_ADDR, PRS_4_SPS_64X_OVERSAMPLING, 0xFF);
    if (res==I2C_FAILURE) radio_print_debug("Failure writing and verifying SPL07_PRS_CFG_ADDR\r\n");

    // Configure temperature measurement TMP_CFG
    res = i2c_write_and_verify_byte(SPL07_CHIP_ADDR, SPL07_TMP_CFG_ADDR, TMP_4_SPS_1X_OVERSAMPLING, 0xFF);
    if (res==I2C_FAILURE) radio_print_debug("Failure writing and verifying SPL07_TMP_CFG_ADDR\r\n");

    // Int and FIFO config
    res = i2c_write_and_verify_byte(SPL07_CHIP_ADDR, SPL07_CFG_ADDR, P_SHIFT_EN, 0xFF); // All disable
    if (res==I2C_FAILURE) radio_print_debug("Failure writing and verifying SPL07_CFG_ADDR\r\n");

    // Set measurement mode MEAS_CFG
    res = i2c_write_and_verify_byte(SPL07_CHIP_ADDR, SPL07_MEAS_CFG_ADDR, CONTINUOUS_PRESS_TEMP, SPL07_MEAS_CFG_W_MASK);
    if (res==I2C_FAILURE) radio_print_debug("Failure writing and verifying SPL07_MEAS_CFG_ADDR\r\n");

}

void spl07_print_cal_coefs(void)
{
    char buff[256];
    sprintf(buff, "c0: %f\r\n", (float)baroState.calib.c0);
    radio_print_debug(buff);
    sprintf(buff, "c1: %f\r\n", baroState.calib.c1);
    radio_print_debug(buff);
    sprintf(buff, "c00: %f\r\n", baroState.calib.c00);
    radio_print_debug(buff);
    sprintf(buff, "c10: %f\r\n", baroState.calib.c10);
    radio_print_debug(buff);
    sprintf(buff, "c01: %f\r\n", baroState.calib.c01);
    radio_print_debug(buff);
    sprintf(buff, "c11: %f\r\n", baroState.calib.c11);
    radio_print_debug(buff);
    sprintf(buff, "c20: %f\r\n", baroState.calib.c20);
    radio_print_debug(buff);
    sprintf(buff, "c21: %f\r\n", baroState.calib.c21);
    radio_print_debug(buff);
    sprintf(buff, "c30: %f\r\n", baroState.calib.c30);
    radio_print_debug(buff);
    sprintf(buff, "c31: %f\r\n", baroState.calib.c31);
    radio_print_debug(buff);
    sprintf(buff, "c40: %f\r\n", baroState.calib.c40);
    radio_print_debug(buff);
}

void spl07_read_cal_coefs(void)
{
    // First extract all the registers
    #define SPL07_C0_REG_ADDR 0x10
    uint8_t coef_arr[NUM_COEF_REGS];

    for (int i=0; i<NUM_COEF_REGS; i++)
    {
        i2c_read(SPL07_CHIP_ADDR, SPL07_C0_REG_ADDR + i, &coef_arr[i], 1);
    }
    // Now sort them into their coef vals, see section 7.11 of datasheet

    // 0x11 c0 [3:0] + 0x10 c0 [11:4]
    baroState.calib.c0 = (float)getTwosComplement(((uint32_t)coef_arr[0] << 4) | (((uint32_t)coef_arr[1] >> 4) & 0x0F), 12);

    // 0x11 c1 [11:8] + 0x12 c1 [7:0]
    baroState.calib.c1 = (float)getTwosComplement((((uint32_t)coef_arr[1] & 0x0F) << 8) | (uint32_t)coef_arr[2], 12);

    // 0x13 c00 [19:12] + 0x14 c00 [11:4] + 0x15 c00 [3:0]
    baroState.calib.c00 = (float)getTwosComplement(((uint32_t)coef_arr[3] << 12) | ((uint32_t)coef_arr[4] << 4) | (((uint32_t)coef_arr[5] >> 4) & 0x0F), 20);

    // 0x15 c10 [19:16] + 0x16 c10 [15:8] + 0x17 c10 [7:0]
    baroState.calib.c10 = (float)getTwosComplement((((uint32_t)coef_arr[5] & 0x0F) << 16) | ((uint32_t)coef_arr[6] << 8) | (uint32_t)coef_arr[7], 20);

    // 0x18 c01 [15:8] + 0x19 c01 [7:0]
    baroState.calib.c01 = (float)getTwosComplement(((uint32_t)coef_arr[8] << 8) | (uint32_t)coef_arr[9], 16);

    // 0x1A c11 [15:8] + 0x1B c11 [7:0]
    baroState.calib.c11 = (float)getTwosComplement(((uint32_t)coef_arr[10] << 8) | (uint32_t)coef_arr[11], 16);

    // 0x1C c20 [15:8] + 0x1D c20 [7:0]
    baroState.calib.c20 = (float)getTwosComplement(((uint32_t)coef_arr[12] << 8) | (uint32_t)coef_arr[13], 16);

    // 0x1E c21 [15:8] + 0x1F c21 [7:0]
    baroState.calib.c21 = (float)getTwosComplement(((uint32_t)coef_arr[14] << 8) | (uint32_t)coef_arr[15], 16);

    // 0x20 c30 [15:8] + 0x21 c30 [7:0]
    baroState.calib.c30 = (float)getTwosComplement(((uint32_t)coef_arr[16] << 8) | (uint32_t)coef_arr[17], 16);

    // 0x23 c31 [3:0] + 0x22 c31 [11:4]
    baroState.calib.c31 = (float)getTwosComplement(((uint32_t)coef_arr[18] << 4) | (((uint32_t)coef_arr[19] >> 4) & 0x0F), 12);

    // 0x23 c40 [11:8] + 0x24 c40 [7:0]
    baroState.calib.c40 = (float)getTwosComplement((((uint32_t)coef_arr[19] & 0x0F) << 8) | (uint32_t)coef_arr[20], 12);
}

void spl07_update_baro(void)
{
    char pbuf[256]; // Debugging
    // Choose compensation scale factors kT (for temperature) and kP (for pressure) based on the chosen precision rate.
    // The scaling factors are listed in Table 9 of the datasheet, and the compensation factors are in table 4.
    float kP = 1040384.0f; // 64 times
    float kT = 524288.0f; // 1 times

    uint8_t buf[NUM_PRESSURE_AND_TEMP_BYTES];
    i2c_read(SPL07_CHIP_ADDR, SPL07_PRS_B2_ADDR, buf, NUM_PRESSURE_AND_TEMP_BYTES);

    // Read the pressure and temperature result from the registers
    int32_t Praw = getTwosComplement(((uint32_t)buf[0] << 16) + ((uint32_t)buf[1] << 8) + (uint32_t)buf[2], 24);
    int32_t Traw = getTwosComplement(((uint32_t)buf[3] << 16) + ((uint32_t)buf[4] << 8) + (uint32_t)buf[5], 24);

    // Calculate scaled measurement results.
    float Praw_sc = Praw / kP;
    float Traw_sc = Traw / kT;

    // Calculate compensated measurement results
    float c0 = baroState.calib.c0;
    float c1 = baroState.calib.c1;
    float c00 = baroState.calib.c00;
    float c01 = baroState.calib.c01;
    float c10 = baroState.calib.c10;
    float c11 = baroState.calib.c11;
    float c20 = baroState.calib.c20;
    float c21 = baroState.calib.c21;
    float c30 = baroState.calib.c30;
    float c31 = baroState.calib.c31;
    float c40 = baroState.calib.c40;

    // See section 4.6 Calibration and Measurement Compensation of datasheet
    baroState.pressure = c00 + Praw_sc * (c10 + Praw_sc * (c20 + Praw_sc * (c30 + Praw_sc * c40))) + Traw_sc * c01 + Traw_sc * Praw_sc * (c11 + Praw_sc * (c21 + Praw_sc * c31));
    baroState.temperature = c0 * 0.5f + c1 * Traw_sc;

    
    sprintf(pbuf, "Pressure = %f, Temp = %f\r\n", baroState.pressure, baroState.temperature);
    radio_print_debug(pbuf);
}

float get_baro_pressure(void)
{
    return baroState.pressure;
}