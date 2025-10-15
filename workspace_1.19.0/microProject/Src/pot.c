// pot.c
#define STM32F411xE
#include "stm32f4xx.h"
#include "pot.h"

uint16_t Pot_ReadValue(void) {
  uint32_t old = (ADC1->SQR3 & ADC_SQR3_SQ1);
  ADC1->SQR3 = (ADC1->SQR3 & ~ADC_SQR3_SQ1) | (4u << ADC_SQR3_SQ1_Pos);

  ADC1->CR2 |= ADC_CR2_SWSTART;
  while ((ADC1->SR & ADC_SR_EOC) == 0) {}
  uint16_t raw = (uint16_t)ADC1->DR;

  // restore ช่องเดิม
  ADC1->SQR3 = (ADC1->SQR3 & ~ADC_SQR3_SQ1) | old;
  return raw;
}


