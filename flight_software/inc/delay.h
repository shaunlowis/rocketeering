
// https://github.com/prosper00/STM8-AD9833-Function-Generator/tree/main
#ifndef DELAY_H
#define DELAY_H

/** Global tick **/
extern volatile uint32_t tick;

/** Functions **/
void TIM4_UPD_OVF_IRQHandler(void) __interrupt(23);
void TIM4_Config(void);
void delay_ms(uint16_t ms);
void delay_us(uint16_t us);
uint16_t millis(void);
uint32_t micros(void);

#endif //DELAY_H