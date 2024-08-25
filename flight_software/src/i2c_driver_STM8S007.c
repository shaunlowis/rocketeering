#include "common.h"
#include "i2c_driver_STM8S007.h"

void i2c_init(void)
{
    I2C_Cmd(ENABLE);
    I2C_Init(80000, 0x69, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, F_CPU/1000000);
}

uint8_t i2c_write_and_verify_byte(uint8_t device_address, uint8_t register_address, uint8_t byte, uint8_t write_mask)
{
    i2c_write_byte(device_address, register_address, byte);
    uint8_t ret_val[1];
    i2c_read(device_address, register_address, ret_val, 1);
    char buff[100];
    // sprintf(buff, "Register 0x%x has value = ", register_address);
    radio_print_debug(buff);
    print_bits_of_byte(ret_val[0]);
    radio_print_debug("\r\n");

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

    //radio_print_debug("Addr W\r\n");
    I2C_Send7bitAddress(device_address, I2C_DIRECTION_TX);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) continue;

    //radio_print_debug("Data\r\n");
    I2C_SendData(register_address);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) continue;
    
    //radio_print_debug("Repeated start\r\n");
    I2C_GenerateSTART(ENABLE);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)) continue;

    //radio_print_debug("Addr R\r\n");
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
    radio_print_debug("Start\r\n");
    I2C_GenerateSTART(ENABLE);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)) continue;

    radio_print_debug("Chip addr\r\n");
    I2C_Send7bitAddress(device_address, I2C_DIRECTION_TX);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) continue;

    radio_print_debug("Reg addr\r\n");
    I2C_SendData(register_address);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) continue;

    radio_print_debug("Data\r\n");
    I2C_SendData(byte);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) continue;

    radio_print_debug("Stop\r\n");
    I2C_GenerateSTOP(ENABLE);
}