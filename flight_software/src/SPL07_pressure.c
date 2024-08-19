// https://github.com/betaflight/betaflight/blob/6dcc2689182aa22707dce16448c57b40af9dd957/src/main/drivers/barometer/barometer_dps310.c#L73

#include "SPL07_pressure.h"
#include "common.h"

#define SPL07_CHIP_ADDR (0x77 << 1) // Left shifted by 1 as SPL expects it in this form

#define SPL07_MEAS_CFG_ADDR 0x08
#define SPL07_MEAS_CFG_W_MASK 0x07
#define SPL07_PRS_CFG_ADDR 0x06
#define SPL07_TMP_CFG_ADDR 0x07
#define SPL07_PRS_B2_ADDR 0x00

#define CONTINUOUS_PRESS_TEMP 0x07
#define PRS_16_SPS_8X_OVERSAMPLING 0x01000011
#define TMP_16_SPS_1X_OVERSAMPLING 0x01000000

#define NUM_PRESSURE_BYTES 3
#define NUM_TEMP_BYTES 3
#define NUM_COEF_REGS 21

uint8_t coef_arr[NUM_COEF_REGS];

void i2c_write_byte(uint8_t device_address, uint8_t register_address, uint8_t byte);
void i2c_read(uint8_t device_address, uint8_t register_address, uint8_t bytes[], uint8_t num_bytes);
uint8_t i2c_write_and_verify_byte(uint8_t device_address, uint8_t register_address, uint8_t byte, uint8_t write_mask);
void spl07_read_cal_coefs(void);

void spl07_init(void)
{
    I2C_Cmd(ENABLE);
    I2C_Init(80000, 0x69, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, F_CPU/1000000);

    spl07_read_cal_coefs();
    for (int i=0; i<NUM_COEF_REGS; i++)
    {
        print_bits_of_byte(coef_arr[i]);
        radio_print("\r\n");
    }
    while(1) continue;

    uint8_t res = 0;
    
    // Configure pressure measurement PRS_CFG
    res = i2c_write_and_verify_byte(SPL07_CHIP_ADDR, SPL07_PRS_CFG_ADDR, PRS_16_SPS_8X_OVERSAMPLING, 0xFF);
    if (res==I2C_FAILURE) radio_print("Failure writing and verifying byte\r\n");

    // Configure temperature measurement TMP_CFG
    res = i2c_write_and_verify_byte(SPL07_CHIP_ADDR, SPL07_TMP_CFG_ADDR, TMP_16_SPS_1X_OVERSAMPLING, 0xFF);
    if (res==I2C_FAILURE) radio_print("Failure writing and verifying byte\r\n");

    // Set measurement mode MEAS_CFG
    res = i2c_write_and_verify_byte(SPL07_CHIP_ADDR, SPL07_MEAS_CFG_ADDR, CONTINUOUS_PRESS_TEMP, SPL07_MEAS_CFG_W_MASK);
    if (res==I2C_FAILURE) radio_print("Failure writing and verifying byte\r\n");
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
    #define SPL07_C0_REG_ADDR 0x10
    for (int i=0; i<NUM_COEF_REGS; i++)
    {
        //uint8_t temp_byte = 0;
        i2c_read(SPL07_CHIP_ADDR, SPL07_C0_REG_ADDR + i, &coef_arr[i], 1);
        //coef_arr[i] = temp_byte;
    }
}

int spl07_read_pressure()
{
    uint8_t bytes[NUM_PRESSURE_BYTES];
    i2c_read(SPL07_CHIP_ADDR, SPL07_PRS_B2_ADDR, bytes, NUM_PRESSURE_BYTES);
    print_bits_of_byte(bytes[0]);
    print_bits_of_byte(bytes[1]);
    print_bits_of_byte(bytes[2]);

    int press_raw = getTwosComplement(((uint32_t)bytes[0] << 16) | ((uint32_t)bytes[1] << 8) | (uint32_t)bytes[2], 24);
    char buff[256];
    sprintf(buff, "  Pressure = %d\r\n", press_raw);
    radio_print(buff);
}