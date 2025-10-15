#include "board.h"
#include "uart.h"
#include "light.h"
#include "ui.h"
#include "fsm.h"
#include "buttons.h"

extern volatile uint8_t g_fsm_done;

int main(void) {
  Clocks_Enable();
  FPU_Enable();
  GPIO_Init();
  USART2_Init_9600_8N1();
  ADC1_Init_Single_CH1();

  Light_Init();
  Buttons_Init();

  UI_ShowBoot();

  while (!g_fsm_done) {
      FSM_Tick();
    }
}
