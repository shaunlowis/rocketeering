#include "SPL07_pressure.h"
#include "common.h"

#define SPL07_CHIP_ADDR (0x77 << 1) // Left shifted by 1 as SPL expects it in this form

#define SPL07_MEAS_CFG_ADDR 0x08

void spl07_init(void)
{
    I2C_Cmd(ENABLE);
    I2C_Init(80000, 0x69, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, F_CPU/1000000);
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