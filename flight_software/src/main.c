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

void assert_failed(uint8_t* file, uint32_t line);
void clock_config(void);

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

  // i2c_init();
  // imu_init(); // Needs i2c_init called first
  // spl07_init(); // Needs i2c_init called first
  //gps_init();

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

  // Thermocouple pins:
  // "Drive CS low and apply a clock signal at SCK to read the results at SO."
  GPIO_Init(THERMO_CHIP_PORT, THERMO_CHIP_PIN_SELECT, GPIO_MODE_OUT_PP_HIGH_FAST);

  radio_print_debug("Thermocouple initialized\r\n");

  // read_thermocouple();

  // MSD_Init();
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



  
  while(1)
  {
    GPIO_WriteReverse(GREEN_LED_PORT, GREEN_LED_PIN);
    
    // radio_print_debug("peepeepoopoo\r\n");
    // read_thermocouple();

    // Drive CS low:
    GPIO_WriteLow(THERMO_CHIP_PORT, THERMO_CHIP_PIN_SELECT);

    // bit D[31:18] is the 14-bit thermocouple temperature data.

    // to store the 4x 8bit reads from thermocouple SPI
    uint8_t thermocouple_data_buff[4];
    for (int i=0; i<4; i++)
    {
      SPI_SendData(0xFF);
      thermocouple_data_buff[i] = SPI_ReceiveData();

      // Debug print to verify each byte received --> This showed data output:
      // Byte 0: 48
      // Byte 1: 1
      // Byte 2: 240
      // Byte 3: 28

      // char pbuff[100];
      // sprintf(pbuff, "Byte %d: %u\n", i, thermocouple_data_buff[i]);
      // radio_print_debug(pbuff);
      // radio_print_debug("\r\n");
    }

    // Print each byte in thermocouple_data_buff:
    // Byte 0: 0x00
    // Byte 1: 0x01
    // Byte 2: 0xF0
    // Byte 3: 0x1C
    char pbuff[50];
    for (int i = 0; i < 4; i++) {
        sprintf(pbuff, "Byte %d: 0x%02X\r\n", i, thermocouple_data_buff[i]);
        radio_print_debug(pbuff);
    }

    // We cast the uint8s to a 32 bit int:
    uint32_t thermocouple_32 = thermocouple_data_buff[0] | (thermocouple_data_buff[1] << 8) | (thermocouple_data_buff[2] << 16) | (thermocouple_data_buff[3] << 24);

    // Just check the 32 bit int directly
    // char pbuff[50];
    // sprintf(pbuff, "Combined thermocouple_32: 0x%" PRIx32 "\r\n", thermocouple_32);
    // radio_print_debug(pbuff);

    // Now we try a bit mask:
    int16_t thermocouple_temp = (thermocouple_32 >> 18) & 0x3FFF;

    if (thermocouple_temp & 0x2000) {  // Check if the 14th bit (sign bit) is set
    thermocouple_temp |= 0xC000;  // Set the remaining high bits for 16-bit signed extension
    }

    // char pbuff[50];
    // sprintf(pbuff, "Thermocouple temperature: %" PRId16 "\r\n", thermocouple_temp);
    // radio_print_debug(pbuff);
    // radio_print_debug("\r\n");

    // char pbuf[50];
  // sprintf(pbuf, "Raw: %"PRIu32"   |   Length: %"PRIu8"   |   Comp: %"PRIu32"\r\n", raw, length, comparison);
  // radio_print_debug(pbuf);

    // char pbuff[10];
    // for (int i=0; i<255; i++)
    // {
    //   sprintf(pbuff, "%d\r\n", (int)extracted_bits[i]);
    //   radio_print_debug(pbuff);
    // }

    // Then we index our array for the thermocouple reading and print that:
    // char pbuff[100];
    // for (int i=0; i<33; i++)
    // {
    //   if (i > 18 && i < 31)
    //   {
    //     sprintf(pbuff, "%u ", thermocouple_32);
    //     radio_print_debug(pbuff);
    //   }
    // }
    // radio_print_debug("\r\n");

    // uint8_t data_section = thermocouple_32[31:18];

    // // Then we index our array for the thermocouple reading and print that:
    // char pbuff[100];
    // for (int i=0; i<33; i++)
    // {

    //   sprintf(pbuff, "%u ", data_section);
    //   radio_print_debug(pbuff);

    // }
    // radio_print_debug("\r\n");

    // Then drive CS high:
    GPIO_WriteHigh(THERMO_CHIP_PORT, THERMO_CHIP_PIN_SELECT);
  
    // read_gps_buffer();
    // update_imu_state();
    // spl07_update_baro();
    // send_telemetry();
    delay_ms(1000); // Needs some delay
  }


  
  while (1){
    GPIO_WriteReverse(GREEN_LED_PORT, GREEN_LED_PIN);
    delay_ms(500);
  }
}

// void read_thermocouple(void)
// { 
//   uint8_t thermocouple_data_buff[14];
//   thermocouple_data_buff = SPI_ReceiveData();
//     // Can pass direct string or sprintf into a char buff and pass into here, like this:
//     char pbuff[14];
//     for (int i=0; i<255; i++)
//     {
//       sprintf(pbuff, "%d\r\n", (int)pbuff[i]);
//       radio_print_debug(pbuff);
//     }
// }

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

int32_t getTwosComplement(uint32_t raw, uint8_t length) {
  uint32_t comparison = (uint32_t)1<<(length - 1);
  int32_t result = 0;

  // char pbuf[50];
  // sprintf(pbuf, "Raw: %"PRIu32"   |   Length: %"PRIu8"   |   Comp: %"PRIu32"\r\n", raw, length, comparison);
  // radio_print_debug(pbuf);

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
    delay_ms(100);
  }
}
#endif