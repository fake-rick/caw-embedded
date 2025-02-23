#include "./discover.h"
#include "../log.h"
#include "../utils/endian.h"
#include "./code.h"
#include "./protocol.h"
#include <string.h>

void discover(device_t* device) {
  discover_pack_t pack;
  pack.body.device_id = endian_u32(device->device_id);
  pack.body.type_id = endian_u32(device->type_id);
  protocol_header_init(device, &pack, main_code_system,
                       sub_code_system_discover, 0, sizeof(pack.body));
  device_write_buffer(device, &pack, sizeof(pack));
  // debug("discover > device_id: %d type_id: %d", device->device_id,
  //       device->type_id);
}

// /**
//  * @description: 检查是否是Discover标志
//  * @param {device_t*} device
//  * @return {*} 1: 存在标志 0：不存在标志
//  */
// int discover_check_magic(const device_t* device) {
//   uint8_t buf[4];
//   device_read(device, buf, sizeof(buf));
//   return !memcmp(buf, DISCOVER_MAGIC, sizeof(DISCOVER_MAGIC));
// }

// int discover_check_magic_buf(const uint8_t* buf) {
//   return !memcmp(buf, DISCOVER_MAGIC, sizeof(DISCOVER_MAGIC));
// }