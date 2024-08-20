// https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/4843/SPL07-003.pdf 
// https://github.com/betaflight/betaflight/blob/6dcc2689182aa22707dce16448c57b40af9dd957/src/main/drivers/barometer/barometer_dps310.c#L73

#include "SPL07_pressure.h"
#include "common.h"
#include <inttypes.h>

#define SPL07_CHIP_ADDR (0x77 << 1) // Left shifted by 1 as SPL expects it in this form


#define SPL07_PRS_B2_ADDR 0x00
#define SPL07_PRS_CFG_ADDR 0x06
#define SPL07_TMP_CFG_ADDR 0x07
#define SPL07_MEAS_CFG_ADDR 0x08
#define SPL07_CFG_ADDR 0x09

#define SPL07_MEAS_CFG_W_MASK 0x07


#define CONTINUOUS_PRESS_TEMP 0x07
#define PRS_16_SPS_8X_OVERSAMPLING 0x01000011
#define TMP_16_SPS_1X_OVERSAMPLING 0x01000000

#define NUM_PRESSURE_AND_TEMP_BYTES 6
#define NUM_COEF_REGS 21


typedef struct {
    int16_t c0;     // 12bit
    int16_t c1;     // 12bit
    int32_t c00;    // 20bit
    int32_t c10;    // 20bit
    int16_t c01;    // 16bit
    int16_t c11;    // 16bit
    int16_t c20;    // 16bit
    int16_t c21;    // 16bit
    int16_t c30;    // 16bit
    int16_t c31;    // 12bit
    int16_t c40;    // 12bit
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
    I2C_Cmd(ENABLE);
    I2C_Init(80000, 0x69, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, F_CPU/1000000);

    spl07_read_cal_coefs();

    uint8_t res = 0;
    
    // Configure pressure measurement PRS_CFG
    res = i2c_write_and_verify_byte(SPL07_CHIP_ADDR, SPL07_PRS_CFG_ADDR, PRS_16_SPS_8X_OVERSAMPLING, 0xFF);
    if (res==I2C_FAILURE) radio_print("Failure writing and verifying SPL07_PRS_CFG_ADDR\r\n");

    // Configure temperature measurement TMP_CFG
    res = i2c_write_and_verify_byte(SPL07_CHIP_ADDR, SPL07_TMP_CFG_ADDR, TMP_16_SPS_1X_OVERSAMPLING, 0xFF);
    if (res==I2C_FAILURE) radio_print("Failure writing and verifying SPL07_TMP_CFG_ADDR\r\n");

    // Int and FIFO config
    res = i2c_write_and_verify_byte(SPL07_CHIP_ADDR, SPL07_CFG_ADDR, 0x00, 0xFF); // All disable
    if (res==I2C_FAILURE) radio_print("Failure writing and verifying SPL07_CFG_ADDR\r\n");

    // Set measurement mode MEAS_CFG
    res = i2c_write_and_verify_byte(SPL07_CHIP_ADDR, SPL07_MEAS_CFG_ADDR, CONTINUOUS_PRESS_TEMP, SPL07_MEAS_CFG_W_MASK);
    if (res==I2C_FAILURE) radio_print("Failure writing and verifying SPL07_MEAS_CFG_ADDR\r\n");
}

uint8_t i2c_write_and_verify_byte(uint8_t device_address, uint8_t register_address, uint8_t byte, uint8_t write_mask)
{
    i2c_write_byte(device_address, register_address, byte);
    uint8_t ret_val[1];
    i2c_read(device_address, register_address, ret_val, 1);

    for (int i=0; i<8; i++)
    {   
        if ((write_mask >> i) & 1) // Need to check the bit
        {
            uint8_t bit_mask = 1<<i;
            if ((ret_val[0] & bit_mask) != (byte & bit_mask))
            {
                return I2C_FAILURE;
            }
        }
    }

    return I2C_SUCCESS;

}

