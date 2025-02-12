#include "device.h"
#include "./protocols/code.h"
#include "./utils/endian.h"
#include "cmsis_os.h"
#include "dma.h"
#include "task.h"
#include <assert.h>
#include <string.h>

#define MAX_DELAY 0xFFFFFFFFU

extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;

static volatile uint8_t TRANSMIT_BUSY = 0;

void device_init(device_t* device) {
  memset(device, 0, sizeof(device_t));
  buffer_init(&(device->tx_buf));
  device->handle = UART_HANDLE;
  device->read = HAL_UART_Receive_DMA;
  device->write = HAL_UART_Transmit_DMA;
}

void device_set_id(device_t* device, uint32_t device_id, uint32_t type_id) {
  device->device_id = device_id;
  device->type_id = type_id;
}

#define write(device, buf, size) device->write(device->handle, buf, size)
#define read(device, buf, size) device->read(device->handle, buf, size)

int device_write(device_t* device, const uint8_t* buf, uint16_t size) {
  assert(device->write);
  while (HAL_UART_STATE_BUSY_TX ==
         ((UART_HandleTypeDef*)(device->handle))->gState)
    ;
  write(device, buf, size);

  // while (RESET == __HAL_UART_GET_FLAG((UART_HandleTypeDef*)(device->handle),
  //                                     UART_FLAG_TC)) {
  // }
  return 0;
}

int device_read(device_t* device, uint8_t* buf, uint16_t size) {
  assert(device->read);
  return read(device, buf, size);
}

int device_write_buffer(device_t* device, const uint8_t* buf, uint16_t size) {
  if (0 != buffer_write_block(&(device->tx_buf), buf, size))
    return -1;

  block_t* block = buffer_get_read_block(&(device->tx_buf));
  if (0 == block) {
    return -1;
  }
  return device_write(device, block->buffer, block->block_size);

  // return buffer_write_block(&(device->tx_buf), buf, size);
}

int device_event_step(device_t* device) {
  block_t* block = buffer_get_read_block(&(device->tx_buf));
  if (0 == block) {
    return -1;
  }
  return device_write(device, block->buffer, block->block_size);
}