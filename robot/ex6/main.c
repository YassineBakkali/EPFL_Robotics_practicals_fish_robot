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
  
	uint32_t rgb = ((uint32_t) r << 16) | ((uint32_t) g << 8) | b;
	

  // Changes the color of the led (green) to show the boot
  set_color_i(2, 0);


//   // Calls the main mode loop (see modes.c)
//   main_mode_loop();

  return 0;
}
