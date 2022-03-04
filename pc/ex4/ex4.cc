#include <iostream>
#include "remregs.h"
#include "robot.h"
#include <regdefs.h>
#include "utils.h"
#include <math.h>
#include <stdio.h>

#define MODE_ON  1
#define MODE_OFF 0

#define FREQUENCY 1
#define AMP_DEG   40

#define REG8_MODE 0
#define MREG_SETPOINT 0x2F

#define IMODE_MOTOR_SETPOINT 2

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

void start_read_setpoint(CRemoteRegs &regs){
  regs.set_reg_b(REG8_MODE, IMODE_MOTOR_SETPOINT);
}

int main()
{
  CRemoteRegs regs;
  int8_t setpoint;

  if (!init_radio_interface(INTERFACE, RADIO_CHANNEL, regs)) {
    return 1;
  }

  // Reboots the head microcontroller to make sure it is always in the same state
  reboot_head(regs);


  start_read_setpoint(regs);
  while(!kbhit()){
    setpoint = (int8_t)AMP_DEG*sin(2*M_PI*FREQUENCY*time_d());
    cout << "setpoint is : " << (int) setpoint << "       \r";
    send_setpoint((uint8_t)setpoint, regs);
  }
  stop_robot(regs);
  regs.close();
  return 0;
}
