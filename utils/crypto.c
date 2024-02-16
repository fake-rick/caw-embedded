#include "crypto.h"

uint8_t crc8_ccit_update(uint8_t in_crc, uint8_t in_data) {
  uint8_t i;
  uint8_t data;
  data = in_crc ^ in_data;
  for (i = 0; i < 8; i++) {
    if ((data & 0x80) != 0) {
      data <<= 1;
      data ^= 0x07;
    } else {
      data <<= 1;
    }
  }
  return data;
}