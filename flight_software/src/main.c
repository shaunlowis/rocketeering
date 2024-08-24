#include "common.h"
#include "gpio.h"
#include "ebyte_radio.h"
#include "SPL07_pressure.h"

void assert_failed(uint8_t* file, uint32_t line);
void clock_config(void);

void main(void)
{
  clock_config();
  GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  radio_uart_init();
  radio_print("Radio initialized\r\n");
  spl07_init();
  while (1){
    GPIO_WriteReverse(LED_PORT, LED_PIN);
    
    spl07_update_baro();
    
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
  radio_print(bits);
}

// int32_t getTwosComplement(uint32_t raw, uint8_t length)
// {

//     if (raw & ((int)1 << (length - 1))) {
//         radio_print("Negative!\r\n");
//         return ((int32_t)raw) - ((int32_t)1 << length);
//     }
//     else {
//         return (int32_t) raw;
//     }
// }

void logRawValue(uint32_t raw, uint8_t length, uint32_t comp) {
    char buff[256];
    sprintf(buff, "Raw = %"PRIu32" length = %"PRIu8" comp = %"PRIu32"\r\n", raw, length, comp);
    radio_print(buff);
}

int32_t getTwosComplement(uint32_t raw, uint8_t length) {
  uint32_t comparison = (uint32_t)1<<(length - 1);
  logRawValue(raw, length, comparison);  // Separate logging functionality
  // Check if the sign bit is set
  if (raw & comparison) {
      radio_print("Negative\r\n");
      // If the sign bit is set, convert to two's complement
      return (int32_t)(raw | (~0U << length));
  } else {
      radio_print("Positive\r\n");
      // If the sign bit is not set, it's already a positive value
      return (int32_t)raw;
  }
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
    radio_print(buff);
    GPIO_WriteReverse(LED_PORT, LED_PIN);
    delay_ms(100);
  }
}
#endif