#include "pingpong.h"

#include "../protocols/code.h"
#include "../protocols/protocol.h"

int pingpong_cb(device_t* device) {
  protocol_header_write(device, main_code_system, sub_code_system_pong, 0, 0);
}