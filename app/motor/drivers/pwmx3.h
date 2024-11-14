#ifndef __PWMX3_H__
#define __PWMX3_H__

#include "tim.h"
typedef struct _pwmx3_driver {
  TIM_HandleTypeDef* pwm_tim;
  float voltage_power_supply;
  float voltage_limit;
  float max_duty;
} pwmx3_driver;

int pwmx3_driver_init(pwmx3_driver* drv, TIM_HandleTypeDef* tim,
                      float voltage_power_supply, float voltage_limit,
                      float max_duty);
void pwmx3_driver_set_pwm(pwmx3_driver* drv, float ua, float ub, float uc);

#endif