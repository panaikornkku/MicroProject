#include <string.h>
#include <math.h>
#include "board.h"
#include "light.h"
#include "ui.h"
#include "buttons.h"
#include "temp.h"
#include "pot.h"

/* ====== Parameter ====== */
static const float   LIGHT_THRESHOLD_LUX = 100.0f;
static const float   TEMP_THRESHOLD      = 35.0f;
static const uint32_t LOOP_DELAY_MS      = 50;
static const uint32_t LOCK_LOOP_DELAY_MS = 10;

/* ====== PIN settings ====== */
static const char PRESET_PIN[] = "1001";
#define PIN_LEN 4
#define POT_TOL_ADC 40    // ยอมได้ ±40 counts ~ ±1% ของ 4095
#define POT_UNLOCK_STABLE_COUNT 2

/* ====== State machine ====== */
typedef enum {
  ST_DAY_IDLE = 0,
  ST_LOCK_MODE,
  ST_PIN_ENTRY,
  ST_PIN_OK,
  ST_TEMP_CHECK,
  ST_SET_POT_PIN,
  ST_POT_UNLOCK,
  ST_END
} state_t;

static state_t  g_state = ST_DAY_IDLE;

volatile uint8_t g_fsm_done = 0;

/* ====== Shared variables ====== */
static char pin_buf[PIN_LEN];
static int  pin_len = 0;
static char new_pin[PIN_LEN];
static int  new_pin_len = 0;
static uint16_t pot_target = 0;

#define POT_TOLERANCE 20

/* ====== Helper Functions ====== */
static void Pin_Clear(void) {
  memset(pin_buf, 0, sizeof pin_buf);
  pin_len = 0;
  UI_PinProgress(pin_buf, pin_len);
}

static void NewPin_Clear(void) {
  memset(new_pin, 0, sizeof new_pin);
  new_pin_len = 0;
  UI_PinProgress(new_pin, new_pin_len);
}

