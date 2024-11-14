#include "pid.h"
#include "../macros/helper.h"

void pid_init(pid_t* self, float p, float i, float d, float ramp, float limit) {
  self->p = p;
  self->i = i;
  self->d = d;
  self->output_ramp = ramp;
  self->limit = limit;
  self->error_prev = 0.0;
  self->output_prev = 0.0;
  self->integral_prev = 0.0;
  self->timestamp_prev = HAL_GetTick();
}

float pid_calc(pid_t* self, float error) {
  uint64_t timestamp_now = HAL_GetTick();
  float ts = (float)(timestamp_now - self->timestamp_prev) * 1e-3;
  if (ts <= 0 || ts > 0.5f)
    ts = 1e-3;

  float proportional = self->p * error;
  float integral =
      self->integral_prev + self->i * ts * 0.5f * (error + self->error_prev);
  integral = _constrain(integral, -self->limit, self->limit);
  float derivative = self->d * (error - self->error_prev) / ts;

  float output = proportional + integral + derivative;

  output = _constrain(output, -self->limit, self->limit);

  if (self->output_ramp > 0) {
    float output_rate = (output - self->output_prev) / ts;
    if (output_rate > self->output_ramp)
      output = self->output_prev + self->output_ramp * ts;
    else if (output_rate < -self->output_ramp)
      output = self->output_prev - self->output_ramp * ts;
  }

  self->integral_prev = integral;
  self->output_prev = output;
  self->error_prev = error;
  self->timestamp_prev = timestamp_now;
  return output;
}

void pid_reset(pid_t* self) {
  self->integral_prev = 0.0;
  self->output_prev = 0.0;
  self->error_prev = 0.0;
}