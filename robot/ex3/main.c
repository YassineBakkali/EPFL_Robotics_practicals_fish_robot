#include "hardware.h"
#include "module.h"
#include "robot.h"
#include "registers.h"
#include "regdefs.h"

// list of the number of limbs for its corresponding body
const uint8_t LIMB_NUMBER[BODY_NUMBER] = {3, 1};
// list of body address
const uint8_t MOTOR_ADDR[BODY_NUMBER] = {72, 21};

// table of position of the limbs corresponding to a body
static int8_t pos[LIMB_NUMBER_MAX][BODY_NUMBER] = {0}; 

static int8_t register_handler(uint8_t operation, uint8_t address, RadioData* radio_data)
{
  uint8_t i, j, size_sum = 0;
    if (operation == ROP_READ_MB){
      // send position values over radio
      if (address < BODY_NUMBER) {
        for (j = 0 ; j < BODY_NUMBER ; ++j){
          for (i = 0; i < LIMB_NUMBER[j]; i++) {
            radio_data->multibyte.data[size_sum] = pos[i][j];
            ++size_sum; 
          } 
        }
        radio_data->multibyte.size = size_sum;
        return TRUE;
      }
    }
  return FALSE;
}

int main(void)
{
  hardware_init();
  radio_add_reg_callback(register_handler);
  // Changes the color of the led (red) to show the boot
  set_color_i(4, 0);

  // Initialises the body module with the specified address (but do not start
  // the PD controller)
  for(uint8_t i = 0; i < BODY_NUMBER; ++i ){
    init_body_module(MOTOR_ADDR[i]);
    for(uint8_t j = 0; j < LIMB_NUMBER[i]; ++j){
      init_limb_module(MOTOR_ADDR[i] + j);
    }
  }

  // get position values of each motor
  while (1) {
    for(uint8_t i = 0; i < BODY_NUMBER; ++i ){
      for(uint8_t j = 0; j < LIMB_NUMBER[i]; ++j ){
        pos[j][i] = bus_get(MOTOR_ADDR[i]+j, MREG_POSITION);
      }
    }
  }
  return 0;
}
