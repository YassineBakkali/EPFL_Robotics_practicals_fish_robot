#include <stdint.h>
#include "hardware.h"
#include "registers.h"

int main(void)
{
  int8_t i;

  hardware_init();
  reg32_table[REG32_LED] = LED_MANUAL;  // manual LED control

  // while (1) {
  //   for (i = 0; i < 127; i++) {
  //     set_rgb(i, 0, 0);
  //     pause(TEN_MS);
  //   }
  //   for (i = 0; i < 127; i++) {
  //     set_rgb(127, i, 0);
  //     pause(TEN_MS);
  //   }
  //   for (i = 0; i < 127; i++) {
  //     set_rgb(127, 127, i);
  //     pause(TEN_MS);
  //   }
  //   for (i = 127; i >= 0; i--) {
  //     set_rgb(i, 127, 127);
  //     pause(TEN_MS);
  //   }
  //   for (i = 127; i >= 0; i--) {
  //     set_rgb(0, i, 127);
  //     pause(TEN_MS);
  //   }
  //   for (i = 127; i >= 0; i--) {
  //     set_rgb(0, 0, i);
  //     pause(TEN_MS);
  //   }
  // }
  
  // Make the LED blink in green at 1 Hz
  while (1) {
    set_rgb(0, 127, 0);
    pause(5*HUNDRED_MS);
    set_rgb(0, 0, 0);
    pause(5*HUNDRED_MS);
  }
  return 0;
}
