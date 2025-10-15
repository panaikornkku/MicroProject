#include "buttons.h"
#define STM32F411xE
#include "stm32f4xx.h"
#include <string.h>

/* ปุ่มเรียง: BTN1=PA10, BTN2=PB3, BTN3=PB5, BTN4=PB4 (Active-Low) */
/* ใช้บิทแมสก์แบบ register-level (ไม่ใช้ HAL) */
static const uint16_t pins[4]  = { (1u<<10), (1u<<3), (1u<<5), (1u<<4) };
static GPIO_TypeDef*  ports[4] = { GPIOA,     GPIOB,   GPIOB,   GPIOB    };

/* สถานะสำหรับ debounce */
static uint8_t stable[4];   // 0=up, 1=down
static uint8_t prev[4];     // เพื่อหา edge
static uint8_t cnt[4];      // เคาน์เตอร์ debounce

/* ถ้า Buttons_Scan() เรียกทุก ~1ms → 10 = ~10ms debounce */
#define DEBOUNCE_COUNT  10

void Buttons_Init(void) {
  memset(stable, 0, sizeof stable);
  memset(prev,   0, sizeof prev);
  memset(cnt,    0, sizeof cnt);
}

/* อ่านค่าดิบของปุ่มแบบ Active-Low: กด = 0 → คืนค่า 1 */
static inline uint8_t raw_down(int i) {
  return ((ports[i]->IDR & pins[i]) == 0u) ? 1u : 0u;
}

void Buttons_Scan(void) {
  for (int i = 0; i < 4; ++i) {
    uint8_t raw = raw_down(i);
    if (raw == stable[i]) {
      cnt[i] = 0;
    } else {
      if (++cnt[i] >= DEBOUNCE_COUNT) {
        stable[i] = raw;   // อัปเดตเป็นสถานะใหม่หลัง debounce ครบ
        cnt[i] = 0;
      }
    }
  }
}

bool Buttons_GetEdge(ButtonId id) {
  /* เรียกหลัง Buttons_Scan() เสมอ */
  bool edge = (stable[id] == 1 && prev[id] == 0);
  prev[id] = stable[id];
  return edge;
}
