#include "motor_params.h"

#include <string.h>

#include "stm32g4xx_hal.h"

#define FLASH_PARAM_BASE_ADDR (0x0803F800)

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