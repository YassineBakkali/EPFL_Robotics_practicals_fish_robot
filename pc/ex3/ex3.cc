#include <iostream>
#include "remregs.h"
#include "robot.h"
#include "regdefs.h"
#include "utils.h"
#include <stdio.h>

using namespace std;

const uint8_t RADIO_CHANNEL = 201;         ///< robot radio channel
const char* INTERFACE = "COM1";            ///< robot radio interface

void display_limbs_pos(CRemoteRegs& regs)
{
  uint8_t data_buffer[32], len;
    if (regs.get_reg_mb(0, data_buffer, len)) {
      for (uint8_t i = 0; i < len; i++) {
        cout << (int)((int8_t)data_buffer[i]) << ( i == len-1 ? "       \r" : ", " ) ;
      }
    } else {
      cerr << "Unable to read multibyte register." << endl;
    }
}

int main()
{
  CRemoteRegs regs;

  if (!init_radio_interface(INTERFACE, RADIO_CHANNEL, regs)) {
    return 1;
  }

  // Reboots the head microcontroller to make sure it is always in the same state
  reboot_head(regs);
  
  // Register display demo
  while(!kbhit()){
    display_limbs_pos(regs);
  }

  regs.close();
  return 0;
}
