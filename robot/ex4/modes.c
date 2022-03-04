#include "config.h"
#include "modes.h"
#include "robot.h"
#include "module.h"
#include "registers.h"
#include "hardware.h"

const uint8_t MOTOR_ADDR = 21; 
static int8_t pos = 0;

static int8_t register_handler(uint8_t operation, uint8_t address, RadioData* radio_data)
{
    if (operation == ROP_WRITE_8){
      switch(address) {
        case REG8_MODE: 
          reg8_table[REG8_MODE] = radio_data->byte;
          return TRUE;
        case MREG_SETPOINT:
          pos = (int8_t)radio_data->byte;
          return TRUE;
      }
    }
  return FALSE;
}

static void motor_demo_mode(void)
{
  init_body_module(MOTOR_ADDR);
  start_pid(MOTOR_ADDR);
  set_color(4);
  while (reg8_table[REG8_MODE] == IMODE_MOTOR_DEMO) {
    bus_set(MOTOR_ADDR, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(21.0));
    pause(ONE_SEC);
    bus_set(MOTOR_ADDR, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(-21.0));
    pause(ONE_SEC);
  }
  bus_set(MOTOR_ADDR, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(0.0));
  pause(ONE_SEC);
  bus_set(MOTOR_ADDR, MREG_MODE, MODE_IDLE);
  set_color(2);
}

static void read_setpoint_mode(void)
{
  init_body_module(MOTOR_ADDR);
  start_pid(MOTOR_ADDR);
  set_color(3);
  while(reg8_table[REG8_MODE] == IMODE_MOTOR_SETPOINT) {
    bus_set(MOTOR_ADDR, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(pos));
  }
  bus_set(MOTOR_ADDR, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(0.0));
  pause(ONE_SEC);
  bus_set(MOTOR_ADDR, MREG_MODE, MODE_IDLE);
  set_color(2);
}

void main_mode_loop()
{
  reg8_table[REG8_MODE] = IMODE_IDLE;
  radio_add_reg_callback(register_handler);
  while (1)
  {
    switch(reg8_table[REG8_MODE])
    {
      case IMODE_IDLE:
        break;
      case IMODE_MOTOR_DEMO:
        motor_demo_mode();
        break;
      case IMODE_MOTOR_SETPOINT:
        read_setpoint_mode();
        break;
      default:
        reg8_table[REG8_MODE] = IMODE_IDLE;
    }
  }
}
