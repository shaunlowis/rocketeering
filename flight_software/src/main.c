#include "common.h"
#include "gpio.h"
#include "ebyte_radio.h"
#include "SPL07_pressure.h"
#include "YIC_gps.h"
#include "i2c_driver_STM8S007.h"
#include "ICM42670_imu.h"
#include "microsd.h"

void assert_failed(uint8_t* file, uint32_t line);
void clock_config(void);

void main(void)
{
  clock_config();
  GPIO_Init(GREEN_LED_PORT, GREEN_LED_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(RED_LED_PORT, RED_LED_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  //GPIO_Init(MSD_CS_PORT, MSD_CS_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
  radio_uart_init();
  radio_print_debug("Radio initialized\r\n");

  
  SPI_Init(SPI_FIRSTBIT_MSB,
           SPI_BAUDRATEPRESCALER_2,
           SPI_MODE_MASTER,
           SPI_CLOCKPOLARITY_LOW,
           SPI_CLOCKPHASE_1EDGE,
           SPI_DATADIRECTION_2LINES_FULLDUPLEX,
           SPI_NSS_HARD,
           1);
  SPI_Cmd(ENABLE);

  // while(1)
  // {
  //   GPIO_WriteLow(MSD_CS_PORT, MSD_CS_PIN); /* CS pin low: MSD enabled */
  //   u8 DataOut = 0;
  //   /* Wait until the transmit buffer is empty */
  //   while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET);
  //   /* Send the byte */
  //   SPI_SendData(0xff);
  //   /* Wait to receive a byte*/
  //   while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
  //   /*Return the byte read from the SPI bus */ 
  //   DataOut = SPI_ReceiveData();
  //   GPIO_WriteHigh(MSD_CS_PORT, MSD_CS_PIN); /* CS pin low: MSD enabled */

  //   delay_ms(1000);
  // }

  radio_print_debug("SPI initialized\r\n");
  MSD_Init();
  radio_print_debug("SD initialized\r\n");
  // gps_init();
  // gps_test();

  // i2c_init();
  // imu_init(); // Needs i2c_init called first
  // spl07_init(); // Needs i2c_init called first
  
  while (1){
    GPIO_WriteReverse(GREEN_LED_PORT, GREEN_LED_PIN);
    //update_imu_state();
    //spl07_update_baro();
    delay_ms(500);
    
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
    sprintf(buff, "Wrong parameters value: file %s on line %d\r\n", file, line);
    radio_print_debug(buff);
    GPIO_WriteReverse(GREEN_LED_PORT, GREEN_LED_PIN);
    delay_ms(100);
  }
}
#endif