#ifndef __CURRENT_H__
#define __CURRENT_H__

#include "adc.h"
#include "tim.h"

typedef struct _current_iab {
  float ia;
  float ib;
} current_iab;

typedef struct _current_idq {
  float id;
  float iq;
} current_idq;

typedef struct _current_t {
  TIM_HandleTypeDef* htim;
  uint32_t pwm_channel;
  ADC_HandleTypeDef* hadc1;
  ADC_HandleTypeDef* hadc2;

  float shunt_resistor;
  float gain;
  float volts_to_amps_ratio;
  float gain_a;
  float gain_b;
  float offset_ia;
  float offset_ib;
} current_t;

int current_init(current_t* self, TIM_HandleTypeDef* htim, uint32_t pwm_channel,
                 ADC_HandleTypeDef* hadc1, ADC_HandleTypeDef* hadc2,
                 float shunt_resistor, float gain);

void current_calibrate_offsets(current_t* self);

current_iab current_get_phase_currents(current_t* self);
current_idq current_get_dq_currents(current_t* self, float electrical_angle);
current_iab current_get_last_iab();
current_idq current_get_last_idq();
#endif