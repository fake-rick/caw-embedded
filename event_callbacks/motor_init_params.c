#include "motor_init_params.h"
#include "motor_params.h"
#include <string.h>

extern motor_params_t params;

int motor_init_params_cb(device_t* device, uint8_t* buf) {
  memset(&params, 0, sizeof(params));
  motor_params_save(&params);
}