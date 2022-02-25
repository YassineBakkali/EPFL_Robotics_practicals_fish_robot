#include "hardware.h"
#include "module.h"
#include "robot.h"

const uint8_t MOTOR_ADDR = 21;

int main(void)
{
  int8_t pos;

  hardware_init();
  
  // Changes the color of the led (red) to show the boot
  set_color_i(4, 0);

  // Initialises the body module with the specified address (but do not start
  // the PD controller)
  init_body_module(MOTOR_ADDR);
  
  // And then... do this
  while (1) {
    pos = bus_get(MOTOR_ADDR, MREG_POSITION);
    if (pos > 0) {
      set_rgb(pos, 32, 0);
    } else {
      pos = -pos;
      set_rgb(0, 32, pos);
    }
  }

  return 0;
}
