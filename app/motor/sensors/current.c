#include "./current.h"
#include "../../log.h"
#include "../fast_math/fast_math.h"
#include "../log.h"
#include "../macros/helper.h"

#define ADC_VOLTAGE 3.3
#define ADC_RESOLUTION 4096.0

static volatile uint32_t adc1_val = 0;
static volatile uint32_t adc2_val = 0;
static volatile float ia = 0.0;
static volatile float ib = 0.0;
static volatile float last_ia = 0.0;
static volatile float last_ib = 0.0;
static volatile float last_id = 0.0;
static volatile float last_iq = 0.0;

#define _calc_ab_currents(a, b) ((a))

void _calc_current_value() {
  ia = (float)adc1_val * ADC_VOLTAGE / ADC_RESOLUTION;
  ib = (float)adc2_val * ADC_VOLTAGE / ADC_RESOLUTION;
}

int current_init(current_t* self, TIM_HandleTypeDef* htim, uint32_t pwm_channel,
                 ADC_HandleTypeDef* hadc1, ADC_HandleTypeDef* hadc2,
                 float shunt_resistor, float gain) {
  float volts_to_amps_ratio = 1.0 / shunt_resistor / gain;

  self->htim = htim;
  self->pwm_channel = pwm_channel;
  self->hadc1 = hadc1;
  self->hadc2 = hadc2;
  self->shunt_resistor = shunt_resistor;
  self->gain = gain;
  self->volts_to_amps_ratio = volts_to_amps_ratio;
  self->gain_a = volts_to_amps_ratio;
  self->gain_b = volts_to_amps_ratio;
  self->offset_ia = 0.0;
  self->offset_ib = 0.0;

  HAL_ADCEx_InjectedStart_IT(self->hadc1);
  HAL_ADCEx_InjectedStart_IT(self->hadc2);

  HAL_TIM_PWM_Start(self->htim, self->pwm_channel);
  return 0;
}

void current_calibrate_offsets(current_t* self) {
  uint32_t calibration_rounds = 2000;
  for (int i = 0; i < calibration_rounds; i++) {
    _calc_current_value();
    self->offset_ia += ia;
    self->offset_ib += ib;
    HAL_Delay(1);
  }
  self->offset_ia = self->offset_ia / (float)calibration_rounds;
  self->offset_ib = self->offset_ib / (float)calibration_rounds;
}

current_iab current_get_phase_currents(current_t* self) {
  current_iab ret;
  _calc_current_value();
  ret.ia = (ia - self->offset_ia) * self->gain_a;
  ret.ib = (ib - self->offset_ib) * self->gain_b;
  last_ia = ret.ia;
  last_ib = ret.ib;
  return ret;
}

// 这个函数只返回最后一次记录的IA和IB值,不作任何其他计算
// 主要提供给Observation使用
current_iab current_get_last_iab() {
  current_iab ret;
  ret.ia = last_ia;
  ret.ib = last_ib;
  return ret;
}

current_idq current_get_last_idq() {
  current_idq ret;
  ret.id = last_id;
  ret.iq = last_iq;
  return ret;
}

current_idq current_get_dq_currents(current_t* self, float electrical_angle) {
  current_iab val = current_get_phase_currents(self);
  float i_alpha = val.ia;
  float i_beta = _1_SQRT3 * val.ia + _2_SQRT3 * val.ib;
  // calc dq currents
  float st, ct;
  fast_sincos(electrical_angle, &st, &ct);
  current_idq ret;
  ret.id = i_alpha * ct + i_beta * st;
  ret.iq = i_beta * ct - i_alpha * st;
  last_id = ret.id;
  last_iq = ret.iq;
  return ret;
}

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc) {
  uint32_t val = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
  if (hadc->Instance == ADC1) {
    adc1_val = val;
  } else if (hadc->Instance == ADC2) {
    adc2_val = val;
  }
}
