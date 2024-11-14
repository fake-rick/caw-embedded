#include "pwmx3.h"

#include "../macros/helper.h"

int pwmx3_driver_init(pwmx3_driver* drv, TIM_HandleTypeDef* tim,
                      float voltage_power_supply, float voltage_limit,
                      float max_duty) {
  drv->pwm_tim = tim;
  drv->voltage_power_supply = voltage_power_supply;
  drv->voltage_limit = voltage_limit;
  drv->max_duty = max_duty;

  __HAL_TIM_SET_COMPARE(drv->pwm_tim, TIM_CHANNEL_1, 0);
  __HAL_TIM_SET_COMPARE(drv->pwm_tim, TIM_CHANNEL_2, 0);
  __HAL_TIM_SET_COMPARE(drv->pwm_tim, TIM_CHANNEL_3, 0);

  HAL_TIM_PWM_Start(drv->pwm_tim, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(drv->pwm_tim, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(drv->pwm_tim, TIM_CHANNEL_3);
}

void pwmx3_driver_set_pwm(pwmx3_driver* drv, float ua, float ub, float uc) {
  ua = _constrain(ua, 0.0f, drv->voltage_limit);
  ub = _constrain(ub, 0.0f, drv->voltage_limit);
  uc = _constrain(uc, 0.0f, drv->voltage_limit);

  float dc_a = _constrain(ua / drv->voltage_power_supply, 0.0f, 1.0f);
  float dc_b = _constrain(ub / drv->voltage_power_supply, 0.0f, 1.0f);
  float dc_c = _constrain(uc / drv->voltage_power_supply, 0.0f, 1.0f);

  __HAL_TIM_SET_COMPARE(drv->pwm_tim, TIM_CHANNEL_1, dc_a * drv->max_duty);
  __HAL_TIM_SET_COMPARE(drv->pwm_tim, TIM_CHANNEL_2, dc_b * drv->max_duty);
  __HAL_TIM_SET_COMPARE(drv->pwm_tim, TIM_CHANNEL_3, dc_c * drv->max_duty);
}