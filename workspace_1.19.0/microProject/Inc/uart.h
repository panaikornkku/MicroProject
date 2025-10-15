/*
 * uart.h
 *
 *  Created on: Sep 30, 2025
 *      Author: panaikornmaneeratchakit
 */
#pragma once
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

void UART2_TxString(const char* str);
void UART2_Printf(const char *fmt, ...);

bool UART2_GetCharNonBlocking(char *ch);
