/*
 * uart.c
 *
 *  Created on: Sep 30, 2025
 *      Author: panaikornmaneeratchakit
 */
#include <stdio.h>
#include "uart.h"
#define STM32F411xE
#include "stm32f4xx.h"

static char s_uart_buf[128];

void UART2_TxString(const char* str) {
  while (*str) {
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = *str++;
  }
}

void UART2_Printf(const char *fmt, ...) {
  va_list ap; va_start(ap, fmt);
  int n = vsnprintf(s_uart_buf, sizeof s_uart_buf, fmt, ap);
  va_end(ap);
  if (n > 0) UART2_TxString(s_uart_buf);
}

/* NEW: non-blocking RX */
bool UART2_GetCharNonBlocking(char *ch) {
  if (USART2->SR & USART_SR_RXNE) {
    *ch = (char)USART2->DR;      // อ่านแล้ว RXNE จะถูกเคลียร์
    return true;
  }
  return false;
}
