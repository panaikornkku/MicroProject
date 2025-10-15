/*
 * ui.h
 *
 *  Created on: Sep 30, 2025
 *      Author: panaikornmaneeratchakit
 */

#pragma once

void UI_ShowBoot(void);
void UI_ShowLux(float lux);
void UI_RoomSafe(void);
void UI_EnterLockMode(void);

/* เพิ่มสำหรับ PIN */
void UI_PinPrompt(void);
void UI_PinProgress(const char *buf, int len);
void UI_PinOK(void);
void UI_PinWrong(void);

/* ใหม่: แจ้งช่วงล็อกอินพ์ */
void UI_PinLockoutStart(uint32_t ms_total);
void UI_PinLockoutTick(uint32_t ms_left);   // เรียกเป็นระยะ (ถ้าอยากโชว์นับถอยหลัง)
void UI_PinLockoutEnd(void);

/* ใหม่: แสดงเลขถอยหลัง 5..1 ทุกวินาที */
void UI_PinCountdown(uint8_t sec);
void UI_ShowTemp(float lux);
void UI_ShowPot(float pot);

void UI_NewPinPrompt_Keyboard(void);
void UI_NewPinEcho(const char *buf, int len);
void UI_NewPinTooShort(void);
void UI_UnlockSuccess(void);
