#include "hardware.h"
#include "module.h"
#include "robot.h"
#include "registers.h"

//const uint8_t MOTOR_ADDR = 21; 
#define BODY_NUMBER  7
#define LIMB_NUMBER_MA  3

// DO NOT FORGET TO UPDATE THE BODY IDs + LIMB NUMBERs
const uint8_t LIMB_NUMBER[BODY_NUMBER] = {2,0,0,0,2,0,2}
const uint8_t MOTOR_ADDR[BODY_NUMBER] = {1,2,3,4,5,6,7};

static int8_t pos[LIMB_NUMBER][BODY_NUMBER] = {0}; 

static int8_t register_handler(uint8_t operation, uint8_t address, RadioData* radio_data)
{
  uint8_t i;
    if (operation == ROP_READ_MB){
      if (address < BODY_NUMBER) {
        radio_data->multibyte.size = LIMB_NUMBER[address] + 1;
        for (i = 0; i <= LIMB_NUMBER[address]; i++) {
          radio_data->multibyte.data[i] = pos[i][address];
        }
        return TRUE;
      }
    }
  return FALSE;
}

int main(void)
{
  // int8_t pos[LIMB_NUMBER][BODY_NUMBER] = {0}; 

  hardware_init();
  
  // Changes the color of the led (red) to show the boot
  set_color_i(4, 0);

  // Initialises the body module with the specified address (but do not start
  // the PD controller)
  for(uint8_t i = 0; i < BODY_NUMBER; ++i ){
    init_body_module(MOTOR_ADDR[i]);
  }
  
  // And then... do this
  while (1) {
    for(uint8_t i = 0; i < BODY_NUMBER; ++i ){
      for(uint8_t j = 0; j <= LIMB_NUMBER[i]; ++j ){
        pos[j][i] = bus_get(MOTOR_ADDR[i]+j, MREG_POSITION);
      }
      radio_add_reg_callback(register_handler);
    }
    // if (pos > 0) {
    //   set_rgb(pos, 32, 0);
    // } else {
    //   pos = -pos;
    //   set_rgb(0, 32, pos);
    // }
  }
  return 0;
}
