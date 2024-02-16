#ifndef ___PROTOCOL_BMS_H__
#define ___PROTOCOL_BMS_H__

#include <stdint.h>

#include "../device.h"

#pragma pack(1)
typedef struct _protocol_bms_t {
  uint8_t state;
  int32_t cell_voltage[5];
  uint8_t balance[5];
  int32_t voltage;
  int32_t current;
  int32_t temperature;
  int32_t soc;
  int32_t soh;
  uint8_t dsg;
  uint8_t chg;
} protocol_bms_t;
#pragma pack()

void protocol_write_bms_info(device_t* device, protocol_bms_t* t);

#endif