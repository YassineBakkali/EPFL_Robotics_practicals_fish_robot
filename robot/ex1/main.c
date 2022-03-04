#include <stdint.h>
#include "hardware.h"
#include "registers.h"

int main(void)
{
  hardware_init();
  reg32_table[REG32_LED] = LED_MANUAL;  // manual LED control

  // Make the LED blink in green at 1 Hz
  while (1) {
    set_rgb(0, 127, 0);
    pause(5*HUNDRED_MS);
    set_rgb(0, 0, 0);
    pause(5*HUNDRED_MS);
  }
  return 0;
}
