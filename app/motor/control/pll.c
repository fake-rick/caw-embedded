#include "./pll.h"

#include <math.h>

#include "../macros/helper.h"
#include "main.h"

static const float meas_period = 1.0 / 40000;

void pll_init(pll_t* self, uint16_t cpr, float bandwidth) {
  self->cpr = cpr;
  self->bandwidth = bandwidth;
  self->error_rate = 0.0;
  self->shadow_count = 0;
  self->count_in_cpr = 0;
  self->pos_cpr_counts = 0.0;
  self->pos_estimate_counts = 0.0;
  self->vel_estimate_counts = 0.0;
  self->pos_cpr_estimate = 0.0;
  self->pll_ki = 2.0 * bandwidth;
  self->pll_kp = 0.25 * (self->pll_ki * self->pll_ki);
}

// val为编码器当前的采样计数值
void pll_update(pll_t* self, int32_t val) {
  int32_t delta_enc = 0;
  int32_t pos_latched = val;

  delta_enc = pos_latched - self->count_in_cpr;
  delta_enc = mod(delta_enc, self->cpr);

  if (delta_enc > self->cpr / 2) {
    delta_enc -= self->cpr;
  }

  self->shadow_count += delta_enc;
  self->count_in_cpr += delta_enc;
  self->count_in_cpr = mod(self->count_in_cpr, self->cpr);

  // pll
  // 预测当前位置
  self->pos_estimate_counts += meas_period * self->vel_estimate_counts;
  self->pos_cpr_counts += meas_period * self->vel_estimate_counts;
  // 鉴相器
  float delta_pos_counts =
      (float)(self->shadow_count - (int32_t)floor(self->pos_estimate_counts));
  float delta_pos_cpr_counts =
      (float)(self->count_in_cpr - (int32_t)floor(self->pos_cpr_counts));
  delta_pos_cpr_counts = wrap_pm(delta_pos_cpr_counts, (float)self->cpr);
  // 环路滤波
  self->pos_estimate_counts += meas_period * self->pll_kp * delta_pos_counts;
  self->pos_cpr_counts += meas_period * self->pll_kp * delta_pos_cpr_counts;
  self->pos_cpr_counts = fmodf_pos(self->pos_cpr_counts, (float)self->cpr);
  self->vel_estimate_counts +=
      meas_period * self->pll_ki * delta_pos_cpr_counts;
  if (_abs(self->vel_estimate_counts) < 0.5 * meas_period * self->pll_ki) {
    self->vel_estimate_counts = 0.0;
  }
  self->pos_estimate = self->pos_estimate_counts / (float)self->cpr;
  self->vel_estimate = self->vel_estimate_counts / (float)self->cpr;
  self->pos_cpr_estimate = self->pos_cpr_counts / (float)self->cpr;
}

float pll_get_cpr_angle(pll_t* self) { return self->pos_cpr_estimate * _2PI; }
float pll_get_angle(pll_t* self) { return self->pos_estimate * _2PI; }
float pll_get_velocity(pll_t* self) { return self->vel_estimate; }