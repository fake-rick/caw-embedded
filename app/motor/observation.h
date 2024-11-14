#ifndef __MOTOR_OBSERVATION_H__
#define __MOTOR_OBSERVATION_H__

#include "../../device.h"
#include "../../drivers/as5047p/as5047p.h"
#include "../../protocols/protocol.h"
#include "motor.h"
#include "stdint.h"

#pragma pack(1)
typedef struct _motor_observation_t {
  int32_t voltage[3];
  int32_t current[2];
  int32_t dq[2];
  int32_t velocity;
  int32_t angle;
  uint32_t timestamp;
} motor_observation_t;
#pragma pack(0)

#pragma pack(1)
typedef struct _motor_observation_pack_t {
  protocol_header_t header;
  motor_observation_t body;
} motor_observation_pack_t;
#pragma pack()

void motor_observation_write_pack(motor_t* motor, device_t* device);

#endif