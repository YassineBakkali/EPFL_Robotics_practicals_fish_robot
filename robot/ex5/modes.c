#include "config.h"
#include "modes.h"
#include "robot.h"
#include "module.h"
#include "registers.h"
#include "hardware.h"
#include "regdefs.h"
#include <stdint.h>

const uint8_t MOTOR_ADDR = 21;

static uint8_t amplitude = 40;
static uint8_t frequency = 1;

static int8_t register_handler(uint8_t operation, uint8_t address, RadioData* radio_data)
{
    if (operation == ROP_WRITE_8){
      switch(address) {
        case REG8_MODE: 
          reg8_table[REG8_MODE] = radio_data->byte;
          return TRUE;
        case REG_AMP:
          amplitude = radio_data->byte;
          bus_set(MOTOR_ADDR, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(0.0));
          return TRUE;
        case REG_FREQ:
          frequency = radio_data->byte;
          bus_set(MOTOR_ADDR, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(0.0));
          return TRUE;
      }
    }
  return FALSE;
}

void sine_demo_mode()
{
  uint32_t dt, cycletimer;
  float my_time, delta_t, l;
  int8_t l_rounded;

  cycletimer = getSysTICs();
  my_time = 0;

  init_body_module(MOTOR_ADDR);
  start_pid(MOTOR_ADDR);
  set_color(3);
  
  do {
    // Calculates the delta_t in seconds and adds it to the current time
    dt = getElapsedSysTICs(cycletimer);
    cycletimer = getSysTICs();
    delta_t = (float) dt / sysTICSperSEC;
    my_time += delta_t;

    float f_amplitude = DECODE_PARAM_8(amplitude ,AMPLITUDE_MIN, AMPLITUDE_MAX);
    float f_freq = DECODE_PARAM_8(frequency ,FREQ_MIN, FREQ_MAX);
    
    // Amplitude and frequency are capped
    if(f_freq > FREQ_MAX){
      f_freq = FREQ_MAX;
    } else if (f_freq < FREQ_MIN){
      f_freq = FREQ_MIN;
    }
    if(f_amplitude > AMPLITUDE_MAX){
      f_amplitude = AMPLITUDE_MAX;
    } else if (f_amplitude < AMPLITUDE_MIN){
      f_amplitude = AMPLITUDE_MIN;
    }

    // Calculates the sine wave
    l = f_amplitude * sin(M_TWOPI * f_freq * my_time);
    l_rounded = (int8_t) l;

    // Outputs the sine wave to the motor
    bus_set(MOTOR_ADDR, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(l_rounded));

    // Make sure there is some delay, so that the timer output is not zero
    pause(ONE_MS);

  } while (reg8_table[REG8_MODE] == IMODE_SINE_DEMO);

  bus_set(MOTOR_ADDR, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(0.0));
  pause(ONE_SEC);
  bus_set(MOTOR_ADDR, MREG_MODE, MODE_IDLE);
  // Back to the "normal" green
  set_color(2);
}

void main_mode_loop()
{
  radio_add_reg_callback(register_handler);
  reg8_table[REG8_MODE] = IMODE_IDLE;

  while (1)
  {
    switch(reg8_table[REG8_MODE])
    {
      case IMODE_IDLE:
        break;
      case IMODE_SINE_DEMO:
        sine_demo_mode();
        break;
      default:
        reg8_table[REG8_MODE] = IMODE_IDLE;
    }
  }
}
