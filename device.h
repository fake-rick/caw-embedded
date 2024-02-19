#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <stdint.h>

#include "caw_config.h"

#ifdef USE_IT
typedef int (*read_fn)(void*, uint8_t*, uint16_t);
#else
typedef int (*read_fn)(void*, uint8_t*, uint16_t, uint32_t);
#endif

typedef int (*write_fn)(void*, uint8_t*, uint16_t, uint32_t);
typedef int (*reset_fn)(void*);

typedef enum _device_type_id_e {
  device_type_bms = (uint32_t)0,
  device_type_motor = (uint32_t)1,
} device_type_id_e;

typedef struct _device_t {
  uint32_t device_id;
  uint32_t type_id;
  read_fn read;
  write_fn write;
  reset_fn reset;
  void* handle;
} device_t;

void device_init(device_t* device);
void device_set_id(device_t* device, uint32_t device_id, uint32_t type_id);
int device_write(device_t* device, uint8_t* buf, uint32_t size);
int device_read(device_t* device, uint8_t* buf, uint32_t size);
int device_reset(device_t* device);
int device_abort(device_t* device);

#endif