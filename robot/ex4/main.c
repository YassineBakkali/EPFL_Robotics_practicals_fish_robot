#include "hardware.h"
#include "registers.h"
// #include "modes.h"
#include "module.h"

#include "config.h"
#include "robot.h"

static uint8_t MOTOR_ADDR = 21; //TODO: change address, maybe delete modes.c from makefile and put back const

static int8_t register_handler(uint8_t operation, uint8_t address, RadioData* radio_data)
{
  uint8_t i;
    if (operation == ROP_WRITE_8){
      switch(address) {
        case REG8_MODE: 
          reg8_table[REG8_MODE] = radio_data->byte;
          return TRUE;
        case MREG_SETPOINT:
          bus_set(MOTOR_ADDR, MREG_SETPOINT, DEG_TO_OUTPUT_BODY((int8_t)radio_data->byte));
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

  // Calls the main mode loop (see modes.c)
  while (1) {
    
  }

  return 0;
}

