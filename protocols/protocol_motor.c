#include "./protocol_motor.h"
#include "../utils/endian.h"
#include "./code.h"

void protocol_write_motor_params(device_t* device, motor_params_t* params) {

  protocol_motor_params_t pack;
  pack.body.mode = endian_u32(params->mode);
  pack.body.target = endian_f32(params->target);
  pack.body.current_d.kp = endian_f32(params->current_d.kp);
  pack.body.current_d.ki = endian_f32(params->current_d.ki);
  pack.body.current_d.kd = endian_f32(params->current_d.kd);
  pack.body.current_d.output_ramp = endian_f32(params->current_d.output_ramp);
  pack.body.current_d.output_limit = endian_f32(params->current_d.output_limit);
  pack.body.current_d.lpf_time_constant =
      endian_f32(params->current_d.lpf_time_constant);

  pack.body.current_q.kp = endian_f32(params->current_q.kp);
  pack.body.current_q.ki = endian_f32(params->current_q.ki);
  pack.body.current_q.kd = endian_f32(params->current_q.kd);
  pack.body.current_q.output_ramp = endian_f32(params->current_q.output_ramp);
  pack.body.current_q.output_limit = endian_f32(params->current_q.output_limit);
  pack.body.current_q.lpf_time_constant =
      endian_f32(params->current_q.lpf_time_constant);

  pack.body.velocity.kp = endian_f32(params->velocity.kp);
  pack.body.velocity.ki = endian_f32(params->velocity.ki);
  pack.body.velocity.kd = endian_f32(params->velocity.kd);
  pack.body.velocity.output_ramp = endian_f32(params->velocity.output_ramp);
  pack.body.velocity.output_limit = endian_f32(params->velocity.output_limit);
  pack.body.velocity.lpf_time_constant =
      endian_f32(params->velocity.lpf_time_constant);

  pack.body.angle.kp = endian_f32(params->angle.kp);
  pack.body.angle.ki = endian_f32(params->angle.ki);
  pack.body.angle.kd = endian_f32(params->angle.kd);
  pack.body.angle.output_ramp = endian_f32(params->angle.output_ramp);
  pack.body.angle.output_limit = endian_f32(params->angle.output_limit);
  pack.body.angle.lpf_time_constant =
      endian_f32(params->angle.lpf_time_constant);

  protocol_header_init(&pack, main_code_motor, sub_code_motor_load_params_reply,
                       0, sizeof(pack.body));
  device_write_buffer(device, &pack, sizeof(pack));
}