#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <stdint.h>

#include "../device.h"

typedef struct __packed _protocol_header_t {
  uint8_t magic[4];
  uint32_t main_code;
  uint32_t sub_code;
  uint16_t version;
  uint32_t length;
  uint8_t checksum;
} protocol_header_t;

int protocol_header_recv(device_t* device, protocol_header_t* header);
int protocol_header_parse(protocol_header_t* header);
int protocol_header_init(protocol_header_t* header, uint32_t main_code,
                         uint32_t sub_code, uint8_t* buf, uint32_t data_size);
#endif