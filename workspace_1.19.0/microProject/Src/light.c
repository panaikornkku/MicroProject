/*
 * light.c
 *
 *  Created on: Sep 30, 2025
 *      Author: panaikornmaneeratchakit
 */
#include <math.h>
#define STM32F411xE
#include "stm32f4xx.h"
#include "light.h"

/* ====== Calibration & circuit params ====== */
#define VREF        3.3f
#define ADC_MAXRES  4095.0f
#define RX          10000.0f     // series resistor with LDR (ohms)
#define SLOPE       -0.6875f     // LDR calibration slope (log10)
#define OFFSET      5.1276f      // LDR calibration offset (log10)

/* Helpers */
static float adc_to_voltage(uint16_t raw) {
  return (raw * VREF) / ADC_MAXRES;
}
static float ldr_resistance(float v_adc) {
  if (v_adc <= 0.0f) return 1e9f;
  if (v_adc >= VREF) return 1e9f;
  return RX * (v_adc / (VREF - v_adc));
}
static float resistance_to_lux(float r_ldr) {
  float logR = log10f(r_ldr);
  float x    = (logR - OFFSET) / SLOPE;
  return powf(10.0f, x);
}

float Light_ReadLux(void) {
  /* Start single conversion */
  ADC1->CR2 |= ADC_CR2_SWSTART;
  while ((ADC1->SR & ADC_SR_EOC) == 0) { /* wait */ }
  uint16_t raw = (uint16_t)ADC1->DR;

  float v   = adc_to_voltage(raw);
  float r   = ldr_resistance(v);
  float lux = resistance_to_lux(r);
  return lux;
}

