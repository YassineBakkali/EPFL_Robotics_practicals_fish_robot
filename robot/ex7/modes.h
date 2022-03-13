#ifndef __MODES_H
#define __MODES_H

/// Idle mode: do nothing

#define IMODE_IDLE            0
#define IMODE_GO_FORWARD      1
#define IMODE_TURN_RIGHT      2
#define IMODE_TURN_LEFT       3

#define REG_AMP               1
#define REG_FREQ              2
#define REG_PHI               3

#define REG_LED         4
#define LED_MODE_OFF    0

typedef enum Mode{FORWARD, LEFT, RIGHT} Mode;

/// The main loop for mode switching
void main_mode_loop(void);

#endif // __MODES_H
