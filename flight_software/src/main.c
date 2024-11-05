#include "common.h"
#include "gpio.h"
#include "ebyte_radio.h"
#include "SPL07_pressure.h"
#include "YIC_gps.h"
#include "i2c_driver_STM8S007.h"
#include "ICM42670_imu.h"
#include "microsd.h"
#include "inttypes.h"
#include "logging.h"
// #include "MAX31855_therm.h"
#include "battery.h"

void assert_failed(uint8_t* file, uint32_t line);
void clock_config(void);
uint8_t spi_read_byte(void);

#define MAIN_LOOP_FREQ_HZ 20 // 20 mins logging 200 chars @ 50 Hz is around 12 mB.

void main(void)
{
  clock_config();
  GPIO_Init(GREEN_LED_PORT, GREEN_LED_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(RED_LED_PORT, RED_LED_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(MSD_CS_PORT, MSD_CS_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(M0_RADIO_PORT, M0_RADIO_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(M1_RADIO_PORT, M1_RADIO_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  radio_uart_init();

  radio_print_debug("Radio initialized\r\n");

  init_battery_measurements();
  i2c_init();
  imu_init(); // Needs i2c_init called first
  spl07_init(); // Needs i2c_init called first
  gps_init();

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
  radio_print_debug("SPI initialized\r\n");

  thermo_init(); // Needs SPI setup first.

  //MSD_Init();
  // radio_print_debug("SD initialized\r\n");

  // print_sd_to_radio();
  //add_data_to_log_buff();
  //test_log_buff();

  // char bytes[512];
  // for (int i = 0; i < 512; i++)
  // {
  //   bytes[i] = 0xFF;
  // }
  // MSD_WriteBlock(bytes, 1024, 512);

  // char bytes_read[512];
  // MSD_ReadBlock(bytes_read, 1024, 512);

  // char pbuff[10];
  // for (int i=0; i<255; i++)
  // {
  //   sprintf(pbuff, "%d\r\n", (int)bytes_read[i]);
  //   radio_print_debug(pbuff);
  // }

  uint32_t loop_start_time;
  uint32_t current_time;
  while(1)
  {
    GPIO_WriteReverse(GREEN_LED_PORT, GREEN_LED_PIN);
  

    read_and_convert_thermo_value();
    // read_gps_buffer();
    // update_imu_state();
    // spl07_update_baro();
    // send_telemetry();
    delay_ms(250); // Needs some delay
  }


  
  while (1){
    GPIO_WriteReverse(GREEN_LED_PORT, GREEN_LED_PIN);

    // Read all sensors
    read_gps_buffer();
    update_imu_state();
    spl07_update_baro();
    read_batt_voltage();
    read_batt_current();
    float batt_voltage_V = get_batt_voltage();
    float batt_current_mA = get_batt_current();

    // Log to SD
    send_telemetry();
    
    // Paced loop, wait until time to continue
    current_time = millis();
    while(current_time - loop_start_time < (1000 / MAIN_LOOP_FREQ_HZ))
    {
      current_time = millis();
    }
  }
}

void clock_config(void)
{
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
  TIM4_Config();
}

void print_bits_of_byte(uint8_t byte)
{
  char bits[9];
  for (int i = 0; i < 8; i++)
  {
    bits[i] = (byte & (1 << (7-i))) ? '1' : '0';
  }
  bits[8] = '\0';
  radio_print_debug(bits);
}

uint8_t spi_read_byte(void)
{
  /* Wait until the transmit buffer is empty */
    while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET);
    SPI_SendData(0xFF);
    /* Wait to receive a byte*/
    while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
    return SPI_ReceiveData();
}

int32_t getTwosComplement(uint32_t raw, uint8_t length) {
  uint32_t comparison = (uint32_t)1<<(length - 1);
  int32_t result = 0;

  // Check if the sign bit is set
  if (raw & comparison) {
      // If the sign bit is set, convert to two's complement
      uint32_t mask = ((uint32_t)1 << length) - 1; // inverting mask
      raw = (raw ^ mask) + 1; // Invert each bit then add one
      result = -1 * (int32_t)raw; // Make it negative
  } else {
      // If the sign bit is not set, it's already a positive value
      result = (int32_t)raw;
  }
  return result;
}

// THERMOCOUPLE STUFF:
void thermo_init(void)
{
    // Thermocouple pins:
    GPIO_Init(THERMO_CHIP_PORT, THERMO_CHIP_PIN_SELECT, GPIO_MODE_OUT_PP_HIGH_FAST);

    radio_print_debug("Thermocouple initialized\r\n");
}

float read_and_convert_thermo_value()
{
    // "Drive CS low and apply a clock signal at SCK to read the results at SO."
    // Drive CS low:
    GPIO_WriteLow(THERMO_CHIP_PORT, THERMO_CHIP_PIN_SELECT);

    // to store the 4x 8bit reads from thermocouple SPI
    uint8_t thermocouple_data_buff[4];
    for (int i=0; i<4; i++)
    {
        thermocouple_data_buff[i] = spi_read_byte();
    }

    // We cast the uint8s to a 32 bit int. MSB first from IC so order as below
    uint32_t thermocouple_32 = ((uint32_t)thermocouple_data_buff[3]) | ((uint32_t)thermocouple_data_buff[2] << 8) | ((uint32_t)thermocouple_data_buff[1] << 16) | ((uint32_t)thermocouple_data_buff[0] << 24);

    // Now we try a bit mask:
    uint32_t thermocouple_temp = (thermocouple_32 >> 18) & 0x3FFF;

    int32_t temp = getTwosComplement(thermocouple_temp, 14);

    float temp_f = (float)temp / 4.0 ;

    char pbuff[100];
    sprintf(pbuff, "Thermocouple temperature: %" PRId32 " %f\r\n", temp, temp_f);
    radio_print_debug(pbuff);

    // Finish the read.
    GPIO_WriteHigh(THERMO_CHIP_PORT, THERMO_CHIP_PIN_SELECT);

    return temp_f;
}


#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
    char buff[1000];
    #ifdef DEBUG_ENABLE
    sprintf(buff, "Wrong parameters value: file %s on line %d\r\n", file, line);
    radio_print_debug(buff);
    #endif
    GPIO_WriteReverse(GREEN_LED_PORT, GREEN_LED_PIN);
    delay_ms(2000);
  }
}
#endif