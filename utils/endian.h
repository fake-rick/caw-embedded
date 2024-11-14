#ifndef __ENDIAN_H__
#define __ENDIAN_H__
#include <stdint.h>

typedef union {
  float f;
  char c[4];
} float_conv_T;

float endian_f32(float v);
uint16_t endian_u16(uint16_t v);
uint32_t endian_u32(uint32_t v);
uint64_t endian_u64(uint64_t v);

#endif