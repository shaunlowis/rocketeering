#include "battery.h"
#include "inttypes.h"
#include "ebyte_radio.h"


#define VBATT_ANALOG_CHANNEL ADC2_CHANNEL_9
#define IBATT_ANALOG_CHANNEL ADC2_CHANNEL_3 // ADC2_CHANNEL_2 also

#define VBATT_TO_ADC_SLOPE   135.0 // From calibration, see ADC calibration spreadsheet in google drive
#define VBATT_TO_ADC_OFFSET   73.6 // From calibration, see ADC calibration spreadsheet in google drive
 
#define IBATT_TO_ADC_SLOPE     2.15 // From calibration, see ADC calibration spreadsheet in google drive
#define IBATT_TO_ADC_OFFSET   -4.16 // From calibration, see ADC calibration spreadsheet in google drive

#define linear_adc_to_input_val(adc_count, slope, offset) (((float)adc_count - (float)offset) / (float)slope)

static uint16_t read_adc_channel(ADC2_Channel_TypeDef adc_channel);
static uint16_t read_average_adc_count(uint16_t num_to_average, ADC2_Channel_TypeDef adc_channel);

typedef struct {
    float batt_voltage_V;
    float batt_current_mA;
} battery_status_t;

static battery_status_t battery_status = {.batt_voltage_V = 0.0,
                                          .batt_current_mA = 0.0};


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

static uint16_t read_average_adc_count(uint16_t num_to_average, ADC2_Channel_TypeDef adc_channel)
{
    uint32_t adc_total = 0;
    for (int i=0; i<num_to_average; i++)
    {
        uint16_t adc_val = read_adc_channel(adc_channel);
        adc_total += (uint32_t)adc_val;
    }

    uint16_t adc_average = (uint16_t)(adc_total / (uint32_t)num_to_average);
    return adc_average;
}

void read_batt_voltage(void)
{
    uint16_t average_adc_count = read_average_adc_count(10, VBATT_ANALOG_CHANNEL);
    float voltage = linear_adc_to_input_val(average_adc_count, VBATT_TO_ADC_SLOPE, VBATT_TO_ADC_OFFSET);
    // char buff[100];
    // sprintf(buff, "Voltage ADC count %u votlage %.2f V\r\n", average_adc_count, voltage);
    // radio_print_debug(buff);
    battery_status.batt_voltage_V = voltage;
}

void read_batt_current(void)
{
    uint16_t average_adc_count = read_average_adc_count(10, IBATT_ANALOG_CHANNEL);
    float current = linear_adc_to_input_val(average_adc_count, IBATT_TO_ADC_SLOPE, IBATT_TO_ADC_OFFSET);
    // char buff[100];
    // sprintf(buff, "Current ADC count %u current %.1f mA\r\n", average_adc_count, current);
    // radio_print_debug(buff);
    battery_status.batt_current_mA = current;
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
}

float get_batt_voltage(void)
{
    return battery_status.batt_voltage_V;
}

float get_batt_current(void)
{
    return battery_status.batt_current_mA;
}
