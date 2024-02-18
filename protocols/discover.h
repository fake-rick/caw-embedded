#ifndef __DISCOVER_H__
#define __DISCOVER_H__

#include <stdint.h>

#include "../device.h"

extern uint8_t DISCOVER_MAGIC[4];
extern uint8_t DEVICE_MAGIC[4];

#pragma pack(1)
typedef struct _discover_t {
  uint8_t magic[4];
  uint32_t device_id;
  uint32_t type_id;
} discover_t;
#pragma pack()

void discover(const device_t* device);
int discover_check_magic(const device_t* device);
int discover_check_magic_buf(const uint8_t* buf);

#endif