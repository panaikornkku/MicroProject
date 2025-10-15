#pragma once
#include <stdint.h>
#define STM32F411xE
#include "stm32f4xx.h"

/* System / clocks / FPU */
void Clocks_Enable(void);
void FPU_Enable(void);

/* GPIO mux and modes:
   - PA2/PA3: USART2 AF7
   - PA5:     output (LOCK LED)
   - PA1:     analog (ADC1_IN1) for LDR
*/
void GPIO_Init(void);

/* Peripherals */
void USART2_Init_9600_8N1(void);
void ADC1_Init_Single_CH1(void);

/* Busy-wait delay (rough) */
void delay_ms(uint32_t ms);