/* ====== Main FSM ====== */
void FSM_Tick(void) {
  switch (g_state) {

  /* --------- Stage 1: Normal Day Mode --------- */
  case ST_DAY_IDLE: {
    float lux = Light_ReadLux();
    UI_ShowLux(lux);
    if (lux < LIGHT_THRESHOLD_LUX) {
      UI_EnterLockMode();
      Pin_Clear();
      g_state = ST_PIN_ENTRY;
    } else {
      UI_RoomSafe();
    }
    delay_ms(LOOP_DELAY_MS);
  } break;

  /* --------- Stage 2: PIN Entry --------- */
  case ST_PIN_ENTRY: {
    Buttons_Scan();

    if (Buttons_GetEdge(BTN1)) {
      if (pin_len < PIN_LEN) { pin_buf[pin_len++] = '1'; UI_PinProgress(pin_buf, pin_len); }
    }
    if (Buttons_GetEdge(BTN2)) {
      if (pin_len < PIN_LEN) { pin_buf[pin_len++] = '0'; UI_PinProgress(pin_buf, pin_len); }
    }
    if (Buttons_GetEdge(BTN3)) {
      if (pin_len > 0) { pin_buf[--pin_len] = 0; UI_PinProgress(pin_buf, pin_len); }
    }
    if (Buttons_GetEdge(BTN4)) {
      if (strncmp(pin_buf, PRESET_PIN, PIN_LEN) == 0) {
        UI_PinOK();
        g_state = ST_TEMP_CHECK;
      } else {
        UI_PinWrong();
        Pin_Clear();
      }
    }

    delay_ms(LOCK_LOOP_DELAY_MS);
  } break;

  /* --------- Stage 3: Temperature Check --------- */
  case ST_TEMP_CHECK: {
    float temp = Temp_ReadCelsius();
    UI_ShowTemp(temp);

    if (temp > TEMP_THRESHOLD) {
      UART2_TxString("!!! Temperature HIGH !!!\r\n");
      GPIOA->ODR |= GPIO_ODR_OD5;   // LED ON
      NewPin_Clear();
      g_state = ST_SET_POT_PIN;
    } else {
      GPIOA->ODR &= ~GPIO_ODR_OD5;  // LED OFF
    }

    delay_ms(1000);
  } break;

  /* --------- Stage 4: Set new PIN --------- */
  case ST_SET_POT_PIN: {
    static char new_pin[PIN_LEN];
    static int  new_pin_len = 0;
    char c;

    if (new_pin_len == 0) {
      UI_NewPinPrompt_Keyboard();
    }

    if (UART2_GetCharNonBlocking(&c)) {
      if (c == '\r' || c == '\n') {
        if (new_pin_len == PIN_LEN) {
          UART2_TxString("\r\nNew password saved.\r\n");

          int val = 0;
          for (int i = 0; i < PIN_LEN; ++i)
            val = val * 10 + (new_pin[i] - '0');

          /* แบบเดิม: ใช้ mod 4096 ให้ได้ช่วง 0..4095 (ง่าย/ไม่เป็นเชิงเส้น) */
          pot_target = (uint16_t)(val % 4096);


          // ทางเลือก (เชิงเส้นเต็มช่วง 0000..9999 -> 0..4095):
//             pot_target = (uint16_t)((val * 4095) / 9999);

          UART2_Printf("Target potentiometer value = %u\r\n", pot_target);

          /* next state */
          new_pin_len = 0;
          memset(new_pin, 0, sizeof new_pin);
          g_state = ST_POT_UNLOCK;
        } else {
          UART2_TxString("\r\n");
          UI_NewPinTooShort();
          new_pin_len = 0;
          memset(new_pin, 0, sizeof new_pin);
          UI_NewPinPrompt_Keyboard();
        }
      }
      /* Backspace (0x08 หรือ 0x7F) */
      else if (c == 0x08 || c == 0x7F) {
           if (new_pin_len > 0) {
             new_pin[--new_pin_len] = 0;
             UART2_TxString("\b \b");          // ลบตัวล่าสุดบนเทอร์มินัล
           }
      }
      else if (c >= '0' && c <= '9') {       // << เปลี่ยนเป็นรับ 0..9
        if (new_pin_len < PIN_LEN) {
          new_pin[new_pin_len++] = c;
          /* echo ทันที */
          while (!(USART2->SR & USART_SR_TXE));
          USART2->DR = c;
        }
        /* ต้องกด Enter เพื่อยืนยัน (หรือจะ auto-submit เมื่อครบ 4 ก็ได้) */
      }
      /* อักขระอื่น: เมิน */
    }

    delay_ms(LOCK_LOOP_DELAY_MS);
  } break;


  /* --------- Stage 5: POT Unlock (auto) --------- */
  case ST_POT_UNLOCK: {
    static uint8_t  banner_shown = 0;
    static uint8_t  stable_hits  = 0;      // จำนวนรอบที่อยู่ในช่วง
    static uint32_t acc_ms       = 0;      // สะสมเวลาเพื่อพิมพ์สถานะเป็นช่วง ๆ

    if (!banner_shown) {
      UART2_TxString("\r\n=== UNLOCK USING POTENTIOMETER ===\r\n");
      UART2_TxString("Rotate POT until it matches the target (auto-detect).\r\n");
      UART2_Printf("Target (ADC) = %u  (tolerance ±%u)\r\n",pot_target, (unsigned)POT_TOL_ADC);
      banner_shown = 1;
      stable_hits  = 0;
      acc_ms       = 0;
    }

    /* อ่านค่า POT ปัจจุบัน (0..4095) และคำนวณส่วนต่าง */
    uint16_t pot_raw = Pot_ReadValue();                 // จาก pot.c (uint16_t)
    int16_t  diff    = (int16_t)pot_raw - (int16_t)pot_target;

    /* โชว์สถานะปัจจุบันเป็นช่วง ๆ (ทุก ~1s) เพื่อลดสแปมหน้าจอ */
    acc_ms += 300;   // เท่ากับ delay ข้างล่าง
    if (acc_ms >= 1000) {
      acc_ms = 0;
      UART2_Printf("POT=%u  Target=%u  Δ=%d\r\n", pot_raw, pot_target, (int)diff);
    }

    /* ตรวจว่าอยู่ในช่วงยอมรับหรือไม่ */
    if (diff <= (int16_t)POT_TOL_ADC && diff >= -(int16_t)POT_TOL_ADC) {
      if (stable_hits < 255) stable_hits++;
    } else {
      stable_hits = 0;
    }

    /* อยู่ในช่วงต่อเนื่องครบตามที่กำหนด → ปลดล็อกอัตโนมัติ */
    if (stable_hits >= POT_UNLOCK_STABLE_COUNT) {
    	UI_UnlockSuccess();

      // ไฟสถานะ: ออกจากโหมดล็อก → เขียวติด เหลืองดับ
      // ถ้ามี helper:
      // LED_Yellow_Off();
      // LED_Green_On();
      // หรือเขียนรีจิสเตอร์ตรง:

      banner_shown = 0;
      stable_hits  = 0;
      acc_ms       = 0;
    }
    g_state = ST_END;   // << เปลี่ยนมาที่สถานะใหม่
    delay_ms(300);   // คาบตรวจวัด ~300 ms
    break;
  }

  /* --------- Final state: ST_END --------- */
  case ST_END: {
    static uint8_t printed = 0;
    if (!printed) {
      // ข้อความจบ
      UART2_TxString("Unlock success\r\n");  // หรือ UI_UnlockSuccessSlow(40);
      // (ถ้าต้องการ) ตั้งไฟสถานะท้ายสุด
      // LED_Yellow_Off(); LED_Green_On();

      printed = 1;
    }

    // จบ FSM: ออกจากลูปหลัก
    g_fsm_done = 1;
    break;
  }



  }
}
