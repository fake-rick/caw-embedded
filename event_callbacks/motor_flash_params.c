#include "motor_flash_params.h"
#include "log.h"
#include "motor_params.h"
#include <string.h>

extern motor_params_t global_motor_params;

int motor_flash_params_cb(device_t* device, uint8_t* buf) {
  debug("motor_flash_params_cb");
  motor_params_save(&global_motor_params);
}