void i2c_read(uint8_t device_address, uint8_t register_address, uint8_t bytes[], uint8_t num_bytes)
{
    if (num_bytes > 1) I2C_AcknowledgeConfig(I2C_ACK_CURR); // Need to ack the bytes recieved so that we keep reading;
    else I2C_AcknowledgeConfig(I2C_ACK_NONE); // Send a nack on next byte recieved

    // TODO: add timeouts to whiles
    I2C_GenerateSTART(ENABLE);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)) continue;

    //radio_print("Addr W\r\n");
    I2C_Send7bitAddress(device_address, I2C_DIRECTION_TX);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) continue;

    //radio_print("Data\r\n");
    I2C_SendData(register_address);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) continue;
    
    //radio_print("Repeated start\r\n");
    I2C_GenerateSTART(ENABLE);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)) continue;

    //radio_print("Addr R\r\n");
    I2C_Send7bitAddress(device_address, I2C_DIRECTION_RX);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) continue;

    // Read bytes with ack (if reading multiple bytes)
    for (int i=0; i<num_bytes - 1; i++) // -1 as we must NACK the last byte
    {
        while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)) continue;
        bytes[i] = I2C_ReceiveData();
    }

    // Nack to next byte recieved
    I2C_AcknowledgeConfig(I2C_ACK_NONE); // Send a nack on next byte recieved to let slaveknow we are done
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)) continue;
    bytes[num_bytes-1] = I2C_ReceiveData();
    
    I2C_GenerateSTOP(ENABLE);
}

void i2c_write_byte(uint8_t device_address, uint8_t register_address, uint8_t byte)
{
    // TODO: add timeouts to whiles
    I2C_GenerateSTART(ENABLE);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)) continue;

    I2C_Send7bitAddress(device_address, I2C_DIRECTION_TX);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) continue;

    //radio_print("Data\r\n");
    I2C_SendData(register_address);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) continue;

    //radio_print("Data\r\n");
    I2C_SendData(byte);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) continue;

    I2C_GenerateSTOP(ENABLE);
}

void spl07_print_cal_coefs(void)
{
    char buff[256];
    sprintf(buff, "c0: %f\r\n", (float)baroState.calib.c0);
    radio_print(buff);
    sprintf(buff, "c1: %" PRId16 "\r\n", baroState.calib.c1);
    radio_print(buff);
    sprintf(buff, "c00: %" PRId32 "\r\n", baroState.calib.c00);
    radio_print(buff);
    sprintf(buff, "c10: %" PRId32 "\r\n", baroState.calib.c10);
    radio_print(buff);
    sprintf(buff, "c01: %" PRId16 "\r\n", baroState.calib.c01);
    radio_print(buff);
    sprintf(buff, "c11: %" PRId16 "\r\n", baroState.calib.c11);
    radio_print(buff);
    sprintf(buff, "c20: %" PRId16 "\r\n", baroState.calib.c20);
    radio_print(buff);
    sprintf(buff, "c21: %" PRId16 "\r\n", baroState.calib.c21);
    radio_print(buff);
    sprintf(buff, "c30: %" PRId16 "\r\n", baroState.calib.c30);
    radio_print(buff);
    sprintf(buff, "c31: %" PRId16 "\r\n", baroState.calib.c31);
    radio_print(buff);
    sprintf(buff, "c40: %" PRId16 "\r\n", baroState.calib.c40);
    radio_print(buff);
}


