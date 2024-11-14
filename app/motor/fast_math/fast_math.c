#include "./fast_math.h"
#include "../macros/helper.h"
#include "./table.h"

const float MULTIPLIER = 81.4873308631f;

float fast_sin(float theta) {
  while (theta < 0.0f)
    theta += _2PI;
  while (theta >= _2PI)
    theta -= _2PI;
  return SinTable[(int)(MULTIPLIER * theta)];
}

float fast_cos(float theta) { return fast_sin(_PI_2 - theta); }

float fast_sincos(float theta, float* s, float* c) {
  *s = fast_sin(theta);
  *c = fast_cos(theta);
}