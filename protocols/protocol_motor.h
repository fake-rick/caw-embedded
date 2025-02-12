#ifndef __PROTOCOLS_MOTOR_H__
#define __PROTOCOLS_MOTOR_H__
#include "../app/motor/motor_params.h"
#include "../device.h"
#include "./protocol.h"
#include <stdint.h>

#pragma pack(1)
typedef struct _protocol_motor_params_t {
  protocol_header_t header;
  motor_params_t body;
} protocol_motor_params_t;
#pragma pack()

void protocol_write_motor_params(device_t* device, motor_params_t* params);

#endif