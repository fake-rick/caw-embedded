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

event_t event_handle;

int event_find_chain(uint32_t main_code, uint32_t sub_code) {
  assert(event_handle.chain_index <= MAX_EVENT_CHAIN);
  for (int i = 0; i < event_handle.chain_index; i++) {
    if (event_handle.chains[i].main_code == main_code &&
        event_handle.chains[i].sub_code == sub_code) {
      return i;
    }
  }
  return -1;
}

void dispatch(protocol_header_t* header, uint8_t* buf) {
  int chain_idx = event_find_chain(header->main_code, header->sub_code);
  if (chain_idx != -1) {
    event_handle.chains[chain_idx].callback(event_handle.device, buf);
  }
}

protocol_header_t* protocol_header;
uint16_t recv_state;

typedef enum _uart_it_state_e {
  uart_it_state_receive_protocol_header = 0,
  uart_it_state_receive_protocol_body,
} uart_it_state_e;

void parse(block_t* block) {
  /// 协议解析
  protocol_header_t* header = (protocol_header_t*)block->buffer;
  dispatch(header, block->buffer + sizeof(protocol_header_t));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
  if (recv_state == uart_it_state_receive_protocol_header) {
    block_t* prev_block =
        buffer_get_prev_write_block(&(event_handle.device->rx_buf));
    protocol_header = prev_block->buffer;
    if (-1 == protocol_header_parse(protocol_header)) {
      error("parse protocol header failed");
    }
    if (0 == protocol_header->length) {
      prev_block->state = 1;
      block_t* block = buffer_get_write_block(&(event_handle.device->rx_buf));
      device_read_buffer(event_handle.device, block, 0,
                         sizeof(protocol_header_t));
    } else {
      recv_state = uart_it_state_receive_protocol_body;
      prev_block->block_size += protocol_header->length;
      device_read_buffer(event_handle.device, prev_block,
                         sizeof(protocol_header_t), protocol_header->length);
    }
  } else if (recv_state == uart_it_state_receive_protocol_body) {
    block_t* prev_block =
        buffer_get_prev_write_block(&(event_handle.device->rx_buf));
    prev_block->state = 1;
    recv_state = uart_it_state_receive_protocol_header;
    block_t* block = buffer_get_write_block(&(event_handle.device->rx_buf));
    device_read_buffer(event_handle.device, block, 0,
                       sizeof(protocol_header_t));
  }
}

void event_run() {
  recv_state = uart_it_state_receive_protocol_header;
  block_t* rx_block = buffer_get_write_block(&(event_handle.device->rx_buf));
  if (rx_block) {
    device_read_buffer(event_handle.device, rx_block, 0,
                       sizeof(protocol_header_t));
  }
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
  if (event_handle.discover_count % 1000 == 0) {
    event_handle.discover_count = 0;
    discover(event_handle.device);
  }
  block_t* tx_block = buffer_get_read_block(&(event_handle.device->tx_buf));
  if (0 != tx_block) {
    device_write(event_handle.device, tx_block->buffer, tx_block->block_size);
  }
  block_t* seek_block = buffer_seek_read_block(&(event_handle.device->rx_buf));
  if (seek_block) {
    if (seek_block->state) {
      block_t* rx_block = buffer_get_read_block(&(event_handle.device->rx_buf));
      parse(rx_block);
    }
  }
}
