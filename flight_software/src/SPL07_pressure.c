#include "SPL07_pressure.h"
#include "common.h"

#define SPL07_CHIP_ADDR 0x77

#define SPL07_MEAS_CFG_ADDR 0x08

void spl07_init(void)
{
    I2C_Cmd(ENABLE);
    I2C_Init(80000, 0x69, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, F_CPU/1000000);
}

void spl07_test(void)
{

    char buff1[] = "Here1\r\n";
    char buff2[] = "Here2\r\n";
    char buff3[] = "Here3\r\n";
    char buff4[] = "Here4\r\n";
    
    radio_transmit_string(buff1, sizeof(buff1)/sizeof(buff1[0]));
    I2C_GenerateSTART(ENABLE);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT)) continue;

    radio_transmit_string(buff2, sizeof(buff2)/sizeof(buff2[0]));
    I2C_Send7bitAddress(SPL07_CHIP_ADDR, I2C_DIRECTION_TX);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) continue;

    radio_transmit_string(buff3, sizeof(buff3)/sizeof(buff3[0]));
    I2C_SendData(SPL07_MEAS_CFG_ADDR);
    while(!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) continue;
    radio_transmit_string(buff4, sizeof(buff4)/sizeof(buff4[0]));
}