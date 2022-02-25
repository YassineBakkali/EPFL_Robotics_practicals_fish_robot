#include "hardware.h"
#include "registers.h"

static uint32_t datavar = 0;
static uint8_t last_mb_size = 0;
static uint8_t counter = 0;
static uint8_t mb_buffer[MAX_MB_SIZE];

/* Register callback function, handles some new registers on the radio.
 * All these registers are of course completely useless, but it demonstrates how
 * to implement a register callback function, and what it can do.
 */
static int8_t register_handler(uint8_t operation, uint8_t address, RadioData* radio_data)
{
  uint8_t i;
  
  switch (operation)
  {
    case ROP_READ_8:
      if (address == 6) {
        radio_data->byte = counter;
        counter = 0;
        return TRUE;
      } else if (address == 21) {
        counter++;
        radio_data->byte = 0x42;
        return TRUE;
      }
      break;
    case ROP_READ_32:
      if (address == 2) {
        radio_data->dword = datavar;
        return TRUE;
      }
      break;
    case ROP_READ_MB:
      if (address == 2) {
        radio_data->multibyte.size = last_mb_size;
        for (i = 0; i < last_mb_size; i++) {
          radio_data->multibyte.data[i] = mb_buffer[i];
        }
        return TRUE;
      }
      break;
    case ROP_WRITE_8:
      if (address >= 2 && address <= 4) {
        mb_buffer[address - 2] = radio_data->byte;
        return TRUE;
      }
    case ROP_WRITE_16:
      if (address == 7) {
        datavar = (datavar * 3) + radio_data->word;
        return TRUE;
      }
      break;
    case ROP_WRITE_MB:
      if (address == 2) {
        last_mb_size = radio_data->multibyte.size;
        for (i = 0; i < last_mb_size; i++) {
          mb_buffer[i] = radio_data->multibyte.data[i] + 4;
        }
        return TRUE;
      }
      break;
  }
  return FALSE;
}

int main(void)
{
  hardware_init();
  
  // Registers the register handler callback function
  radio_add_reg_callback(register_handler);
  
  // Changes the color of the led (first red then green) to show the boot
  // and then say that the module is ready.
  set_color_i(4, 0);
  pause(ONE_SEC);
  set_color_i(2, 0);
  
  // Keeps the LED blinking in green to demonstrate that the main program is
  // still running and registers are processed in background.
  while (1) {
    set_color_i(2, 0);
    pause(ONE_SEC / 2);
    set_color_i(0, 0);
    pause(ONE_SEC / 2);
  }

  return 0;
}
