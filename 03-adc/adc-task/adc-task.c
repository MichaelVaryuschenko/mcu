#include "adc-task.h"

const uint32_t ADC_PIN = 26;
const uint32_t ADC_CH = 0;
const uint32_t ADC_CH_TEMP = 4; // Number of ADC channel attached to temperature sensor
adc_task_state_t adc_state = ADC_TASK_STATE_IDLE;
uint64_t adc_ts = 0;
uint32_t ADC_TASK_MEAS_PERIOD_US = 100000;

void adc_task_init() {
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_set_temp_sensor_enabled(true);
}

void adc_task_set_state(adc_task_state_t new_adc_state) {
    adc_state = new_adc_state;
}

float adc_task_measure() {
    adc_select_input(ADC_CH);
    uint16_t voltage_counts = adc_read();
    float voltage = 3.3 * ((float)voltage_counts / 4095); // Voltage measured by ADC (V units)
    return voltage;
}

float adc_task_measure_temp() {
    adc_select_input(ADC_CH_TEMP);
    uint16_t temp_counts = adc_read();
    float temp_V = 3.3 * ((float)temp_counts / 4095); // Voltage measured by ADC (V units)
    float temp = 27.0f - (temp_V - 0.706f) / 0.001721f; // Temperature measured (Celsius degrees)
    return temp;
}

void adc_task_handle() {
    switch (adc_state)
    {
    case ADC_TASK_STATE_IDLE:
        break;

    case ADC_TASK_STATE_RUN:
        if(time_us_64() > adc_ts) {
            adc_ts = time_us_64() + ADC_TASK_MEAS_PERIOD_US;
            float temp = adc_task_measure_temp();
            float voltage = adc_task_measure();
            printf("%f %f\n", voltage, temp);
        }
        break;
    
    default:
        break;
    }
}