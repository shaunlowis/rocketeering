#include "battery.h"
#include "inttypes.h"
#include "ebyte_radio.h"


#define VBATT_ANALOG_CHANNEL ADC2_CHANNEL_9
#define IBATT_ANALOG_CHANNEL ADC2_CHANNEL_3 // ADC2_CHANNEL_2 also

#define VBATT_TO_ADC_SLOPE 133.0 // From calibration, see ADC calibration spreadsheet in google drive
#define VBATT_TO_ADC_OFFSET 85.0 // From calibration, see ADC calibration spreadsheet in google drive


static uint16_t read_adc_channel(ADC2_Channel_TypeDef adc_channel);

#define linear_adc_to_input_val(adc_count, slope, offset) (((float)adc_count - (float)offset) / (float)slope)

static uint16_t read_adc_channel(ADC2_Channel_TypeDef adc_channel)
{
    ADC2_ClearFlag();
    ADC2_ConversionConfig(ADC2_CONVERSIONMODE_SINGLE, adc_channel, ADC2_ALIGN_RIGHT);
    ADC2_StartConversion();
    while(!ADC2_GetFlagStatus())
    {
        continue;
    }
    uint16_t adc_val = ADC2_GetConversionValue();
    return adc_val;
}

float read_batt_voltage(void)
{
    uint16_t adc_count = read_adc_channel(VBATT_ANALOG_CHANNEL);
    float voltage = linear_adc_to_input_val(adc_count, VBATT_TO_ADC_SLOPE, VBATT_TO_ADC_OFFSET);
    char buff[100];
    sprintf(buff, "%f V\r\n", voltage);
    radio_print_debug(buff);
}


void print_average_adc_value(uint8_t num_to_average)
{
    uint32_t adc_total = 0;
    for (int i=0; i<num_to_average; i++)
    {
        uint16_t adc_val = read_adc_channel(VBATT_ANALOG_CHANNEL);
        adc_total += (uint32_t)adc_val;
    }

    uint16_t adc_average = (uint16_t)(adc_total / (uint32_t)num_to_average);

    // float batt_voltage = ((float)adc_val / ADC_MAX_VALUE) * VDD * VBATT_ADC_IN_TO_VOLTAGE_SCALE_FACTOR;

    char buff[100];
    sprintf(buff, "%"PRIu16" sample average ADC value = %"PRIu16"\r\n", num_to_average, adc_average);
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
        read_batt_voltage();
        delay_ms(1000);
    }    
}


