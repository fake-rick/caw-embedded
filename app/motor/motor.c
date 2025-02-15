#include "motor.h"

#include "../log.h"
#include "./macros/helper.h"
#include "./sensors/current.h"
#include "gpio.h"
#include "math.h"

float _electrical_angle(motor_t* self) {
  return self->shaft_angle * (float)self->pole_pairs;
}

float _normalize_angle(float angle) {
  float a = fmod(angle, _2PI);
  return a >= 0 ? a : (a + _2PI);
}

float _sensor_electrical_angle(motor_t* self) {
  if (self->encoder_pll) {
    return _normalize_angle((float)(self->sensor_direction) *
                                (float)(self->pole_pairs) *
                                pll_get_cpr_angle(self->encoder_pll) -
                            self->zero_electrical_angle);
  }
  return _normalize_angle((float)(self->sensor_direction) *
                              (float)(self->pole_pairs) *
                              as5047p_get_mechanical_angle(self->sensor) -
                          self->zero_electrical_angle);
}

int motor_init(motor_t* self, pwmx3_driver* driver, current_t* current_sensor,
               as5047p_t* sensor, int32_t sensor_direction,
               motor_control_type_e control_type, uint16_t pole_pairs,
               pid_t* pid_current_q, lowpass_filter_t* lpf_q,
               pid_t* pid_current_d, lowpass_filter_t* lpf_d,
               pid_t* pid_velocity, lowpass_filter_t* lpf_velocity,
               pid_t* pid_angle, lowpass_filter_t* lpf_angle,
               pll_t* encoder_pll) {
  self->velocity_limit = 3000.0;
  self->driver = driver;
  self->current_sensor = current_sensor;
  self->sensor = sensor;
  self->sensor_direction = sensor_direction;
  self->control_type = control_type;
  self->dq_voltage.d = 0.0;
  self->dq_voltage.q = 0.0;
  self->pole_pairs = pole_pairs;
  self->shaft_angle = 0.0;
  self->shaft_angle_sp = 0.0;
  self->shaft_velocity = 0.0;
  self->shaft_velocity_sp = 0.0;
  self->current_sp = 0.0;
  self->open_loop_timestamp = 0;
  self->voltage_sensor_align = 3.0;
  self->zero_electrical_angle = 0.0;
  self->electrical_angle = 0.0;

  self->idq.id = 0.0;
  self->idq.iq = 0.0;

  self->pid_current_q = pid_current_q;
  self->lpf_q = lpf_q;
  self->pid_current_d = pid_current_d;
  self->lpf_d = lpf_d;
  self->pid_velocity = pid_velocity;
  self->lpf_velocity = lpf_velocity;
  self->pid_angle = pid_angle;
  self->lpf_angle = lpf_angle;

  self->encoder_pll = encoder_pll;
  return 0;
}

float motor_shaft_velocity(motor_t* self) {
  if (self->encoder_pll) {
    return (float)(self->sensor_direction) *
           pll_get_velocity(self->encoder_pll);
  }
  return (float)(self->sensor_direction) *
         lowpass_filter_calc(self->lpf_velocity,
                             as5047p_get_velocity(self->sensor));
}

float motor_shaft_angle(motor_t* self) {
  if (self->encoder_pll) {
    return (float)(self->sensor_direction) * pll_get_angle(self->encoder_pll);
  }
  return (float)(self->sensor_direction) *
         lowpass_filter_calc(self->lpf_angle, as5047p_get_angle(self->sensor));
}

void motor_sensor_calibrate_offsets(motor_t* self) {
  pwmx3_driver_set_pwm(self->driver, self->driver->voltage_limit / 2.0,
                       self->driver->voltage_limit / 2.0,
                       self->driver->voltage_limit / 2.0);
  current_calibrate_offsets(self->current_sensor);
  pwmx3_driver_set_pwm(self->driver, 0.0, 0.0, 0.0);
}

int _exec(motor_t* self) {
  if (self->encoder_pll) {
    pll_update(self->encoder_pll, as5047p_get_raw_count(self->sensor));
  } else {
    if (0 != as5047p_update(self->sensor)) {
      return -1;
    }
  }

  self->electrical_angle = _sensor_electrical_angle(self);
  current_idq idq =
      current_get_dq_currents(self->current_sensor, self->electrical_angle);
  self->idq.iq = lowpass_filter_calc(self->lpf_q, idq.iq);
  self->idq.id = lowpass_filter_calc(self->lpf_d, idq.id);
  self->dq_voltage.q = pid_calc(self->pid_current_q, self->current_sp - idq.iq);
  self->dq_voltage.d = pid_calc(self->pid_current_d, -idq.id);
  motor_set_phase_voltage(self, self->dq_voltage.q, self->dq_voltage.d,
                          self->electrical_angle);
  return 0;
}

