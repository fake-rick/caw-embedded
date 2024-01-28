#ifndef __DISCOVER_H__
#define __DISCOVER_H__

#include <stdint.h>

#include "../device.h"

#pragma pack(1)
typedef struct _discover_t {
  uint8_t magic[4];
  uint32_t device_id;
  uint32_t type_id;
} discover_t;
#pragma pack()

void discover(const device_t* device);
int discover_check_magic(const device_t* device);

#endif