#include "device.h"

#include <assert.h>
#include <string.h>

#include "./protocols/code.h"
#include "utils/endian.h"

#define MAX_DELAY 0xFFFFFFFFU
// #define MAX_DELAY 100

void device_init(device_t* device, void* handle, read_write_fn read,
                 read_write_fn write, reset_fn reset) {
  memset(device, 0, sizeof(device_t));
  device->handle = handle;
  device->read = read;
  device->write = write;
  device->reset = reset;
}

void device_set_id(device_t* device, uint32_t device_id, uint32_t type_id) {
  device->device_id = device_id;
  device->type_id = type_id;
}

#define write(device, buf, size) \
  device->write(device->handle, buf, size, MAX_DELAY)
#define read(device, buf, size) \
  device->read(device->handle, buf, size, MAX_DELAY)

int device_write(device_t* device, uint8_t* buf, uint32_t size) {
  assert(device->write);
  return write(device, buf, size);
}
int device_read(device_t* device, uint8_t* buf, uint32_t size) {
  assert(device->read);
  return read(device, buf, size);
}
int device_reset(device_t* device) {
  assert(device->reset);
  return device->reset(device->handle);
}