int motor_step(motor_t* self, float target) {
  self->shaft_velocity = motor_shaft_velocity(self);
  self->shaft_angle = motor_shaft_angle(self);

  // if (0 != as5047p_update(self->sensor)) {
  //   return -1;
  // }
  // self->electrical_angle = _sensor_electrical_angle(self);

  if (self->control_type == TORQUE) {
    self->current_sp = target;
    _exec(self);
  } else if (self->control_type == VELOCITY) {
    self->shaft_velocity_sp = target;
    self->current_sp = pid_calc(self->pid_velocity,
                                self->shaft_velocity_sp - self->shaft_velocity);
    _exec(self);
  } else if (self->control_type == ANGLE) {
    self->shaft_angle_sp = target;
    self->shaft_velocity_sp = _constrain(
        pid_calc(self->pid_angle, self->shaft_angle_sp - self->shaft_angle),
        -self->velocity_limit, self->velocity_limit);
    self->current_sp = pid_calc(self->pid_velocity,
                                self->shaft_velocity_sp - self->shaft_velocity);
    _exec(self);
  } else if (self->control_type == VELOCITY_OPEN_LOOP) {
    self->shaft_velocity_sp = target;
    self->dq_voltage.q =
        motor_velocity_open_loop(self, self->shaft_velocity_sp);
    self->dq_voltage.d = 0.0;
  }
  return 0;
}

float motor_velocity_open_loop(motor_t* self, float target) {
  uint64_t now_ms = (uint64_t)HAL_GetTick();
  float ts = (now_ms - self->open_loop_timestamp) * 1e-3;
  if (ts <= 0 || ts > 0.5) {
    ts = 1e-3;
  }
  self->shaft_angle = _normalize_angle(self->shaft_angle + target * ts);
  self->shaft_velocity = target;
  float uq = self->driver->voltage_limit;
  motor_set_phase_voltage(self, uq, 0.0, _electrical_angle(self));
  self->open_loop_timestamp = now_ms;
  return uq;
}

void motor_align_sensor(motor_t* self) {
  motor_set_phase_voltage(self, self->voltage_sensor_align, 0.0, _3PI_2);
  HAL_Delay(700);
  as5047p_update(self->sensor);
  self->zero_electrical_angle = 0.0;
  self->zero_electrical_angle = _sensor_electrical_angle(self);
  HAL_Delay(20);
  motor_set_phase_voltage(self, 0.0, 0.0, 0.0);
  HAL_Delay(100);
}

void motor_set_phase_voltage(motor_t* self, float uq, float ud,
                             float electrical_angle) {
  int sector;
  float uout;

  if (ud) {
    uout = sqrt(ud * ud + uq * uq) / self->driver->voltage_limit;
    electrical_angle = _normalize_angle(electrical_angle + atan2(uq, ud));
  } else {
    uout = uq / self->driver->voltage_limit;
    electrical_angle = _normalize_angle(electrical_angle + _PI_2);
  }
  sector = floor(electrical_angle / _PI_3) + 1;
  float t1 = _SQRT3 * sin(sector * _PI_3 - electrical_angle) * uout;
  float t2 = _SQRT3 * sin(electrical_angle - (sector - 1.0f) * _PI_3) * uout;
  float t0 = 1 - t1 - t2;

  // 计算占空比
  float ta, tb, tc;
  switch (sector) {
  case 1:
    ta = t1 + t2 + t0 / 2.0;
    tb = t2 + t0 / 2.0;
    tc = t0 / 2.0;
    break;
  case 2:
    ta = t1 + t0 / 2.0;
    tb = t1 + t2 + t0 / 2.0;
    tc = t0 / 2.0;
    break;
  case 3:
    ta = t0 / 2.0;
    tb = t1 + t2 + t0 / 2.0;
    tc = t2 + t0 / 2.0;
    break;
  case 4:
    ta = t0 / 2.0;
    tb = t1 + t0 / 2.0;
    tc = t1 + t2 + t0 / 2.0;
    break;
  case 5:
    ta = t2 + t0 / 2.0;
    tb = t0 / 2.0;
    tc = t1 + t2 + t0 / 2.0;
    break;
  case 6:
    ta = t1 + t2 + t0 / 2.0;
    tb = t0 / 2.0;
    tc = t1 + t0 / 2.0;
    break;
  default:
    // 错误状态
    ta = 0;
    tb = 0;
    tc = 0;
  }

  self->ua = ta * self->driver->voltage_limit;
  self->ub = tb * self->driver->voltage_limit;
  self->uc = tc * self->driver->voltage_limit;

  pwmx3_driver_set_pwm(self->driver, self->ua, self->ub, self->uc);
}

void motor_set_control_type(motor_t* self, motor_control_type_e type) {
  self->control_type = type;
}

void motor_reset(motor_t* self) {
  self->velocity_limit = 3000.0;
  self->dq_voltage.d = 0.0;
  self->dq_voltage.q = 0.0;
  self->shaft_angle = 0.0;
  self->shaft_angle_sp = 0.0;
  self->shaft_velocity = 0.0;
  self->shaft_velocity_sp = 0.0;
  self->current_sp = 0.0;
  self->open_loop_timestamp = 0;
  self->voltage_sensor_align = 3.0;
  self->zero_electrical_angle = 0.0;
  self->electrical_angle = 0.0;

  self->idq.id = 0.0;
  self->idq.iq = 0.0;
}