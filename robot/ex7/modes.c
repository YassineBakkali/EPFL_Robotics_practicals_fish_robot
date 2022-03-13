#include "config.h"
#include "modes.h"
#include "robot.h"
#include "module.h"
#include "registers.h"
#include "hardware.h"
#include "regdefs.h"
#include "can.h"
#include <stdint.h>

#define BODY_NUMBER           2

#undef  FREQ_MAX 
#define FREQ_MAX              1.5f

#define PHI_MIN               0.f
#define PHI_MAX M_PI

#define STEERING_ANGLE        20
#define FIN_CORRECTION_RATIO  1.6f
#define SIDE_FIN_AMP_RATIO    2

const uint8_t MOTOR_ADDR[BODY_NUMBER] = {72, 21};
const uint8_t LIMB_NUMBER[BODY_NUMBER] = {3, 1};

// set default values
static uint8_t amplitude = ENCODE_PARAM_8(20, AMPLITUDE_MIN, AMPLITUDE_MAX); // 20 deg
static uint8_t frequency = ENCODE_PARAM_8(1, FREQ_MIN, FREQ_MAX); // 1 Hz
static uint8_t phi = ENCODE_PARAM_8(0, PHI_MIN, PHI_MAX); // phase difference of pi

static void turn_off_all_leds(void) {
  uint8_t i = 0;
  for (i = 0; i < BODY_NUMBER; ++i) {
    set_reg_value_dw(MOTOR_ADDR[i], MREG32_LED, 0);
  }
  set_rgb(0, 0, 0);
}

static void move_mode(Mode mode) {
  uint32_t dt, cycletimer;
  float my_time, delta_t, l, l_offset;
  int8_t l_rounded, l_offset_rounded;

  cycletimer = getSysTICs();
  my_time = 0;

  // Initialises the body module with the specified address (but do not start
  // the PD controller)
  uint8_t i = 0;
  uint8_t j = 0;
  for(i = 0; i < BODY_NUMBER; ++i ){
    init_body_module(MOTOR_ADDR[i]);
    for(j = 0; j < LIMB_NUMBER[i]; ++j){
      init_limb_module(MOTOR_ADDR[i] + j);
      start_pid(MOTOR_ADDR[i] + j);
      pause(HALF_SEC);
    }
  }
  
  do {
    // Calculates the delta_t in seconds and adds it to the current time
    dt = getElapsedSysTICs(cycletimer);
    cycletimer = getSysTICs();
    delta_t = (float) dt / sysTICSperSEC;
    my_time += delta_t;

    float f_amplitude = DECODE_PARAM_8(amplitude ,AMPLITUDE_MIN, AMPLITUDE_MAX);
    float f_freq = DECODE_PARAM_8(frequency ,FREQ_MIN, FREQ_MAX);
    float f_phi = DECODE_PARAM_8(phi, PHI_MIN, PHI_MAX);

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

    if(f_phi > PHI_MAX){
      f_phi = PHI_MAX;
    } else if (f_phi < PHI_MIN){
      f_phi = PHI_MIN;
    }

    // Calculates the sine wave
    l = f_amplitude * sin(M_TWOPI * f_freq * my_time);
    l_rounded = (int8_t) l;
    int8_t l_rounded_corr = (int8_t)((float)l_rounded * FIN_CORRECTION_RATIO);

    l_offset = f_amplitude * sin(M_TWOPI * f_freq * my_time + f_phi);
    l_offset_rounded = (int8_t) l_offset;
    switch (mode) {
      case FORWARD:
        bus_set(MOTOR_ADDR[0] + 1, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(l_rounded/SIDE_FIN_AMP_RATIO));
        pause(ONE_MS);
        bus_set(MOTOR_ADDR[0] + 2, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(l_rounded/SIDE_FIN_AMP_RATIO));
        pause(ONE_MS);
        bus_set(MOTOR_ADDR[1], MREG_SETPOINT, DEG_TO_OUTPUT_BODY(l_rounded));
        pause(ONE_MS);
        bus_set(MOTOR_ADDR[0], MREG_SETPOINT, DEG_TO_OUTPUT_BODY(l_offset_rounded));
        pause(ONE_MS);
        break;
      case LEFT:
        bus_set(MOTOR_ADDR[0] + 1, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(0.0));
        bus_set(MOTOR_ADDR[0] + 1, MREG_MODE, MODE_IDLE);
        bus_set(MOTOR_ADDR[0] + 2, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(l_rounded/SIDE_FIN_AMP_RATIO));
        bus_set(MOTOR_ADDR[1], MREG_SETPOINT, DEG_TO_OUTPUT_BODY(l_rounded));
        bus_set(MOTOR_ADDR[0], MREG_SETPOINT, DEG_TO_OUTPUT_BODY(-STEERING_ANGLE));
        break;
      case RIGHT:
        bus_set(MOTOR_ADDR[0] + 2, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(0.0));
        bus_set(MOTOR_ADDR[0] + 2, MREG_MODE, MODE_IDLE);
        bus_set(MOTOR_ADDR[0] + 1, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(l_rounded_corr/SIDE_FIN_AMP_RATIO));
        bus_set(MOTOR_ADDR[1], MREG_SETPOINT, DEG_TO_OUTPUT_BODY(l_rounded));
        bus_set(MOTOR_ADDR[0], MREG_SETPOINT, DEG_TO_OUTPUT_BODY(STEERING_ANGLE));
        break;
    }

  } while (reg8_table[REG8_MODE] != MODE_IDLE);

  for (i = 0; i < BODY_NUMBER; ++i) {
    for (j = 0; j < LIMB_NUMBER[i]; ++j) {
      bus_set(MOTOR_ADDR[i] + j, MREG_SETPOINT, DEG_TO_OUTPUT_BODY(0.0));
      pause(HALF_SEC);
    }
  } 
  for (i = 0; i < BODY_NUMBER; ++i) {
    for (j = 0; j < LIMB_NUMBER[i]; ++j) {
      bus_set(MOTOR_ADDR[i] + j, MREG_MODE, MODE_IDLE);
      pause(HALF_SEC);
    }
  }
}

static int8_t register_handler(uint8_t operation, uint8_t address, RadioData* radio_data)
{
    if (operation == ROP_WRITE_8){
      switch(address) {
        case REG8_MODE: 
          reg8_table[REG8_MODE] = radio_data->byte;
          return TRUE;
        case REG_AMP:
          amplitude = radio_data->byte;
          return TRUE;
        case REG_FREQ:
          frequency = radio_data->byte;
          return TRUE;
        case REG_PHI:
          phi = radio_data->byte;
          return TRUE;
        case REG_LED:
          turn_off_all_leds();
          return TRUE;
      }
    }
  return FALSE;
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
      case IMODE_GO_FORWARD:
        move_mode(FORWARD);
        break;
      case IMODE_TURN_RIGHT:
        move_mode(RIGHT);
        break;
      case IMODE_TURN_LEFT:
        move_mode(LEFT);
        break;
      default:
        reg8_table[REG8_MODE] = IMODE_IDLE;
    }
  }
}