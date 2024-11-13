#include "common.h"
#include "gpio.h"
#include "ebyte_radio.h"
#include "SPL07_pressure.h"
#include "YIC_gps.h"
#include "i2c_driver_STM8S007.h"
#include "ICM42670_imu.h"
#include "inttypes.h"
#include "MAX31855_therm.h"
#include "battery.h"

void assert_failed(uint8_t* file, uint32_t line);
static void clock_config(void);

#define MAIN_LOOP_FREQ_HZ 5 // 20 mins logging 200 chars @ 50 Hz is around 12 mB.

void main(void)
{
  clock_config();
  GPIO_Init(GREEN_LED_PORT, GREEN_LED_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(RED_LED_PORT, RED_LED_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(M0_RADIO_PORT, M0_RADIO_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(M1_RADIO_PORT, M1_RADIO_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
  radio_uart_init();

  radio_print_debug("Radio initialized\r\n");

  init_battery_measurements();
  i2c_init();
  imu_init(); // Needs i2c_init called first
  spl07_init(); // Needs i2c_init called first
  gps_init();
  thermo_init();
  
  uint32_t loop_start_time;
  uint32_t current_time;
  
  while (1){
    loop_start_time = millis();
    GPIO_WriteReverse(GREEN_LED_PORT, GREEN_LED_PIN);

    // Read all sensors
    read_gps_buffer();
    update_imu_state();
    spl07_update_baro();
    read_batt_voltage();
    read_batt_current();
    update_thermo_state();

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