#ifndef __PLL_H__
#define __PLL_H__
#include <stdint.h>

typedef struct _pll_t {
  uint16_t cpr;
  float bandwidth;
  float error_rate;
  int32_t shadow_count;
  int32_t count_in_cpr;
  float pos_cpr_counts;
  float pos_estimate_counts;
  float vel_estimate_counts;
  float pll_kp;
  float pll_ki;
  float pos_estimate;
  float vel_estimate;
  float pos_cpr_estimate;
} pll_t;

void pll_init(pll_t* self, uint16_t cpr, float bandwidth);
void pll_update(pll_t* self, int32_t val);
float pll_get_cpr_angle(pll_t* self);
float pll_get_angle(pll_t* self);
float pll_get_velocity(pll_t* self);

#endif