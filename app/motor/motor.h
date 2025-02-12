#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "../../drivers/as5047p/as5047p.h"
#include "../../drivers/drv8323/drv8323.h"
#include "../sensors/current.h"
#include "./control/lowpass_filter.h"
#include "./control/pid.h"
#include "./control/pll.h"
#include "pwmx3.h"
#include "tim.h"

typedef enum _motor_control_type_e {
  TORQUE,
  VELOCITY,
  ANGLE,
  VELOCITY_OPEN_LOOP
} motor_control_type_e;

typedef struct _dq_voltage_t {
  float d;
  float q;
} dq_voltage_t;

typedef struct _motor_t {
  float velocity_limit;
  pwmx3_driver* driver;
  current_t* current_sensor;
  as5047p_t* sensor;
  int32_t sensor_direction;
  motor_control_type_e control_type;

  dq_voltage_t dq_voltage;

  uint16_t pole_pairs;     // 极对数
  float shaft_angle;       // 轴角度
  float shaft_angle_sp;    // 目标角度
  float shaft_velocity;    // 轴速度
  float shaft_velocity_sp; // 目标速度
  float current_sp;        // 目标电流
  current_idq idq;
  float ua;
  float ub;
  float uc;
  uint64_t open_loop_timestamp;

  float voltage_sensor_align;
  float zero_electrical_angle;
  float electrical_angle;

  pid_t* pid_current_q;
  lowpass_filter_t* lpf_q;
  pid_t* pid_current_d;
  lowpass_filter_t* lpf_d;
  pid_t* pid_velocity;
  lowpass_filter_t* lpf_velocity;
  pid_t* pid_angle;
  lowpass_filter_t* lpf_angle;
  pll_t* encoder_pll;
} motor_t;

int motor_init(motor_t* self, pwmx3_driver* driver, current_t* current_sensor,
               as5047p_t* sensor, int32_t sensor_direction,
               motor_control_type_e control_type, uint16_t pole_pairs,
               pid_t* pid_current_q, lowpass_filter_t* lpf_q,
               pid_t* pid_current_d, lowpass_filter_t* lpf_d,
               pid_t* pid_velocity, lowpass_filter_t* lpf_velocity,
               pid_t* pid_angle, lowpass_filter_t* lpf_angle,
               pll_t* encoder_pll);
int motor_step(motor_t* self, float target);
float motor_velocity_open_loop(motor_t* self, float target);
void motor_set_phase_voltage(motor_t* self, float uq, float ud,
                             float electrical_angle);
void motor_sensor_calibrate_offsets(motor_t* self);
void motor_align_sensor(motor_t* self);
float motor_shaft_velocity(motor_t* self);
float motor_shaft_angle(motor_t* self);

#endif