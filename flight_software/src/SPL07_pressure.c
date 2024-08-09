#include "SPL07_pressure.h"
#include "common.h"

#define SPL07_CHIP_ADDR (0x77 << 1) // Left shifted by 1 as SPL expects it in this form

#define SPL07_MEAS_CFG_ADDR 0x08

void spl07_init(void)
{
    I2C_Cmd(ENABLE);
    I2C_Init(80000, 0x69, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, F_CPU/1000000);
}

void spl07_test(void)
{  
    radio_print("Start\r\n");
    I2C_GenerateSTART(ENABLE);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)) continue;

    radio_print("Addr W\r\n");
    I2C_Send7bitAddress(SPL07_CHIP_ADDR, I2C_DIRECTION_TX);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) continue;

    radio_print("Data\r\n");
    I2C_SendData(SPL07_MEAS_CFG_ADDR);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) continue;
    
    radio_print("Repeated start\r\n");
    I2C_GenerateSTART(ENABLE);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)) continue;

    radio_print("Addr R\r\n");
    I2C_Send7bitAddress(SPL07_CHIP_ADDR, I2C_DIRECTION_RX);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) continue;

    radio_print("Waiting for byte\r\n");
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)) continue;
    radio_print("Byte recieved\r\n");
    uint8_t ret_val = I2C_ReceiveData();
    char buff[256];
    sprintf(buff, "Retval = %d\r\n", ret_val);

    I2C_GenerateSTOP(ENABLE);
}