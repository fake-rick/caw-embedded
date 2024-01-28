#include "endian.h"

float endian_f32(float v) {
  float_conv_T d1, d2;
  d1.f = v;
  d2.c[0] = d1.c[3];
  d2.c[1] = d1.c[2];
  d2.c[2] = d1.c[1];
  d2.c[3] = d1.c[0];
  return d2.f;
}

uint16_t endian_u16(uint16_t v) {
  return (((uint16_t)(v) >> 8) | ((uint16_t)(v) << 8));
}

uint32_t endian_u32(uint32_t v) {
  return ((v >> 24) | (v << 24) | ((v & 0x00ff0000) >> 8) |
          ((v & 0x0000ff00) << 8));
}

uint64_t endian_u64(uint64_t v) {
  return ((uint64_t)endian_u32(v) << 32 | (uint64_t)endian_u32(v >> 32));
}