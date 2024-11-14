#include "event.h"

#include <assert.h>

#include "../log.h"
#include "./caw_config.h"
#include "./device.h"
#include "./protocols/code.h"
#include "./protocols/discover.h"
#include "./protocols/protocol.h"
#include "as5047p.h"
#include "current.h"
#include "string.h"

static event_t event_handle;
static uint8_t uart_recv_buf[UART_RECV_BUF_SIZE];

static int event_find_chain(uint32_t main_code, uint32_t sub_code) {
  assert(event_handle.chain_index <= MAX_EVENT_CHAIN);
  for (int i = 0; i < event_handle.chain_index; i++) {
    if (event_handle.chains[i].main_code == main_code &&
        event_handle.chains[i].sub_code == sub_code) {
      return i;
    }
  }
  return -1;
}

static void dispatch(protocol_header_t* header, uint8_t buf) {
  int chain_idx = event_find_chain(header->main_code, header->sub_code);
  if (chain_idx != -1) {
    event_handle.chains[chain_idx].callback(event_handle.device, buf);
  }
}

protocol_header_t protocol_header;
static uint16_t recv_state;

typedef enum _uart_it_state_e {
  uart_it_state_receive_protocol_header = 0,
  uart_it_state_receive_protocol_body,
} uart_it_state_e;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
  if (huart == UART_HANDLE) {
    /// 协议头接收解析
    if (recv_state == uart_it_state_receive_protocol_header) {
      if (-1 == protocol_header_parse(&protocol_header)) {
        error("parse protocol header failed");
      }

      if (protocol_header.data_size <= 0) {
        dispatch(&protocol_header, 0);
        recv_state = uart_it_state_receive_protocol_header;
        event_handle.device->read(UART_HANDLE, &protocol_header,
                                  sizeof(protocol_header_t));
        return;
      }

      recv_state = uart_it_state_receive_protocol_body;
      event_handle.device->read(UART_HANDLE, uart_recv_buf,
                                protocol_header.data_size);
      return;
    }
    /// 协议体接收解析
    if (recv_state == uart_it_state_receive_protocol_body) {
      dispatch(&protocol_header, uart_recv_buf);
      recv_state = uart_it_state_receive_protocol_header;
      event_handle.device->read(UART_HANDLE, &protocol_header,
                                sizeof(protocol_header_t));
      return;
    }
  }
}

void event_run() {
  recv_state = uart_it_state_receive_protocol_header;
  event_handle.device->read(UART_HANDLE, &protocol_header,
                            sizeof(protocol_header_t));
}

void event_init(device_t* device) {
  memset(&event_handle, 0, sizeof(event_t));
  memset(&(event_handle.chains), 0xff, sizeof(event_handle.chains));
  event_handle.device = device;
  event_handle.get_tick = HAL_GetTick;
}

int event_register(uint32_t main_code, uint32_t sub_code,
                   event_processer_cb cbfn) {
  assert(event_handle.chain_index <= MAX_EVENT_CHAIN);

  if (event_handle.chain_index == MAX_EVENT_CHAIN)
    return -1;

  event_handle.chains[event_handle.chain_index].main_code = main_code;
  event_handle.chains[event_handle.chain_index].sub_code = sub_code;
  event_handle.chains[event_handle.chain_index].callback = cbfn;
  ++event_handle.chain_index;
  return 0;
}

void event_step() {
  event_handle.discover_count++;
  if (event_handle.discover_count % 5 == 0) {
    event_handle.discover_count = 0;
    discover(event_handle.device);
  }
}
