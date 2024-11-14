#include "./lowpass_filter.h"

void lowpass_filter_init(lowpass_filter_t* self, float time_constant) {
  self->tf = time_constant;
  self->y_prev = 0.0f;
  self->timestamp_prev = HAL_GetTick();
}

float lowpass_filter_calc(lowpass_filter_t* self, float x) {
  uint64_t timestamp = HAL_GetTick();
  float dt = (timestamp - self->timestamp_prev) * 1e-3;

  if (dt < 0.0f)
    dt = 1e-3f;
  else if (dt > 0.3f) {
    // 如果大于300ms则不处理
    self->y_prev = x;
    self->timestamp_prev = timestamp;
    return x;
  }
  float alpha = self->tf / (self->tf + dt);
  float y = alpha * self->y_prev + (1.0f - alpha) * x;
  self->y_prev = y;
  self->timestamp_prev = timestamp;
  return y;
}