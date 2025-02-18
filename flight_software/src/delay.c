/**
  ******************************************************************************
  * Delay.c - Interrupt-based tick and millisecond delay function
  * Requires a 16MHz CPU to produce accurate millisecond/microsecond functions. 
  * 
  * How to use:
  * 	- #include "delay.h" in your source file
  * 	- Initialize timer4 by calling 'TIM4_Config();
  * 	- use delay_ms(uint16_t n) to delay n milliseconds
  * 	- use delay_us(uint16_t n) to delat n microseconds
  * 	- millis() returns the number of milliseconds since bootup
  * 	- micros() returns the number of microseconds since bootup
  *
  * @author  Brad Roy
  * @version V1.0 All major functions implemented and working.
  * @date    04-Dec-2020
https://github.com/prosper00/STM8-AD9833-Function-Generator/tree/main
*****************************************************************************/ 
#include "common.h"
#include "delay.h"
#define TICK_PIN GPIOD,GPIO_PIN_5

/** Global tick */
volatile uint32_t tick = 0; //increments every 250us

void TIM4_Config(void)
{
	CLK_PeripheralClockConfig (CLK_PERIPHERAL_TIMER4 , ENABLE); 

	TIM4_DeInit();
	TIM4_TimeBaseInit(TIM4_PRESCALER_16, 250); //TimerClock = 16000000 / 16 / 250 = 4000Hz
	disableInterrupts(); // Must do before ITC_SetSoftwarePriority
	TIM4_ClearFlag(TIM4_FLAG_UPDATE);
	ITC_SetSoftwarePriority(ITC_IRQ_TIM4_OVF, ITC_PRIORITYLEVEL_3); // Want this to have the highest priority in our code
	TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
	enableInterrupts(); // global interrupt enable
	TIM4_Cmd(ENABLE);  //Start Timer 4
}

//void TIM4_UPD_OVF_IRQHandler(void) __interrupt(23) // now compiles w/SDCC 4.3.0. not tested for function, yet
INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, ITC_IRQ_TIM4_OVF) // throws a warning w / SDCC >= 4.3.0
{
	tick++;
	TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
	GPIO_WriteReverse(TICK_PIN);            //toggle our systick pin (should be about 2kHz)
}

/** Delay ms */
void delay_ms(uint16_t ms)
{
	for(int i=0; i<ms; i++)
		delay_us(1000);
}

/** delays to the nearest 1us 
 * accepts a value from 0 to 65536us to delay
 * if longer delays are needed, use delay_ms
 **/
void delay_us(uint16_t us)
{
	uint8_t start_us = TIM4_GetCounter();  //tim4 increments every us
	if(us>=250){   //we only need to bother with the following for delays greater than 1 tick (250us)
		uint16_t start_tick = (uint16_t)tick;  //the tick increments every 250us
		uint16_t delay_ticks = us/250;
	
		while(((uint16_t)tick - start_tick) < delay_ticks); // delay in multiples of 250us
	}
	while(TIM4_GetCounter() < start_us); //now wait until our 1us counter matches our start us
}

/** returns the number of milliseconds that have passed since boot  **/
uint32_t millis(void){
	return((uint32_t)(tick >> 2)); // divide tick by 4 returns milliseconds
}

/** returns the number of microseconds that have passed since boot  **/
uint32_t micros(void){
	return(tick*250 + TIM4_GetCounter()); //each tick is worth 250us
}