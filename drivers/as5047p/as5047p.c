#include "./as5047p.h"

#include <stdbool.h>
#include <stdlib.h>

#include "../macros/helper.h"
#include "caw_config.h"
#include "log.h"
#include "math.h"

#define NOP 0x0000
#define ERRFL 0x0001
#define PROG 0x0003
#define DIAAGC 0x3ffc
#define MAG 0x3ffd
#define ANGLEUNC 0x3ffe
#define ANGLECOM 0x3fff
#define OP_WRITE 0x0000
#define OP_READ 0x0001
#define BIT_RESOLUTION 14
#define CPR (1 << (BIT_RESOLUTION))
#define MIN_ELAPSED_TIME 0.000100

#define _NSS(x) HAL_GPIO_WritePin(AS5047P_NSS_GPIO_Port, AS5047P_NSS_Pin, x)
// #define _NSS(x) HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, x)

uint16_t _even_check(uint16_t val) {
  val ^= val >> 8;
  val ^= val >> 4;
  val ^= val >> 2;
  val ^= val >> 1;
  return (val & 1);
}

uint16_t _cmd_frame(uint16_t val, uint16_t op) {
  val |= (op << 14);
  val |= (_even_check(val) << 15);
  return val;
}

uint16_t _write(as5047p_t* self, uint16_t val) {
  uint16_t ret;
  _NSS(0);
  if (HAL_OK !=
      HAL_SPI_TransmitReceive(self->spi, &val, &ret, 1, HAL_MAX_DELAY)) {
    error("as5047p spi transmit failed");
  }
  _NSS(1);
  return ret;
}

int as5047p_init(as5047p_t* self, SPI_HandleTypeDef* spi) {
  self->spi = spi;
  self->angle_prev = 0.0;
  self->angle_prev_ts = 0;
  self->vel_angle_prev = 0.0;
  self->vel_angle_prev_ts = 0;
  self->velocity = 0.0;
  self->full_rotations = 0;
  self->vel_full_rotations = 0;
  return 0;
}

uint16_t as5047p_get_raw_count(as5047p_t* self) {
  _write(self, _cmd_frame(ANGLECOM, OP_READ));
  uint16_t val = _write(self, _cmd_frame(NOP, OP_READ));
  if ((val & ((uint16_t)1 << 14)) > 0) {
    uint16_t err = _write(self, _cmd_frame(ERRFL, OP_READ)) & (uint16_t)0x0003;
    error("as5047p read reg failed: %d", err);
    return SENSOR_ERROR;
  }
  if ((val >> 15) != _even_check(val & 0x7fff)) {
    error("as5047p check failed");
    return SENSOR_ERROR;
  }
  return (val & 0x3fff);
}

int as5047p_get_sensor_angle(as5047p_t* self, float* ret) {
  uint16_t val = as5047p_get_raw_count(self);
  if (SENSOR_ERROR == val) {
    return -1;
  }
  *ret = ((float)val / (float)CPR) * _2PI;
  return 0;
}

int as5047p_update(as5047p_t* self) {
  float val;
  if (-1 == as5047p_get_sensor_angle(self, &val) || val < 0.0) {
    return -1;
  }
  self->angle_prev_ts = HAL_GetTick();
  float d_angle = val - self->angle_prev;
  if (_abs(d_angle) > (0.8 * _2PI)) {
    if (d_angle > 0.0) {
      self->full_rotations -= 1;
    } else {
      self->full_rotations += 1;
    }
  }
  self->angle_prev = val;
  return 0;
}

float as5047p_get_velocity(as5047p_t* self) {
  float ts = (float)(self->angle_prev_ts - self->vel_angle_prev_ts) * 1e-3;
  if (ts < 0.0) {
    self->vel_angle_prev = self->angle_prev;
    self->vel_full_rotations = self->full_rotations;
    self->vel_angle_prev_ts = self->angle_prev_ts;
    return self->velocity;
  }
  if (ts < MIN_ELAPSED_TIME) {
    return self->velocity;
  }
  self->velocity =
      ((float)(self->full_rotations - self->vel_full_rotations) * _2PI +
       (self->angle_prev - self->vel_angle_prev)) /
      ts;
  self->vel_angle_prev = self->angle_prev;
  self->vel_full_rotations = self->full_rotations;
  self->vel_angle_prev_ts = self->angle_prev_ts;
  return self->velocity;
}

float as5047p_get_angle(as5047p_t* self) {
  float ret = (float)self->full_rotations * _2PI + self->angle_prev;
  return ret;
}

int64_t as5047p_get_full_rotations(as5047p_t* self) {
  return self->full_rotations;
}

float as5047p_get_mechanical_angle(as5047p_t* self) { return self->angle_prev; }

uint16_t as5047p_get_cpr() { return CPR; }