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
  pack.body.current[0] = endian_f32(iab.ia);
  pack.body.current[1] = endian_f32(iab.ib);
  pack.body.dq[0] = endian_f32(motor->idq.id);
  pack.body.dq[1] = endian_f32(motor->idq.iq);
  pack.body.velocity = endian_f32(motor->shaft_velocity);
  if (motor->encoder_pll) {
    pack.body.angle = endian_f32(pll_get_cpr_angle(motor->encoder_pll));
  } else {
    pack.body.angle = endian_f32(as5047p_get_mechanical_angle(motor->sensor));
  }
  pack.body.tick = endian_u32(HAL_GetTick());
  device_write_buffer(device, (uint8_t*)&pack, sizeof(pack));
}