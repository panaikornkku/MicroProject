// temp.c
#include "temp.h"
#define STM32F411xE
#include "stm32f4xx.h"

#define VREF        3.3f
#define ADC_MAXRES  4095.0f

float Temp_ReadCelsius(void) {
  ADC1->CR2 |= ADC_CR2_SWSTART;
  while (!(ADC1->SR & ADC_SR_EOC));  // wait for conversion
  uint16_t raw = ADC1->DR;

  float v = (raw * VREF) / ADC_MAXRES;
  float tempC = (v - 0.5f) * 100.0f; // ตัวอย่างการแปลง (เช่น LM35)
  return tempC;
}
