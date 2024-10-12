#include "battery.h"
#include "inttypes.h"
#include "ebyte_radio.h"


#define VBATT_ANALOG_CHANNEL ADC2_CHANNEL_9
#define IBATT_ANALOG_CHANNEL ADC2_CHANNEL_3 // ADC2_CHANNEL_2 also

#define ADC_MAX_VALUE 1024.0f // 10 bit
#define VBATT_R1 1020000.0f
#define VBATT_R2  990000.0f
#define VDD 3.3f

#define VBATT_ADC_IN_TO_VOLTAGE_SCALE_FACTOR ((VBATT_R1 + VBATT_R2) / VBATT_R2)




void measure_batt_voltage(void)
{
    ADC2_ClearFlag();
    ADC2_ConversionConfig(ADC2_CONVERSIONMODE_SINGLE, VBATT_ANALOG_CHANNEL, ADC2_ALIGN_RIGHT);
    ADC2_StartConversion();
    while(!ADC2_GetFlagStatus())
    {
        continue;
    }
    uint16_t adc_val = ADC2_GetConversionValue();
    float batt_voltage = ((float)adc_val / ADC_MAX_VALUE) * VDD * VBATT_ADC_IN_TO_VOLTAGE_SCALE_FACTOR;

    char buff[50];
    sprintf(buff, "ADC val = %.2f\r\n", batt_voltage);
    radio_print_debug(buff);
}


void init_battery_measurements(void)
{

    ADC2_DeInit();
    ADC2_Init(ADC2_CONVERSIONMODE_SINGLE,
              VBATT_ANALOG_CHANNEL,
              ADC2_PRESSEL_FCPU_D18,
              ADC2_EXTTRIG_TIM,
              DISABLE,
              ADC2_ALIGN_RIGHT,
              ADC2_SCHMITTTRIG_ALL,
              DISABLE);
    ADC2_Cmd(ENABLE);

    while(1)
    {
        measure_batt_voltage();
        delay_ms(1000);
    }


    
}


