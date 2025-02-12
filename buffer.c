#include "./buffer.h"
#include "./log.h"
#include <assert.h>
#include <string.h>

void buffer_init(buffer_t* buf) { memset(buf, 0, sizeof(buf)); }

int buffer_write_block(buffer_t* buf, const uint8_t* src, uint32_t size) {
  assert(size <= MAX_BUFFER_SIZE);
  while (buf->mutex)
    ;
  buf->mutex = 1;

  if (buf->overflow) {
    assert(buf->w <= buf->r);
    // 在溢出状态时如果write指针等于read指针时证明无空闲缓冲区可用
    if (buf->w == buf->r) {
      buf->mutex = 0;
      return -1;
    }
  }
  // 写入本次存储的字节数并获取缓冲区地址用于返回
  memcpy(buf->pool[buf->w].buffer, src, size);
  buf->pool[buf->w].block_size = size;
  buf->w = ++buf->w % MAX_BLOCK_SIZE;

  // 如果余数为0表示w已经写完一圈并回到0,溢出标志置位
  // 这时候的read指针值大于write指针
  // write指针写入时需要判断是否小于read指针,否则会覆盖未读数据
  if (0 == buf->w)
    buf->overflow = 1;
  buf->mutex = 0;
  return 0;
}

block_t* buffer_get_write_block(buffer_t* buf) {
  block_t* block = 0;
  while (buf->mutex)
    ;
  buf->mutex = 1;

  if (buf->overflow) {
    assert(buf->w <= buf->r);
    // 在溢出状态时如果write指针等于read指针时证明无空闲缓冲区可用
    if (buf->w == buf->r) {
      buf->mutex = 0;
      return -1;
    }
  }
  // 写入本次存储的字节数并获取缓冲区地址用于返回
  block = &(buf->pool[buf->w]);
  buf->w = ++buf->w % MAX_BLOCK_SIZE;

  // 如果余数为0表示w已经写完一圈并回到0,溢出标志置位
  // 这时候的read指针值大于write指针
  // write指针写入时需要判断是否小于read指针,否则会覆盖未读数据
  if (0 == buf->w)
    buf->overflow = 1;
  buf->mutex = 0;
  return block;
}

block_t* buffer_get_read_block(buffer_t* buf) {
  while (buf->mutex)
    ;
  buf->mutex = 1;
  block_t* ret;
  if (!buf->overflow) {
    assert(buf->r <= buf->w);
    if (buf->r == buf->w) {
      buf->mutex = 0;
      return 0;
    }
  }
  ret = &(buf->pool[buf->r]);
  buf->r = ++buf->r % MAX_BLOCK_SIZE;
  if (0 == buf->r)
    buf->overflow = 0;
  buf->mutex = 0;
  return ret;
}