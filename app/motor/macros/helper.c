#include "./helper.h"

float fmodf_pos(float x, float y) {
  float res = wrap_pm(x, y);
  if (res < 0) res += y;
  return res;
}