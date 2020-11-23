#include <temp_lib.h>

uint32_t adcBuf = 0;
float tempCelcius = 0;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  tempCelcius = adcBuf*.322;   // convert value from adc buffer to temperature in Centigrade
}
