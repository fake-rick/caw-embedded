#ifndef __MOTOR_PARAMS_H__
#define __MOTOR_PARAMS_H__

#include <stdint.h>

typedef struct _motor_param_item_t {
  float kp;
  float ki;
  float kd;
  float output_ramp;
  float output_limit;
  float lpf_time_constant;
} motor_param_item_t;

typedef struct _motor_params_t {
  uint32_t mode;
  float target;
  motor_param_item_t velocity;
  motor_param_item_t angle;
  motor_param_item_t current_d;
  motor_param_item_t current_q;
} motor_params_t;

int motor_params_save(const motor_params_t* params);
void motor_params_load(motor_params_t* params);

#endif