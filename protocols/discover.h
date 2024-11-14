#ifndef __DISCOVER_H__
#define __DISCOVER_H__

#include <stdint.h>

#include "../device.h"
#include "./protocol.h"

#pragma pack(1)
typedef struct _discover_t {
  uint32_t device_id;
  uint32_t type_id;
} discover_t;
#pragma pack()

#pragma pack(1)
typedef struct _discover_pack_t {
  protocol_header_t header;
  discover_t body;
} discover_pack_t;
#pragma pack()

void discover(device_t* device);
// int discover_check_magic(const device_t* device);
// int discover_check_magic_buf(const uint8_t* buf);

#endif