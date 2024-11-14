#ifndef __LOWPASS_FILTER_H__
#define __LOWPASS_FILTER_H__

#include "stdint.h"

typedef struct _lowpass_filter_t {
  float tf;                // 低通滤波时间常数
  uint64_t timestamp_prev; // 记录上次执行的时间戳
  float y_prev;            // 上一个时间步的过滤值
} lowpass_filter_t;

void lowpass_filter_init(lowpass_filter_t* self, float time_constant);
float lowpass_filter_calc(lowpass_filter_t* self, float x);

#endif