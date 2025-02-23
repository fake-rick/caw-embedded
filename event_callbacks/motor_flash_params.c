#include "motor_flash_params.h"
#include "log.h"
#include "motor_params.h"
#include <string.h>

extern motor_params_t global_motor_params;

int motor_flash_params_cb(device_t* device, uint8_t* buf) {
  if (0 == motor_params_save(&global_motor_params)) {
    info("flash params success");
  } else {
    error("flash params failed");
  }
  return 0;
}