void spl07_test(void)
{  
    //radio_print("Start\r\n");
    uint8_t bytes[] = {0xff, 0xff};
    i2c_read(SPL07_CHIP_ADDR, SPL07_MEAS_CFG_ADDR, bytes, 1);
    radio_print("Retval bits = ");
    print_bits_of_byte(bytes[0]);
    print_bits_of_byte(bytes[1]);
    radio_print("\r\n");    
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
    baroState.calib.c0 = getTwosComplement(((uint32_t)coef_arr[0] << 4) | (((uint32_t)coef_arr[1] >> 4) & 0x0F), 12);

    // 0x11 c1 [11:8] + 0x12 c1 [7:0]
    baroState.calib.c1 = getTwosComplement((((uint32_t)coef_arr[1] & 0x0F) << 8) | (uint32_t)coef_arr[2], 12);

    // 0x13 c00 [19:12] + 0x14 c00 [11:4] + 0x15 c00 [3:0]
    baroState.calib.c00 = getTwosComplement(((uint32_t)coef_arr[3] << 12) | ((uint32_t)coef_arr[4] << 4) | (((uint32_t)coef_arr[5] >> 4) & 0x0F), 20);

    // 0x15 c10 [19:16] + 0x16 c10 [15:8] + 0x17 c10 [7:0]
    baroState.calib.c10 = getTwosComplement((((uint32_t)coef_arr[5] & 0x0F) << 16) | ((uint32_t)coef_arr[6] << 8) | (uint32_t)coef_arr[7], 20);

    // 0x18 c01 [15:8] + 0x19 c01 [7:0]
    baroState.calib.c01 = getTwosComplement(((uint32_t)coef_arr[8] << 8) | (uint32_t)coef_arr[9], 16);

    // 0x1A c11 [15:8] + 0x1B c11 [7:0]
    baroState.calib.c11 = getTwosComplement(((uint32_t)coef_arr[10] << 8) | (uint32_t)coef_arr[11], 16);

    // 0x1C c20 [15:8] + 0x1D c20 [7:0]
    baroState.calib.c20 = getTwosComplement(((uint32_t)coef_arr[12] << 8) | (uint32_t)coef_arr[13], 16);

    // 0x1E c21 [15:8] + 0x1F c21 [7:0]
    baroState.calib.c21 = getTwosComplement(((uint32_t)coef_arr[14] << 8) | (uint32_t)coef_arr[15], 16);

    // 0x20 c30 [15:8] + 0x21 c30 [7:0]
    baroState.calib.c30 = getTwosComplement(((uint32_t)coef_arr[16] << 8) | (uint32_t)coef_arr[17], 16);

    // 0x23 c31 [3:0] + 0x22 c31 [11:4]
    baroState.calib.c31 = getTwosComplement(((uint32_t)coef_arr[18] << 4) | (((uint32_t)coef_arr[19] >> 4) & 0x0F), 12);

    // 0x23 c40 [11:8] + 0x24 c40 [7:0]
    baroState.calib.c40 = getTwosComplement((((uint32_t)coef_arr[19] & 0x0F) << 8) | (uint32_t)coef_arr[20], 12);

    spl07_print_cal_coefs();
}

void spl07_update_baro()
{
    // Choose compensation scale factors kT (for temperature) and kP (for pressure) based on the chosen precision rate.
    // The scaling factors are listed in Table 9 of the datasheet, and the compensation factors are in table 4.
    static float kP = 7864320; // 8 times
    static float kT = 524288; // 1 times

    uint8_t buf[NUM_PRESSURE_AND_TEMP_BYTES];
    i2c_read(SPL07_CHIP_ADDR, SPL07_PRS_B2_ADDR, buf, NUM_PRESSURE_AND_TEMP_BYTES);

    // Read the pressure and temperature result from the registers
    const int32_t Praw = getTwosComplement((buf[0] << 16) + (buf[1] << 8) + buf[2], 24);
    const int32_t Traw = getTwosComplement((buf[3] << 16) + (buf[4] << 8) + buf[5], 24);

    // Calculate scaled measurement results.
    const float Praw_sc = Praw / kP;
    const float Traw_sc = Traw / kT;

    // Calculate compensated measurement results
    const float c0 = baroState.calib.c0;
    const float c1 = baroState.calib.c1;
    const float c00 = baroState.calib.c00;
    const float c01 = baroState.calib.c01;
    const float c10 = baroState.calib.c10;
    const float c11 = baroState.calib.c11;
    const float c20 = baroState.calib.c20;
    const float c21 = baroState.calib.c21;
    const float c30 = baroState.calib.c30;
    const float c31 = baroState.calib.c31;
    const float c40 = baroState.calib.c40;

    // See section 4.6 Calibration and Measurement Compensation of datasheet
    baroState.pressure = c00 + Praw_sc * (c10 + Praw_sc * (c20 + Praw_sc * (c30 + Praw_sc * c40))) + Traw_sc * c01 + Traw_sc * Praw_sc * (c11 + Praw_sc * (c21 + Praw_sc * c31));
    baroState.temperature = c0 * 0.5f + c1 * Traw_sc;

    char pbuf[256];
    sprintf(pbuf, "Pressure = %f, Temp = %f\r\n", baroState.pressure, baroState.temperature);
    radio_print(pbuf);

}