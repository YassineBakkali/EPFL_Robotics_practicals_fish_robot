#include <iostream>
#include "remregs.h"
#include "robot.h"
#include <regdefs.h>
#include "utils.h"
#include <math.h>
#include <stdio.h>
#include "trkcli.h"
#include <fstream>
#include <sstream>

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

#define DISTANCE_MAX        1 // in m

const uint8_t RADIO_CHANNEL = 201;         ///< robot radio channel
const char* INTERFACE = "COM1";            ///< robot radio interface

const char* TRACKING_PC_NAME = "biorobpc6";   ///< host name of the tracking PC
const uint16_t TRACKING_PORT = 10502;          ///< port number of the tracking PC

static float amplitude = 0;
static float frequency = 0;
static float phi = 0;

void send_amplitude(CRemoteRegs &regs, float amplitude){
  if (amplitude < AMPLITUDE_MIN){
    amplitude = AMPLITUDE_MIN;
  } else if (amplitude > AMPLITUDE_MAX){
    amplitude = AMPLITUDE_MAX;
  }
  regs.set_reg_b(REG_AMP, ENCODE_PARAM_8(amplitude, AMPLITUDE_MIN, AMPLITUDE_MAX));
}

void send_freq(CRemoteRegs &regs, float frequency){
  if (frequency < FREQ_MIN){
    frequency = FREQ_MIN;
  } else if (frequency > FREQ_MAX){
    frequency = FREQ_MAX;
  }
  regs.set_reg_b(REG_FREQ, ENCODE_PARAM_8(frequency, FREQ_MIN, FREQ_MAX));
}

void send_phi(CRemoteRegs &regs, float phi) {
  if (phi < PHI_MIN){
    phi = PHI_MIN;
  } else if (phi > PHI_MAX){
    phi = PHI_MAX;
  }
  regs.set_reg_b(REG_PHI, ENCODE_PARAM_8(phi, PHI_MIN, PHI_MAX));
}

void go_forward(CRemoteRegs &regs){
  cout << "Going forward" << endl;
  regs.set_reg_b(REG8_MODE, IMODE_GO_FORWARD);
}

void turn_left(CRemoteRegs &regs){
  cout << "Turning left" << endl;
  regs.set_reg_b(REG8_MODE, IMODE_TURN_LEFT);
}

void turn_right(CRemoteRegs &regs){
  cout << "Turning right" << endl;
  regs.set_reg_b(REG8_MODE, IMODE_TURN_RIGHT);
}

void stop_robot(CRemoteRegs &regs){
  cout << "Stop robot" << endl;
  regs.set_reg_b(REG8_MODE, IMODE_IDLE);
}

void measure_robot_speed(CRemoteRegs &regs, CTrackingClient &trk) { // pass trk by ref?

  cout << "Start robot speed measurement..." << endl;
  static int iter = 1;
  stop_robot(regs);
  uint32_t frame_time;
  // Gets the current position
  if (!trk.update(frame_time)) {
    return;
  }
  double x0, y0;
  // Gets the ID of the first spot (the tracking system supports multiple ones)
  int id = trk.get_first_id();

  // Reads its coordinates (if (id == -1), then no spot is detected)
  if (id != -1 && trk.get_pos(id, x0, y0)) {
    ofstream log;
    ostringstream filename;
    filename << iter << "_fish_position_freq_" << frequency << ".txt";
    log.open (filename.str());
    log << x0 << "," << y0 << endl;
    double t0 = time_d();
    go_forward(regs);
    while (1) {
      if (!trk.update(frame_time)) {
        return;
      }
      int id = trk.get_first_id();
      double x, y;
      double distance = 0;
      if (id != -1 && trk.get_pos(id, x, y)) {
        log << x << "," << y << endl;
        cout << "x = " << x << ", y = " << y << '\r' << flush;
        distance = sqrt((x - x0)*(x - x0) + (y - y0)*(y - y0));
        if(distance >= DISTANCE_MAX || kbhit()) {
          double speed = distance/(time_d()-t0);
          cout << "Measured speed: " << speed << " m/s" << endl;
          log << "speed : " << speed << endl;
          stop_robot(regs);
          log.close();
          ++iter;
          return;
        }
      }
    }
    log.close();
    ++iter;
  } else {
    cout << "(initial position not detected)" << '\r';
  }
}

void turn_off_leds(CRemoteRegs &regs) {
  regs.set_reg_b(REG_LED, LED_MODE_OFF);
}

static void set_led_color(CRemoteRegs &regs, uint32_t rgb) {
  regs.set_reg_dw(REG_LED_COLOR, rgb);
}

static uint32_t calculate_rgb_from_channels(uint8_t r, uint8_t g, uint8_t b){
  return ((uint32_t) r << 16) | ((uint32_t) g << 8) | b;
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

  DWORD key = 0;
  turn_off_leds(regs);
  uint32_t rgb = calculate_rgb_from_channels(0, 255, 0);
  set_led_color(regs, rgb);

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
      send_phi(regs, phi);
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
