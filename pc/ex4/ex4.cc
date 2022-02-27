#include <iostream>
#include "remregs.h"
#include "robot.h"
#include <regdefs.h>
#include <utils.h>
#include <math.h>
#include "module.h"



#define MODE_ON  1
#define MODE_OFF 0

#define FREQUENCY 1
#define AMP_DEG   40

using namespace std;

const uint8_t RADIO_CHANNEL = 201;         ///< robot radio channel
const char* INTERFACE = "COM1";            ///< robot radio interface

void start_robot(CRemoteRegs &regs) {
  regs.set_reg_b(REG8_MODE, MODE_ON);
}

void stop_robot(CRemoteRegs &regs) {
  regs.set_reg_b(REG8_MODE, MODE_OFF); 
}

void send_setpoint(uint8_t setpoint_deg, CRemoteRegs &regs) {
  regs.set_reg_b(MREG_SETPOINT, setpoint_deg);
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
  int8_t setpoint;
  while(1){
    setpoint = (int8_t)AMP_DEG*sin(2*M_PI*FREQUENCY*time_d());
    send_setpoint((uint8_t)setpoint, regs);
  }

  regs.close();
  return 0;
}
