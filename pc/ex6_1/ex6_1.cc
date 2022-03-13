#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <windows.h>
#include "trkcli.h"
#include "utils.h"
#include "remregs.h"
#include "robot.h"

using namespace std;

#define REG_LED               1
#define LED_MODE_OFF          0

#define REG_LED_COLOR         0

#define POOL_LENGTH           6 // x max in m
#define POOL_WIDTH            2 // y max in m

#define GREEN_VALUE           64 
#define MAX_CHANNEL_VALUE     ((1 << 8) -1)

const uint8_t RADIO_CHANNEL = 201;         ///< robot radio channel
const char* INTERFACE = "COM1";            ///< robot radio interface

const char* TRACKING_PC_NAME = "biorobpc6";   ///< host name of the tracking PC
const uint16_t TRACKING_PORT = 10502;          ///< port number of the tracking PC

static void turn_off_leds(CRemoteRegs &regs) {
  regs.set_reg_b(REG_LED, LED_MODE_OFF);
}

static void set_led_color(CRemoteRegs &regs, uint32_t rgb) {
  regs.set_reg_dw(REG_LED_COLOR, rgb);
}

static uint32_t calculate_rgb_from_channels(uint8_t r, uint8_t g, uint8_t b){
  return ((uint32_t) r << 16) | ((uint32_t) g << 8) | b;
}

static uint32_t calculate_rgb_from_pos(CRemoteRegs &regs, double x, double y) {
  uint32_t rgb = 0;
  uint8_t r, g, b = 0;
  if (x < 0||x > POOL_LENGTH || y < 0 || y > POOL_WIDTH) {
    return calculate_rgb_from_channels(0, MAX_CHANNEL_VALUE, 0);
  }
  r = (uint8_t) (x/POOL_LENGTH * MAX_CHANNEL_VALUE);
  g = GREEN_VALUE;
  b = (uint8_t) (y/POOL_WIDTH * MAX_CHANNEL_VALUE);
  rgb = calculate_rgb_from_channels(r, g, b);
  cout << "x = " << x << ", y = " << y << ", R = " << (int) r << ", B = " << (int) b << ", RGB 0x" << hex << rgb << "   " << '\r' << flush;
  return rgb;
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

  // turn off all leds
  turn_off_leds(regs);
  uint32_t rgb = calculate_rgb_from_channels(0, 255, 0);
  set_led_color(regs, rgb);

  while (!kbhit()){ 
    uint32_t frame_time;
    // Gets the current position
    if (!trk.update(frame_time)){
      return 1;
    }
    double x, y;
    cout.precision(2);
    
    // Gets the ID of the first spot (the tracking system supports multiple ones)
    int id = trk.get_first_id();

    // Reads its coordinates (if (id == -1), then no spot is detected)
    if (id != -1 && trk.get_pos(id, x, y)){
      //cout << "(" << fixed << x << ", " << y << ")" << " m      \r";
      set_led_color(regs, calculate_rgb_from_pos(regs, x, y));
    } else {
      cout << "(not detected)" << '\r';
    }

    // Waits 10 ms before getting the info next time (anyway the tracking runs at 15 fps)
    Sleep(10);
  }

  regs.close();
  return 0;
}