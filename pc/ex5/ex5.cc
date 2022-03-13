#include <iostream>
#include "remregs.h"
#include "robot.h"
#include <regdefs.h>
#include "utils.h"
#include <math.h>
#include <stdio.h>

using namespace std;

#define REG8_MODE 0

#define IMODE_SINE_DEMO     2
#define IMODE_IDLE          0

const uint8_t RADIO_CHANNEL = 201;         ///< robot radio channel
const char* INTERFACE = "COM1";            ///< robot radio interface

void send_amplitude(CRemoteRegs &regs, float amplitude){
  if (amplitude < AMPLITUDE_MIN){
    amplitude = AMPLITUDE_MIN;
  } else if (amplitude > AMPLITUDE_MAX){
    amplitude = AMPLITUDE_MAX;
  }
  regs.set_reg_b(REG_AMP, ENCODE_PARAM_8(amplitude,AMPLITUDE_MIN,AMPLITUDE_MAX));
}

void send_freq(CRemoteRegs &regs, float frequency){
  if (frequency < FREQ_MIN){
    frequency = FREQ_MIN;
  } else if (frequency > FREQ_MAX){
    frequency = FREQ_MAX;
  }
  regs.set_reg_b(REG_FREQ, ENCODE_PARAM_8(frequency,FREQ_MIN,FREQ_MAX));
}

void start_sine_demo(CRemoteRegs &regs){
  regs.set_reg_b(REG8_MODE, IMODE_SINE_DEMO);
}

void stop_robot(CRemoteRegs &regs){
  regs.set_reg_b(REG8_MODE, IMODE_IDLE);
}

int main()
{
  CRemoteRegs regs;

  if (!init_radio_interface(INTERFACE, RADIO_CHANNEL, regs)) {
    return 1;
  }

  float amplitude = 0;
  float frequency = 0;
  DWORD key = 0;

  // Reboots the head microcontroller to make sure it is always in the same state
  reboot_head(regs);
  start_sine_demo(regs);

  cout << "Press A to change the amplitude [0 to 60] of the sine, F for the frequency [0 to 2] and SPACE to exit the program" << endl;
  while(key != SPACE_KEYCODE){
    if( key == A_KEYCODE){
      cout << "Please enter an amplitude : " << endl;
      cin >> amplitude; 
      send_amplitude(regs, amplitude);
    }
    if( key == F_KEYCODE){
      cout << endl << "Please enter a frequency :" << endl;
      cin >> frequency;
      send_freq(regs, frequency);
    }
    key = ext_key();
  }

  stop_robot(regs);
  regs.close();
  return 0;
}
