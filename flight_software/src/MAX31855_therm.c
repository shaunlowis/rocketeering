#include "MAX31855_therm.h"

#define MAX31855_TC_BITS_PER_DEGREE 4.0f
#define MAX31855_CJC_BITS_PER_DEGREE 16.0f

static uint32_t read_thermo_reg(void);
static uint8_t spi_read_byte(void);
static void print_thermo_state(void);

static thermocoupleState_t thermo_state = {.tc_temp_C=0.0, 
                                           .cjc_temp_C=0.0, 
                                           .oc_sc_flags=0};

// THERMOCOUPLE STUFF:
void thermo_init(void)
{
    // Thermocouple pins:
    GPIO_Init(THERMO_CHIP_PORT, THERMO_CHIP_PIN_SELECT, GPIO_MODE_OUT_PP_HIGH_FAST);

    // SPI setup:
    SPI_DeInit();
    SPI_Init(
    SPI_FIRSTBIT_MSB,
    SPI_BAUDRATEPRESCALER_2, // unsure if correct..
    SPI_MODE_MASTER,
    SPI_CLOCKPOLARITY_LOW,
    SPI_CLOCKPHASE_1EDGE, // verify with timing diagram
    SPI_DATADIRECTION_2LINES_FULLDUPLEX, // We just need to add a clock at SCK, then read SO.
    SPI_NSS_SOFT, // Dont think this is needed?
    1);
    SPI_Cmd(ENABLE);

    radio_print_debug("Thermocouple initialized\r\n");
}

static uint8_t spi_read_byte(void)
{
  /* Wait until the transmit buffer is empty */
    while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET);
    SPI_SendData(0xFF);
    /* Wait to receive a byte*/
    while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
    return SPI_ReceiveData();
}

static uint32_t read_thermo_reg(void)
{
    // Drive CS low:
    GPIO_WriteLow(THERMO_CHIP_PORT, THERMO_CHIP_PIN_SELECT);

    // to store the 4x 8bit reads from thermocouple SPI
    uint8_t thermocouple_data_buff[4];
    for (int i=0; i<4; i++)
    {
        thermocouple_data_buff[i] = spi_read_byte();
    }
    // Finish the read.
    GPIO_WriteHigh(THERMO_CHIP_PORT, THERMO_CHIP_PIN_SELECT);

    // We cast the uint8s to a 32 bit int. MSB first from IC so order as below
    uint32_t thermocouple_reg_32 = ((uint32_t)thermocouple_data_buff[3]) | ((uint32_t)thermocouple_data_buff[2] << 8) | ((uint32_t)thermocouple_data_buff[1] << 16) | ((uint32_t)thermocouple_data_buff[0] << 24);
    return thermocouple_reg_32;
}

static void print_thermo_state(void)
{
    char pbuf[200];
    sprintf(pbuf, "Thermo temp %.1f CJC temp %.1f Flags %d\r\n", thermo_state.tc_temp_C, thermo_state.cjc_temp_C, thermo_state.oc_sc_flags);
    radio_print_debug(pbuf);
}

void update_thermo_state(void)
{
    // Read the thermocouple register
    uint32_t thermocouple_reg_32 = read_thermo_reg();
    uint32_t thermocouple_reg_32_copy = thermocouple_reg_32;

    // TC temp
    uint32_t thermocouple_temp_reg = (thermocouple_reg_32 >> 18) & 0x3FFF;
    int32_t thermocouple_temp_complimented = getTwosComplement(thermocouple_temp_reg, 14);
    thermo_state.tc_temp_C =  (float)thermocouple_temp_complimented / MAX31855_TC_BITS_PER_DEGREE;

    // CJC temp
    uint32_t thermocouple_cjc_temp = (thermocouple_reg_32 >> 4) & 0xFFF;
    int32_t thermocouple_cjc_temp_complimented = getTwosComplement(thermocouple_cjc_temp, 12);
    thermo_state.cjc_temp_C = (float)thermocouple_cjc_temp_complimented / MAX31855_CJC_BITS_PER_DEGREE;

    // Flags
    thermo_state.oc_sc_flags = thermocouple_reg_32 & 0x07;

    //print_thermo_state();
}

thermocoupleState_t get_thermo_state(void)
{
    return thermo_state;
}
