#include "common.h"
#include "gpio.h"
#include "ebyte_radio.h"

void assert_failed(uint8_t* file, uint32_t line);
void clock_config(void);

void main(void)
{
  clock_config();
  GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  radio_uart_init();

  while (1){
    GPIO_WriteReverse(LED_PORT, LED_PIN);
    char buff[] = "Hello world!\n";
    radio_transmit_string(buff, sizeof(buff)/sizeof(buff[0]));
    delay_ms(1000);
  }
}

void clock_config(void)
{
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
  TIM4_Config();
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
  (void) file; // As they are unused, to prevent compiler warning. delete this line if used
  (void) line; // As they are unused, to prevent compiler warning. delete this line if used
  /* Infinite loop */
  while (1)
  {
  }
}
#endif