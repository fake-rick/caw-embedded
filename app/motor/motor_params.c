#include "motor_params.h"

#include "../control/lowpass_filter.h"
#include "../control/pid.h"
#include "../control/pll.h"
#include "stm32g4xx_hal.h"
#include <string.h>

#define FLASH_PARAM_BASE_ADDR (0x0803F800)

pid_t global_motor_pid_current_q;
lowpass_filter_t global_motor_lpf_q;
pid_t global_motor_pid_current_d;
lowpass_filter_t global_motor_lpf_d;
pid_t global_motor_pid_velocity;
lowpass_filter_t global_motor_lpf_velocity;
pid_t global_motor_pid_angle;
lowpass_filter_t global_motor_lpf_angle;
pll_t global_motor_encoder_pll;
motor_params_t global_motor_params;

void motor_params_reset() {
  pid_init(&global_motor_pid_current_q, global_motor_params.current_q.kp,
           global_motor_params.current_q.ki, global_motor_params.current_q.kd,
           global_motor_params.current_q.output_ramp,
           global_motor_params.current_q.output_limit);
  lowpass_filter_init(&global_motor_lpf_q,
                      global_motor_params.current_q.lpf_time_constant);
  pid_init(&global_motor_pid_current_d, global_motor_params.current_d.kp,
           global_motor_params.current_d.ki, global_motor_params.current_d.kd,
           global_motor_params.current_d.output_ramp,
           global_motor_params.current_d.output_limit);
  lowpass_filter_init(&global_motor_lpf_d,
                      global_motor_params.current_d.lpf_time_constant);
  pid_init(&global_motor_pid_velocity, global_motor_params.velocity.kp,
           global_motor_params.velocity.ki, global_motor_params.velocity.kd,
           global_motor_params.velocity.output_ramp,
           global_motor_params.velocity.output_limit);
  lowpass_filter_init(&global_motor_lpf_velocity,
                      global_motor_params.velocity.lpf_time_constant);
  pid_init(&global_motor_pid_angle, global_motor_params.angle.kp,
           global_motor_params.angle.ki, global_motor_params.angle.kd,
           global_motor_params.angle.output_ramp,
           global_motor_params.angle.output_limit);
  lowpass_filter_init(&global_motor_lpf_angle,
                      global_motor_params.angle.lpf_time_constant);
}

int motor_params_save(const motor_params_t* params) {
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
  FLASH_EraseInitTypeDef flash;
  uint32_t err;
  flash.Banks = FLASH_BANK_1;
  flash.TypeErase = FLASH_TYPEERASE_PAGES;
  flash.Page = 127;
  flash.NbPages = 1;
  HAL_FLASHEx_Erase(&flash, &err);

  uint64_t buf[1 + sizeof(motor_params_t) / sizeof(uint64_t)];
  memcpy(buf, (void*)params, sizeof(motor_params_t));

  for (unsigned int i = 0; i < 1 + sizeof(motor_params_t) / sizeof(uint64_t);
       i++) {
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                      FLASH_PARAM_BASE_ADDR + i * 8, buf[i]);
  }

  HAL_FLASH_Lock();
  return 0;
}

void motor_params_load(motor_params_t* params) {
  memcpy(params, FLASH_PARAM_BASE_ADDR, sizeof(motor_params_t));
}