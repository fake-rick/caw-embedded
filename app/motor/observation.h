#ifndef __MOTOR_OBSERVATION_H__
#define __MOTOR_OBSERVATION_H__

#include "../../device.h"
#include "../../drivers/as5047p/as5047p.h"
#include "../../protocols/protocol.h"
#include "motor.h"
#include "stdint.h"

#pragma pack(1)
typedef struct _motor_observation_t {
  // float voltage[3];
  float current[2];
  float dq[2];
  float velocity;
  float angle;
  uint32_t tick;
} motor_observation_t;
#pragma pack()

#pragma pack(1)
typedef struct _motor_observation_pack_t {
  protocol_header_t header;
  motor_observation_t body;
} motor_observation_pack_t;
#pragma pack()

void motor_observation_write_pack(motor_t* motor, device_t* device);

#endif