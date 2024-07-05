#include "stm8s.h"

#define Led_Init GPIO_Init(GPIOD, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_FAST)
#define Led_ON   GPIO_WriteHigh    (GPIOD,GPIO_PIN_1)
#define Led_OFF  GPIO_WriteLow     (GPIOD,GPIO_PIN_1)
#define Led_TOG  GPIO_WriteReverse (GPIOD,GPIO_PIN_1)

void main(void) 
{

   // Init LED Port, Pin
   Led_Init;

   // Set LED ON
   Led_ON;

   // Loop
   while(1){
      // Toggle LED ON/OFF
      Led_TOG;

      // White moment
      for(uint16_t d = 0; d<19000; d++){
         for(uint8_t c = 0; c<5; c++);
      }
   }
}
