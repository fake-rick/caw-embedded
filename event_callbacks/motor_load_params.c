#include "./motor_load_params.h"
#include "../log.h"

int count = 0;

int motor_load_params_cb(device_t* device) {
  debug("motor_load_params_cb %d", count++);
  return 0;
}