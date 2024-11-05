// // #include "MAX31855_therm.h"
// #include "inttypes.h"
// #include "gpio.h"

// static thermocouple_state_t thermo_state;
// typedef struct {
//     float   thermocouple_reading_c;
// } thermocouple_state_t;

// void thermo_init(void)
// {
//     // Thermocouple pins:
//     GPIO_Init(THERMO_CHIP_PORT, THERMO_CHIP_PIN_SELECT, GPIO_MODE_OUT_PP_HIGH_FAST);

//     radio_print_debug("Thermocouple initialized\r\n");
// }

// float read_and_convert_thermo_value()
// {
//     // "Drive CS low and apply a clock signal at SCK to read the results at SO."
//     // Drive CS low:
//     GPIO_WriteLow(THERMO_CHIP_PORT, THERMO_CHIP_PIN_SELECT);

//     // to store the 4x 8bit reads from thermocouple SPI
//     uint8_t thermocouple_data_buff[4];
//     for (int i=0; i<4; i++)
//     {
//         thermocouple_data_buff[i] = spi_read_byte();
//     }

//     // We cast the uint8s to a 32 bit int. MSB first from IC so order as below
//     uint32_t thermocouple_32 = ((uint32_t)thermocouple_data_buff[3]) | ((uint32_t)thermocouple_data_buff[2] << 8) | ((uint32_t)thermocouple_data_buff[1] << 16) | ((uint32_t)thermocouple_data_buff[0] << 24);

//     // Now we try a bit mask:
//     uint32_t thermocouple_temp = (thermocouple_32 >> 18) & 0x3FFF;

//     int32_t temp = getTwosComplement(thermocouple_temp, 14);

//     float temp_f = (float)temp / 4.0 ;

//     char pbuff[100];
//     sprintf(pbuff, "Thermocouple temperature: %" PRId32 " %f\r\n", temp, temp_f);
//     radio_print_debug(pbuff);
//     radio_print_debug("\r\n");

//     // Finish the read.
//     GPIO_WriteHigh(THERMO_CHIP_PORT, THERMO_CHIP_PIN_SELECT);

//     return temp_f;
// }

// void print_thermo_data(void)
// {
//     #ifdef DEBUG_ENABLE
//     char pbuff[100];
//     sprintf(pbuff, "Thermocouple temperature: %f\r\n", thermo_state.thermocouple_reading_c);
//     radio_print_debug(pbuff);
//     #endif
// }


// void update_thermo_state(void)
// {
//     thermo_state.thermocouple_reading_c = read_and_convert_thermo_value();
// }


// float get_thermo_state(void)
// {
//     return thermo_state.thermocouple_reading_c;
// }
