#ifndef __PROTOCOLS_MOTOR_H__
#define __PROTOCOLS_MOTOR_H__
#include "../app/motor/motor_params.h"
#include "../device.h"
#include "./protocol.h"
#include <stdint.h>

typedef struct __packed _protocol_motor_params_body_t {
  motor_params_t body;
} protocol_motor_params_body_t;

typedef struct __packed _protocol_motor_params_t {
  protocol_header_t header;
  motor_params_t body;
} protocol_motor_params_t;

void protocol_write_motor_params(device_t* device, motor_params_t* params);
void protocol_unpack_motor_params(motor_params_t* dst,
                                  const protocol_motor_params_body_t* src);

#endif