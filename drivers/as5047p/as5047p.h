#ifndef __AS5047P_H__
#define __AS5047P_H__

#include "spi.h"
#include "stdint.h"

#define SENSOR_ERROR 0xffff

typedef struct _as5047p_t {
  SPI_HandleTypeDef* spi;
  float angle_prev;
  uint64_t angle_prev_ts;
  float vel_angle_prev;
  uint64_t vel_angle_prev_ts;
  float velocity;
  int64_t full_rotations;
  int64_t vel_full_rotations;
} as5047p_t;

int as5047p_init(as5047p_t* self, SPI_HandleTypeDef* spi);
uint16_t as5047p_get_raw_count(as5047p_t* self);
int as5047p_get_sensor_angle(as5047p_t* self, float* ret);
int as5047p_update(as5047p_t* self);
float as5047p_get_velocity(as5047p_t* self);

float as5047p_get_angle(as5047p_t* self);
int64_t as5047p_get_full_rotations(as5047p_t* self);
float as5047p_get_mechanical_angle(as5047p_t* self);

uint16_t as5047p_get_cpr();

#endif