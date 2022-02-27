#include <iostream>
#include "remregs.h"
#include "robot.h"

using namespace std;

#define BODY_NUMBER  7

const uint8_t RADIO_CHANNEL = 201;         ///< robot radio channel
const char* INTERFACE = "COM1";            ///< robot radio interface


// Displays the contents of a multibyte register as a list of bytes
void display_multibyte_register(CRemoteRegs& regs, const uint8_t addr)
{
  uint8_t data_buffer[32], len;
  if (regs.get_reg_mb(addr, data_buffer, len)) {
    cout << (int) len << " pos for body " << addr << ": ";
    for (unsigned int i(0); i < len; i++) {
      if (i > 0) cout << ", ";
      cout << (int) data_buffer[i];
    }
    cout << endl;
  } else {
    cerr << "Unable to read multibyte register." << endl;
  }
}

void read_position(CRemoteRegs& regs)
{
    for(uint8_t i = 0; i < BODY_NUMBER; ++i ){
        display_multibyte_register(regs, i);
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
  while(1){
    read_position(regs);
  }
  regs.close();
  return 0;
}
