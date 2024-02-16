#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#include <stdint.h>

uint8_t crc8_ccit_update(uint8_t in_crc, uint8_t in_data);

#endif