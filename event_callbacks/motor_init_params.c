#include "motor_init_params.h"
#include "motor_params.h"
#include <string.h>

extern motor_params_t global_motor_params;

int motor_init_params_cb(device_t* device, uint8_t* buf) {
  memset(&global_motor_params, 0, sizeof(global_motor_params));
  motor_params_save(&global_motor_params);
}