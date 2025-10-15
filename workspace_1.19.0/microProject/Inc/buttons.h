/*
 * buttons.h
 *
 *  Created on: Sep 30, 2025
 *      Author: panaikornmaneeratchakit
 */
#pragma once
#include <stdint.h>
#include <stdbool.h>

/* ปุ่ม 4 ปุ่มบนบอร์ด (สมมติ PA0..PA3, Active-Low + Pull-Up) */
typedef enum {
  BTN1 = 0,  // '1'
  BTN2 = 1,  // '0'
  BTN3 = 2,  // backspace
  BTN4 = 3   // submit
} ButtonId;

void Buttons_Init(void);

/* เรียกบ่อย ๆ (ทุก 1–5 ms) เพื่อสแกนและ debounce */
void Buttons_Scan(void);

/* ขอบการกดครั้งเดียว (press rising edge หลัง debounce) */
bool Buttons_GetEdge(ButtonId id);
