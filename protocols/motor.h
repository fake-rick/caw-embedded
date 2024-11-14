#ifndef __PROTOCOLS_MOTOR_H__
#define __PROTOCOLS_MOTOR_H__
#include <stdint.h>

#include "./protocol.h"

#pragma pack(1)
typedef struct _motor_params_t {
  uint32_t control_mode;
  uint32_t target;
  uint32_t current_p;
  uint32_t current_i;
  uint32_t current_d;
  uint32_t current_f;
  uint32_t velocity_p;
  uint32_t velocity_i;
  uint32_t velocity_d;
  uint32_t velocity_f;
  uint32_t angle_p;
  uint32_t angle_i;
  uint32_t angle_d;
  uint32_t angle_f;
} motor_params_t;
#pragma pack()

#pragma pack(1)
typedef struct _motor_params_pack_t {
  protocol_header_t header;
  motor_params_t body;
} motor_params_pack_t;
#pragma pack()

#endif