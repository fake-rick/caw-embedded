#include "event.h"

#include <assert.h>

#include "device.h"
#include "protocols/code.h"
#include "protocols/discover.h"
#include "protocols/protocol.h"
#include "string.h"

#define TIME_OUT_ERR 3

static event_processer_cb event_find_callback(event_t* event,
                                              uint32_t main_code,
                                              uint32_t sub_code) {
  assert(event->chain_index <= MAX_EVENT_CHAIN);
  for (int i = 0; i < event->chain_index; i++) {
    if (event->chains[i].main_code == main_code &&
        event->chains[i].sub_code == sub_code) {
      return event->chains[i].callback;
    }
  }
  return 0;
}

void event_init(event_t* event, device_t* device, get_tick_fn get_tick) {
  memset(event, 0, sizeof(event_t));
  memset(&event->chains, 0xff, sizeof(event->chains));
  event->device = device;
  event->get_tick = get_tick;
}

int event_register(event_t* event, uint32_t main_code, uint32_t sub_code,
                   event_processer_cb cbfn) {
  assert(event->chain_index <= MAX_EVENT_CHAIN);
  if (event->chain_index == MAX_EVENT_CHAIN) return -1;
  //   for (int i = 0; i < MAX_EVENT_CHAIN; i++) {
  //     if (event->chains[i].main_code == main_code ||
  //         event->chains[i].sub_code == sub_code)
  //       return -1;
  //   }
  event->chains[event->chain_index].main_code = main_code;
  event->chains[event->chain_index].sub_code = sub_code;
  event->chains[event->chain_index].callback = cbfn;
  ++event->chain_index;
  return 0;
}

void event_wait_discover(event_t* event) {
  for (;;) {
    if (discover_check_magic(event->device)) {
      discover(event->device);
      event->ping_tick = event->get_tick();
      break;
    }
  }
}

void event_loop(event_t* event) {
  for (;;) {
    event_wait_discover(event);
    for (;;) {
      protocol_header_t header;
      if (protocol_header_recv(event->device, &header)) {
        break;
      }
      if (header.main_code == main_code_system &&
          header.sub_code == sub_code_system_ping) {
        event->ping_tick = event->get_tick();
        protocol_header_write(event->device, main_code_system,
                              sub_code_system_pong, 0, 0);
      } else {
        event_processer_cb cbfn =
            event_find_callback(event, header.main_code, header.sub_code);
        if (cbfn != 0) cbfn(event->device);
      }
    }
  }
}

void event_timer(event_t* event) {
  if (event->get_tick() - event->ping_tick >= 9000) {
    device_reset(event->device);
  }
}