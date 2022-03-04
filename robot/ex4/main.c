#include "hardware.h"
#include "registers.h"
#include "modes.h"
#include "module.h"
#include "config.h"
#include "robot.h"

int main(void)
{
  hardware_init();
  registers_init();
  // Changes the color of the led (green) to show the boot
  set_color_i(2, 0);
  main_mode_loop();
  // Calls the main mode loop (see modes.c)
  return 0;
}

