#ifndef __PID_H__
#define __PID_H__

#include <stdint.h>

typedef struct _pid_t{
  float p;           // 比例增益
  float i;           // 积分增益
  float d;           // 微分增益
  float output_ramp; // 输出值的最大变化速度
  float limit;       // 最大输出值

  float error_prev;        // 跟踪最后一次误差值
  float output_prev;       // 最后一次PID输出值
  float integral_prev;     // 最后一次积分值
  uint64_t timestamp_prev; // 记录最后一次时间戳
} pid_t;

void pid_init(pid_t *self, float p, float i, float d, float ramp, float limit);
float pid_calc(pid_t *self, float error);
void pid_reset(pid_t *self);

#endif