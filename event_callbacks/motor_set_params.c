#include "motor_set_params.h"

#include "../app/motor/motor.h"
#include "../protocols/protocol_motor.h"
#include "log.h"

extern motor_params_t global_motor_params;
extern motor_t motor;

int motor_set_params_cb(device_t* device, uint8_t* buf) {
  protocol_unpack_motor_params(&global_motor_params,
                               (protocol_motor_params_t*)buf);
  uint32_t state = global_motor_params.info.state;
  global_motor_params.info.state = 0;
  motor_params_reset();
  motor_set_control_type(&motor, global_motor_params.mode);
  motor_reset(&motor);
  global_motor_params.info.state = state;

  // protocol_write_motor_params(device, &global_motor_params);
  return 0;
}