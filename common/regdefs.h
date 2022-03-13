#ifndef __REGDEFS_H
#define __REGDEFS_H

#include <stdint.h>

/// Encodes a floating point number to a byte, within the specified range
#define ENCODE_PARAM_8(p,pmin,pmax) ((uint8_t) ((p - pmin) / (float) (pmax-pmin) * 255.0))
/// Decodes a floating point number from a byte, using the specified range
#define DECODE_PARAM_8(b,pmin,pmax) (pmin + (pmax-pmin) * (float) b / 255.0)

// Several keycodes for ext_key() function
// These were the values that were returned by the ext_key() function.
#define SPACE_KEYCODE       2097184
#define A_KEYCODE           4259937
#define F_KEYCODE           4587622
#define FORWARD_KEYCODE     2490368
#define LEFT_KEYCODE        2424832
#define RIGHT_KEYCODE       2555904
#define S_KEYCODE           5439603
#define M_KEYCODE           5046381
#define P_KEYCODE           5242992

// For exercise 2 :

// Number of body modules on the robot
#define BODY_NUMBER  2

// Total number of limbs on a body module
#define LIMB_NUMBER_MAX 3

// For exercise 5 :

// Sine amplitude register address
#define REG_AMP   1

// Sine frequency register address
#define REG_FREQ  2

#define AMPLITUDE_MAX 60.0f
#define AMPLITUDE_MIN 0.0f

#define FREQ_MAX 2.0f
#define FREQ_MIN 0.0f

/// Declaration of all 8-bit radio registers
enum {
  REG8_MODE,
  REG8_MAX
};

/// Declaration of all 16-bit radio registers
enum {
  REG16_VER,
  REG16_MAX
};

/// Declaration of all 32-bit radio registers
enum {
  REG32_LED,
  REG32_MAX
};

#endif
