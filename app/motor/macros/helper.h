#ifndef __HELPER_H__
#define __HELPER_H__

#include <math.h>

#define _2_SQRT3 1.15470053838f
#define _SQRT3 1.73205080757f
#define _1_SQRT3 0.57735026919f
#define _SQRT3_2 0.86602540378f
#define _SQRT2 1.41421356237f
#define _120_D2R 2.09439510239f
#define _PI 3.14159265359f
#define _PI_2 1.57079632679f
#define _PI_3 1.0471975512f
#define _2PI 6.28318530718f
#define _3PI_2 4.71238898038f
#define _PI_6 0.52359877559f
#define _RPM_TO_RADS 0.10471975512f

#define _constrain(amt, low, high) \
  ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

#define _abs(x) ((x) > 0 ? (x) : -(x))

// Modulo (as opposed to remainder), per https://stackoverflow.com/a/19288271
inline int mod(const int dividend, const int divisor) {
  int r = dividend % divisor;
  if (r < 0) r += divisor;
  return r;
}

// Round to integer
// Default rounding mode: round to nearest
inline int round_int(float x) {
#ifdef __arm__
  int res;
  asm("vcvtr.s32.f32   %[res], %[x]" : [res] "=X"(res) : [x] "w"(x));
  return res;
#else
  return (int)nearbyint(x);
#endif
}

// Wrap value to range.
// With default rounding mode (round to nearest),
// the result will be in range -y/2 to y/2
inline float wrap_pm(float x, float y) {
#ifdef FPU_FPV4
  float intval = (float)round_int(x / y);
#else
  float intval = nearbyintf(x / y);
#endif
  return x - intval * y;
}

// Same as fmodf but result is positive and y must be positive
float fmodf_pos(float x, float y);

#endif