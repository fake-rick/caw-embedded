#include "../../protocols/code.h"
#include "../../utils/endian.h"
#include "./sensors/current.h"
#include "observation.h"
#include "string.h"

void motor_observation_write_pack(motor_t* motor, device_t* device) {
  motor_observation_pack_t pack;

  protocol_header_init(&pack, main_code_motor, sub_code_motor_observation, 0,
                       sizeof(pack.body));

  current_iab iab = current_get_last_iab();

  pack.body.voltage[0] = endian_u32((int)(motor->ua * 1000));
  pack.body.voltage[1] = endian_u32((int)(motor->ub * 1000));
  pack.body.voltage[2] = endian_u32((int)(motor->uc * 1000));
  pack.body.current[0] = endian_u32((int)(iab.ia * 1000));
  pack.body.current[1] = endian_u32((int)(iab.ib * 1000));
  pack.body.dq[0] = endian_u32((int)(motor->idq.id * 1000));
  pack.body.dq[1] = endian_u32((int)(motor->idq.iq * 1000));
  pack.body.velocity = endian_u32((int)(motor->shaft_velocity * 1000));
  pack.body.angle =
      endian_u32((int)(as5047p_get_mechanical_angle(motor->sensor) * 1000));
  pack.body.timestamp = endian_u32(HAL_GetTick());
  device_write_buffer(device, (uint8_t*)&pack, sizeof(pack));
}