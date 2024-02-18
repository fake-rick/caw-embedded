#include "discover.h"

#include <string.h>

#include "../utils/endian.h"

uint8_t DISCOVER_MAGIC[4] = {0xff, 0xff, 0xff, 0x00};
uint8_t DEVICE_MAGIC[4] = {0xff, 0xff, 0xff, 0x01};

void discover(const device_t* device) {
  discover_t st;
  memcpy(&(st.magic), DEVICE_MAGIC, sizeof(DEVICE_MAGIC));
  st.device_id = endian_u32(device->device_id);
  st.type_id = endian_u32(device->type_id);
  device_write(device, &st, sizeof(st));
}

/**
 * @description: 检查是否是Discover标志
 * @param {device_t*} device
 * @return {*} 1: 存在标志 0：不存在标志
 */
int discover_check_magic(const device_t* device) {
  uint8_t buf[4];
  device_read(device, buf, sizeof(buf));
  return !memcmp(buf, DISCOVER_MAGIC, sizeof(DISCOVER_MAGIC));
}

int discover_check_magic_buf(const uint8_t* buf) {
  return !memcmp(buf, DISCOVER_MAGIC, sizeof(DISCOVER_MAGIC));
}