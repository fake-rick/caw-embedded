#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdint.h>

#define MAX_BLOCK_SIZE 50
#define MAX_BUFFER_SIZE 300

typedef struct _block_t {
  uint8_t buffer[MAX_BUFFER_SIZE];
  uint32_t block_size;
  uint8_t state;
} block_t;

typedef struct _buffer_t {
  block_t pool[MAX_BLOCK_SIZE];
  block_t* prev_w_block;
  uint64_t w;
  uint64_t r;
  uint8_t overflow;
} buffer_t;

void buffer_init(buffer_t* buf);
int buffer_write_block(buffer_t* buf, const uint8_t* src, uint32_t size);
block_t* buffer_get_write_block(buffer_t* buf);
block_t* buffer_get_read_block(buffer_t* buf);
block_t* buffer_get_prev_write_block(buffer_t* buf);
block_t* buffer_seek_read_block(buffer_t* buf);

#endif