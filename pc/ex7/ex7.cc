#include <iostream>
#include "remregs.h"
#include "robot.h"
#include <regdefs.h>
#include "utils.h"
#include <math.h>
#include <stdio.h>
#include "trkcli.h"

using namespace std;

#define REG8_MODE             0
#define IMODE_IDLE            0
#define IMODE_GO_FORWARD      1
#define IMODE_TURN_RIGHT      2
#define IMODE_TURN_LEFT       3

#define REG_AMP               1
#define REG_FREQ              2
#define REG_PHI               3

#undef FREQ_MAX
#define FREQ_MAX 1.5f

#define PHI_MIN 0.5f
#define PHI_MAX 1.5f

#define REG_LED         4
#define LED_MODE_OFF    0

#define REG_LED_COLOR   0

#define GREEN_VALUE     64 
#define MAX_CHANNEL_VALUE (2^8-1)

#define DISTANCE        4 // in m

const uint8_t RADIO_CHANNEL = 201;         ///< robot radio channel
const char* INTERFACE = "COM1";            ///< robot radio interface


const char* TRACKING_PC_NAME = "biorobpc11";   ///< host name of the tracking PC
const uint16_t TRACKING_PORT = 10502;          ///< port number of the tracking PC

void send_amplitude(CRemoteRegs &regs, float amplitude){
  regs.set_reg_b(REG_AMP, ENCODE_PARAM_8(amplitude, AMPLITUDE_MIN, AMPLITUDE_MAX));
}

void send_freq(CRemoteRegs &regs, float frequency){
  regs.set_reg_b(REG_FREQ, ENCODE_PARAM_8(frequency, FREQ_MIN, FREQ_MAX));
}

void send_phi(CRemoteRegs &regs, float phi) {
  regs.set_reg_b(REG_PHI, ENCODE_PARAM_8(phi, PHI_MIN, PHI_MAX));
}

void go_forward(CRemoteRegs &regs){
  regs.set_reg_b(REG8_MODE, IMODE_GO_FORWARD);
}

void turn_left(CRemoteRegs &regs){
  regs.set_reg_b(REG8_MODE, IMODE_TURN_LEFT);
}

void turn_right(CRemoteRegs &regs){
  regs.set_reg_b(REG8_MODE, IMODE_TURN_RIGHT);
}

void stop_robot(CRemoteRegs &regs){
  regs.set_reg_b(REG8_MODE, IMODE_IDLE);
}

void measure_robot_speed(CRemoteRegs &regs, CTrackingClient trk) { // pass trk by ref?

    stop_robot(regs);
    uint32_t frame_time;
    // Gets the current position
    if (!trk.update(frame_time)) {
      return 1;
    }
    double x0, y0;
    
    // Gets the ID of the first spot (the tracking system supports multiple ones)
    int id = trk.get_first_id();

    // Reads its coordinates (if (id == -1), then no spot is detected)
    if (id != -1 && trk.get_pos(id, x0, y0)) {
      double t0 = time_d();
      go_forward(regs);
      while (1) {
        double dist = 0;
        if (!trk.update(frame_time)) {
          return 1;
        }
        double x, y;
        if (id != -1 && trk.get_pos(id, x, y)) {
          if((x - x0)^2 + (y - y0)^2 >= DISTANCE^2) {
            double speed = sqrt((x - x0)^2 + (y - y0)^2)/(time_d()-t0);
            cout << "Measured speed: " << speed << " m/s" << endl;
            stop_robot(regs);
            return;
          }
        }
      }
    } else {
      cout << "(initial position not detected)" << '\r';
    }
}

void turn_off_leds(CRemoteRegs &regs) {
  regs.set_reg_b(REG_LED, LED_MODE_OFF);
}

int main()
{
  CRemoteRegs regs;
  CTrackingClient trk;

  if (!init_radio_interface(INTERFACE, RADIO_CHANNEL, regs)) {
    return 1;
  }

  // Connects to the tracking server
  if (!trk.connect(TRACKING_PC_NAME, TRACKING_PORT)) {
    return 1;
  }

  // Reboots the head microcontroller to make sure it is always in the same state
  reboot_head(regs);

  if (!init_radio_interface(INTERFACE, RADIO_CHANNEL, regs)) {
    return 1;
  }

  float amplitude = 0;
  float frequency = 0;
  float phi = 0;
  DWORD key = 0;

  // Reboots the head microcontroller to make sure it is always in the same state
  reboot_head(regs);
  turn_off_leds(regs);

  cout << "A: Set amplitude" << endl;
  cout << "F: Set frequency" << endl;
  cout << "P: Set phase" << endl;
  cout << "UP ARROW : Go forward" << endl;
  cout << "RIGHT ARROW : Go right" << endl;
  cout << "LEFT ARROW : Go left" << endl;
  cout << "S : Stop the robot" << endl;
  cout << "M : Measure robot speed" << endl;
  cout << "SPACE : Exit program" << endl;

  while(key != SPACE_KEYCODE){
    if(key == A_KEYCODE){
      cout << "Please enter an amplitude : " << endl;
      cin >> amplitude; 
      send_amplitude(regs, amplitude);
    }
    if(key == F_KEYCODE){
      cout << endl << "Please enter a frequency :" << endl;
      cin >> frequency;
      send_freq(regs, frequency);
    }
    if (key == P_KEYCODE) {
      cout << endl << "Please enter a phase :" << endl;
      cin >> phi;
      send_freq(regs, phi);
    }
    if (key == FORWARD_KEYCODE) {
      go_forward(regs);
    }
    if (key == RIGHT_KEYCODE) {
      turn_right(regs);
    }
    if (key == LEFT_KEYCODE) {
      turn_left(regs);
    }
    if (key == S_KEYCODE) {
      stop_robot(regs);
    }
    if (key == M_KEYCODE) {
      measure_robot_speed(regs, trk);
    }
    key = ext_key();
  }

  stop_robot(regs);
  regs.close();
  return 0;
}
