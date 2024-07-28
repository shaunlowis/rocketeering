#include "common.h"
#include "gpio.h"

void assert_failed(uint8_t* file, uint32_t line);

void main(void)
{
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
  TIM4_Config();
  GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_FAST);

  while (1){
    GPIO_WriteReverse(LED_PORT, LED_PIN);
    delay_ms(1000);
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
  (void) file; // As they are unused, to prevent compiler warning. delete this line if used
  (void) line; // As they are unused, to prevent compiler warning. delete this line if used
  /* Infinite loop */
  while (1)
  {
  }
}
#endif