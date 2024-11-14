#include "protocol.h"

#include <string.h>

#include "../utils/endian.h"

static uint8_t MAGIC[4] = {'C', 'A', 'W', 'X'};
static uint16_t VERSION = 0x101;

int protocol_header_recv(device_t* device, protocol_header_t* header) {
  if (device_read(device, header, sizeof(protocol_header_t)))
    return -1;
  if (memcmp(header->magic, MAGIC, sizeof(MAGIC)))
    return -1;
  header->main_code = endian_u32(header->main_code);
  header->sub_code = endian_u32(header->sub_code);
  header->version = endian_u16(header->version);
  if (VERSION != header->version)
    return -1;
  header->data_size = endian_u32(header->data_size);
  return 0;
}

int protocol_header_parse(protocol_header_t* header) {
  if (memcmp(header->magic, MAGIC, sizeof(MAGIC)))
    return -1;
  header->main_code = endian_u32(header->main_code);
  header->sub_code = endian_u32(header->sub_code);
  header->version = endian_u16(header->version);
  if (VERSION != header->version)
    return -1;
  header->data_size = endian_u32(header->data_size);
  return 0;
}

int protocol_header_init(protocol_header_t* header, uint32_t main_code,
                         uint32_t sub_code, uint8_t* buf, uint32_t data_size) {
  memcpy(header->magic, MAGIC, sizeof(MAGIC));
  header->main_code = endian_u32(main_code);
  header->sub_code = endian_u32(sub_code);
  header->version = endian_u16(VERSION);
  header->data_size = endian_u32(data_size);

  return 0;
}