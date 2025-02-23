#ifndef __CODE_H__
#define __CODE_H__

#include <stdint.h>

typedef enum _main_code_e {
  main_code_other = (uint32_t)0,
  main_code_system = (uint32_t)1,
  main_code_bms = (uint32_t)2,
  main_code_motor = (uint32_t)3,
} main_code_e;

typedef enum _sub_code_other_e {
  unknown = (uint32_t)0,
} sub_code_other_e;

typedef enum _sub_code_system_e {
  sub_code_system_discover = (uint32_t)0,
  sub_code_system_log = (uint32_t)1,
  sub_code_system_discover_reply = (uint32_t)1000,
} sub_code_system_e;

typedef enum _sub_code_bms_e {
  sub_code_bms_info = (uint32_t)0,
} sub_code_bms_e;

typedef enum _sub_code_motor_e {
  sub_code_motor_observation = (uint32_t)0,
  sub_code_motor_load_params = (uint32_t)1,
  sub_code_motor_init_params = (uint32_t)2,
  sub_code_motor_flash_params = (uint32_t)3,
  sub_code_motor_calibrate = (uint32_t)4,
  sub_code_motor_set_params = (uint32_t)5,
  sub_code_motor_load_params_reply = (uint32_t)1001,
} sub_code_motor_e;

#endif