#ifndef __EVENT_H__
#define __EVENT_H__

#include <stdint.h>

#include "device.h"

#define MAX_EVENT_CHAIN 20

typedef int (*event_processer_cb)(device_t*);
typedef uint32_t (*get_tick_fn)();

typedef struct _event_chain_t {
  uint32_t main_code;
  uint32_t sub_code;
  event_processer_cb callback;
} event_chain_t;

typedef struct _event_t {
  event_chain_t chains[MAX_EVENT_CHAIN];
  uint16_t chain_index;
  device_t* device;
  get_tick_fn get_tick;
  uint32_t ping_tick;
} event_t;

void event_init(event_t* event, device_t* device, get_tick_fn get_tick);
int event_register(event_t* event, uint32_t main_code, uint32_t sub_code,
                   event_processer_cb cbfn);
void event_wait_discover(event_t* event);
void event_loop(event_t* event);
void event_timer(event_t* event);

#endif