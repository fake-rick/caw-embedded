#include "device.h"

#include <assert.h>
#include <string.h>

#include "./protocols/code.h"
#include "utils/endian.h"

#ifdef USE_STM32_UART

int reset_usart_device(void* handle) {
  HAL_UART_DeInit(handle);
  HAL_UART_Init(handle);
  // device_abort(handle);
}
#endif

#define MAX_DELAY 0xFFFFFFFFU

void device_init(device_t* device) {
  memset(device, 0, sizeof(device_t));
#ifdef USE_STM32_UART
  device->handle = UART_HANDLE;
#ifdef USE_IT
  device->read = HAL_UART_Receive_IT;
#else
  device->read = HAL_UART_Receive;
#endif
  device->write = HAL_UART_Transmit;
  device->reset = reset_usart_device;
#endif
}

void device_set_id(device_t* device, uint32_t device_id, uint32_t type_id) {
  device->device_id = device_id;
  device->type_id = type_id;
}

#define write(device, buf, size) \
  device->write(device->handle, buf, size, MAX_DELAY)

#ifdef USE_IT
#define read(device, buf, size) device->read(device->handle, buf, size)
#else
#define read(device, buf, size) \
  device->read(device->handle, buf, size, MAX_DELAY)
#endif

int device_write(device_t* device, uint8_t* buf, uint32_t size) {
  assert(device->write);
  return write(device, buf, size);
}

int device_read(device_t* device, uint8_t* buf, uint32_t size) {
  assert(device->read);
  return read(device, buf, size);
}

int device_abort(device_t* device) {
#ifdef USE_IT
  HAL_UART_Abort_IT(&huart1);
#else
  HAL_UART_Abort(&huart1);
#endif
}

int device_reset(device_t* device) {
  assert(device->reset);
  return device->reset(device->handle);
}
