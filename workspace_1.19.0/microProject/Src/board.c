/*
 * board.c
 *
 *  Created on: Sep 30, 2025
 *      Author: panaikornmaneeratchakit
 */
#include "board.h"

void Clocks_Enable(void) {
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;     // GPIOA
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;     // GPIOB
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;      // ADC1
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;    // USART2
}

void FPU_Enable(void) {
  SCB->CPACR |= (0b1111 << 20);
  __asm volatile("dsb");
  __asm volatile("isb");
}

void GPIO_Init(void) {
  /* ==== (เดิม) UART2 PA2/PA3, LED PA5, ADC PA1 ==== */
  GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
  GPIOA->MODER |=  (0b10 << GPIO_MODER_MODER2_Pos) | (0b10 << GPIO_MODER_MODER3_Pos);
  GPIOA->AFR[0]  &= ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3);
  GPIOA->AFR[0]  |=  (0x07 << GPIO_AFRL_AFSEL2_Pos) | (0x07 << GPIO_AFRL_AFSEL3_Pos);

  // สีเขียว
  GPIOB->MODER &= ~(GPIO_MODER_MODER6);
  GPIOB->MODER |=  (0b01 << GPIO_MODER_MODER6_Pos);
  GPIOB->OTYPER &= ~(1 << 6);
  GPIOB->OSPEEDR |= (0b10 << GPIO_OSPEEDR_OSPEED6_Pos);
  GPIOB->ODR &= ~(GPIO_ODR_OD6);

    //สีน้ำเงิน
  GPIOA->MODER &= ~(GPIO_MODER_MODER5);
  GPIOA->MODER |=  (0b01 << GPIO_MODER_MODER5_Pos);
  GPIOA->OTYPER &= ~(1 << 5);
  GPIOA->OSPEEDR |= (0b10 << GPIO_OSPEEDR_OSPEED5_Pos);
  GPIOA->ODR &= ~(GPIO_ODR_OD5);

    //สีเหลือง
  GPIOA->MODER &= ~(GPIO_MODER_MODER7);
  GPIOA->MODER |=  (0b01 << GPIO_MODER_MODER7_Pos);
  GPIOA->OTYPER &= ~(1 << 7);
  GPIOA->OSPEEDR |= (0b10 << GPIO_OSPEEDR_OSPEED7_Pos);
  GPIOA->ODR &= ~(GPIO_ODR_OD7);


  GPIOA->MODER &= ~(GPIO_MODER_MODER1);
  GPIOA->MODER |=  (0b11 << GPIO_MODER_MODER1_Pos);

  /* ==== (ใหม่) ปุ่ม: PA10, PB3, PB5, PB4 เป็น Input Pull-Up ==== */
  /* PA10 */
  GPIOA->MODER  &= ~(3u << (10*2));            // input
  GPIOA->PUPDR  &= ~(3u << (10*2));
  GPIOA->PUPDR  |=  (1u << (10*2));            // pull-up

  // PA4 เป็น Analog (pot wiper)
  GPIOA->MODER &= ~(GPIO_MODER_MODER4);
  GPIOA->MODER |=  (0b11 << GPIO_MODER_MODER4_Pos);   // analog mode

  /* เปิด clock GPIOB เผื่อยังไม่ได้เปิด */
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

  /* PB3 */
  GPIOB->MODER  &= ~(3u << (3*2));
  GPIOB->PUPDR  &= ~(3u << (3*2));
  GPIOB->PUPDR  |=  (1u << (3*2));             // pull-up

  /* PB5 */
  GPIOB->MODER  &= ~(3u << (5*2));
  GPIOB->PUPDR  &= ~(3u << (5*2));
  GPIOB->PUPDR  |=  (1u << (5*2));             // pull-up

  /* PB4 */
  GPIOB->MODER  &= ~(3u << (4*2));
  GPIOB->PUPDR  &= ~(3u << (4*2));
  GPIOB->PUPDR  |=  (1u << (4*2));             // pull-up
}


void USART2_Init_9600_8N1(void) {
  /* 9600 @ 16 MHz APB1 clock */
  USART2->BRR = 139;
  USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;   // enable TX/RX
  USART2->CR1 |= USART_CR1_UE;                  // enable USART
}

void ADC1_Init_Single_CH1(void) {
  ADC1->CR2 |= ADC_CR2_ADON;                    // enable ADC1

  /* Sample time for channel 1 = max (good for high impedance sensor) */
  ADC1->SMPR2 &= ~(ADC_SMPR2_SMP1);
  ADC1->SMPR2 |= ADC_SMPR2_SMP1;             // set to 111b (480 cycles)
  ADC1->SMPR2 |= ADC_SMPR2_SMP4;
  /* Regular sequence length = 1 conversion (L = 0) */
  ADC1->SQR1 &= ~(ADC_SQR1_L);
  ADC1->SQR1 |= (0 << ADC_SQR1_L_Pos);

  /* 1st conversion is channel 1 (PA1) */
  ADC1->SQR3 &= ~(ADC_SQR3_SQ1);
  ADC1->SQR3 |=  (1U << ADC_SQR3_SQ1_Pos);
}

/* crude busy-wait delay; tune multiplier if your core clock differs */
void delay_ms(uint32_t ms) {
  for (volatile uint32_t i = 0; i < (ms * 266); i++) {
    __NOP();
  }
}


