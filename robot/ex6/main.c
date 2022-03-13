#include "hardware.h"
#include "registers.h"
#include "module.h"
#include "config.h"
#include "robot.h"
#include "can.h"

#define REG_LED         1
#define LED_MODE_OFF    0
#define LED_MODE_HEAD   1

#define REG_LED_COLOR   0
#define BODY_NUMBER     2

const uint8_t MOTOR_ADDR[BODY_NUMBER] = {72, 21};

static void turn_off_all_leds(void) {
  uint8_t i = 0;
  for (i = 0; i < BODY_NUMBER; ++i) {
    set_reg_value_dw(MOTOR_ADDR[i], MREG32_LED, 0);
  }
  set_rgb(0, 0, 0);
}

static int8_t register_handler(uint8_t operation, uint8_t address, RadioData* radio_data)
{
    if (operation == ROP_WRITE_8 && address == REG_LED){
      switch(radio_data->byte) {
        case LED_MODE_OFF:
          turn_off_all_leds();
          return TRUE;
      }
    }
  return FALSE;
}

int main(void)
{
  hardware_init();
  registers_init();
  radio_add_reg_callback(register_handler);

  // Changes the color of the led (green) to show the boot
  set_color_i(2, 0);

  while(1) {

  }
  return 0;
}
