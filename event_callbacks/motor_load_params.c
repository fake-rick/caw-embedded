#include "./motor_load_params.h"
#include "../log.h"
#include "../protocols/protocol_motor.h"
#include "motor_params.h"

extern motor_params_t global_motor_params;

int motor_load_params_cb(device_t* device, uint8_t* buf) {
  protocol_write_motor_params(device, &global_motor_params);
  return 0;
}