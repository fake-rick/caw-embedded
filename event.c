#include "event.h"

#include <assert.h>

#include "caw_config.h"
#include "device.h"
#include "protocols/code.h"
#include "protocols/discover.h"
#include "protocols/protocol.h"
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
  return 0;
}

static void dispatch(protocol_header_t* header, uint8_t buf) {
  if (header->main_code == main_code_system &&
      header->sub_code == sub_code_system_ping) {
    event_handle.ping_tick = event_handle.get_tick();
    protocol_header_write(event_handle.device, main_code_system,
                          sub_code_system_pong, 0, 0);
  } else {
    int chain_idx = event_find_chain(header->main_code, header->sub_code);
    if (chain_idx != -1) {
      event_handle.chains[chain_idx].callback(event_handle.device, buf);
    }
  }
}

#ifdef USE_STM32_UART
#ifdef USE_IT
protocol_header_t protocol_header;
static uint16_t recv_state;

typedef enum _uart_it_state_e {
  uart_it_state_receive_discover = 0,
  uart_it_state_receive_protocol_header,
  uart_it_state_receive_protocol_body,
} uart_it_state_e;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
  if (huart == UART_HANDLE) {
    /// DISCOVER处理
    if (recv_state == uart_it_state_receive_discover) {
      recv_state = uart_it_state_receive_discover;
      if (!discover_check_magic_buf(uart_recv_buf)) {
        event_handle.device->read(UART_HANDLE, uart_recv_buf,
                                  sizeof(DISCOVER_MAGIC));
        return;
      }
      event_handle.ping_tick = event_handle.get_tick();
      discover(event_handle.device);

      recv_state = uart_it_state_receive_protocol_header;
      event_handle.device->read(UART_HANDLE, &protocol_header,
                                sizeof(protocol_header_t));
      return;
    }
    /// 协议头接收解析
    if (recv_state == uart_it_state_receive_protocol_header) {
      if (protocol_header_parse(&protocol_header)) {
        recv_state = uart_it_state_receive_discover;
        event_handle.device->read(UART_HANDLE, uart_recv_buf,
                                  sizeof(DISCOVER_MAGIC));
        return;
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
  recv_state = uart_it_state_receive_discover;
  event_handle.device->read(UART_HANDLE, uart_recv_buf, sizeof(DISCOVER_MAGIC));
}

#else
void event_run() {
  for (;;) {
    event_wait_discover(&event_handle);
    event_handle.ping_tick = event_handle.get_tick();
    for (;;) {
      protocol_header_t header;
      if (protocol_header_recv(event_handle.device, &header)) {
        break;
      }
      if (header.data_size <= 0) {
        dispatch(&header, 0);
      } else {
        if (device_read(event_handle.device, uart_recv_buf, header.data_size)) {
          break;
        }
        dispatch(&header, uart_recv_buf);
      }
    }
  }
}
#endif
#endif

void event_init(device_t* device) {
  memset(&event_handle, 0, sizeof(event_t));
  memset(&(event_handle.chains), 0xff, sizeof(event_handle.chains));
  event_handle.device = device;
#ifdef USE_STM32_UART
  event_handle.get_tick = HAL_GetTick;
#endif
}

int event_register(uint32_t main_code, uint32_t sub_code,
                   event_processer_cb cbfn) {
  assert(event_handle.chain_index <= MAX_EVENT_CHAIN);

  if (event_handle.chain_index == MAX_EVENT_CHAIN) return -1;

  event_handle.chains[event_handle.chain_index].main_code = main_code;
  event_handle.chains[event_handle.chain_index].sub_code = sub_code;
  event_handle.chains[event_handle.chain_index].callback = cbfn;
  ++event_handle.chain_index;
  return 0;
}

void event_wait_discover() {
  for (;;) {
    if (discover_check_magic(event_handle.device)) {
      discover(event_handle.device);
      event_handle.ping_tick = event_handle.get_tick();
      break;
    }
  }
}

void event_timer() {
  if (event_handle.get_tick() - event_handle.ping_tick >= 10000) {
    device_reset(event_handle.device);
    event_run();
  }
}