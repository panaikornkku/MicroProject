/*
 * ui.c
 *
 *  Created on: Sep 30, 2025
 *      Author: panaikornmaneeratchakit
 */
#define STM32F411xE
#include "stm32f4xx.h"
#include "uart.h"
#include "ui.h"
#include "board.h"

void UI_ShowBoot(void) {
  UART2_TxString("System boot. Light monitoring started...\r\n");
}
void UI_ShowLux(float lux) {
  UART2_Printf("Light intensity = %d Lux\r\n", (uint16_t)lux);
}
void UI_RoomSafe(void) {
  GPIOB->ODR |= (1 << 6);
  UART2_TxString("Room Safe\r\n");
}
/* กะพริบไฟเหลือง N ครั้ง (บล็อกสั้นๆ ใช้ delay_ms) */
void UI_YellowBlink(uint8_t times, uint32_t on_ms, uint32_t off_ms) {
	GPIOA->ODR &= ~(1 << 7);
	for (uint8_t i = 0; i < times; ++i) {
		GPIOA->ODR |= (1 << 7);
		delay_ms(on_ms);
		GPIOA->ODR &= ~(1 << 7);
		delay_ms(off_ms);
  }
}
void UI_EnterLockMode(void) {
  GPIOB->ODR &= ~(1 << 6);       // ปิด PB6 (สีเขียว)
  UI_YellowBlink(10, 300, 300);
  UART2_TxString("\r\n=== LOCK MODE ===\r\n");
  UART2_TxString("Enter password:\r\n");
}

/* ====== ใหม่สำหรับ PIN ====== */
void UI_PinPrompt(void) {
  UART2_TxString("PIN format (4 chars): BTN1='1', BTN2='0', BTN3=Backspace, BTN4=Submit\r\n");
}
void UI_PinProgress(const char *buf, int len) {
  UART2_Printf("PIN: ");
  for (int i = 0; i < len; ++i) UART2_Printf("%c", buf[i]);
  UART2_TxString("\r");
}
void UI_PinOK(void) {
  UART2_TxString("PIN OK\r\n");
}
void UI_PinWrong(void) {
  UART2_TxString("Wrong PIN. Try again.\r\n");
}

// ใหม่
/* Lockout messages */
void UI_PinLockoutStart(uint32_t ms_total) {
  UART2_Printf("Too many wrong attempts. Locked for %lu ms.\r\n", (unsigned long)ms_total);
}
void UI_PinLockoutTick(uint32_t ms_left) {
  // ถ้าไม่อยากสแปมข้อความ สามารถคอมเมนต์ฟังก์ชันนี้ออกหรือเรียกทุก 1 วินาทีแทนได้
  // UART2_Printf("Lockout: %lu ms left\r\n", (unsigned long)ms_left);
  (void)ms_left;
}
void UI_PinLockoutEnd(void) {
  UART2_TxString("Lockout ended. You can try again.\r\n");
}

/* ใหม่: พิมพ์เลขถอยหลัง (เช่น 5, 4, 3, 2, 1) ทีละบรรทัด */
void UI_PinCountdown(uint8_t sec) {
  UART2_Printf("%u\r\n", (unsigned)sec);
}

void UI_ShowTemp(float temp){
	UART2_Printf("Temperature = %d C\r\n", (uint16_t)temp);
}

void UI_ShowPot(float pot){
	UART2_Printf("Potentiometer = %d \r\n", (uint16_t)pot);
}

void UI_NewPinPrompt_Keyboard(void) {
  UART2_TxString("Set new PIN via keyboard (4 digits of 0-9). Press Enter to confirm.\r\n");
  UART2_TxString("New PIN: ");
}

void UI_NewPinEcho(const char *buf, int len) {
  UART2_TxString("\r\nNew PIN: ");
  for (int i=0;i<len;++i) UART2_Printf("%c", buf[i]);
  UART2_TxString("\r\n");
}

void UI_NewPinTooShort(void) {
  UART2_TxString("PIN must be 4 digits (0/1). Try again.\r\n");
}

void UI_UnlockSuccess(void) {
  UART2_TxString("POT Unlock Successful!\r\n");
}
