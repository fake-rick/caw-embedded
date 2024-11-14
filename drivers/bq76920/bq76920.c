#include "bq76920.h"

#include <math.h>

#include "../utils/crypto.h"

int _I2C_READ_BYTE(bq76920_t* bq, uint16_t reg, uint8_t* data) {
  if (HAL_OK != HAL_I2C_Mem_Read(bq->hi2c, BQ76920_I2C_ADDRESS, reg,
                                 I2C_MEMADD_SIZE_8BIT, data, 1,
                                 BQ76920_I2C_TIMEOUT)) {
    return -1;
  }
  return 0;
}

int _I2C_WRITE_BYTE(bq76920_t* bq, uint16_t reg, uint8_t data) {
  uint8_t buf[3];
  uint8_t crc = 0;
  buf[0] = reg;
  buf[1] = data;
  crc = crc8_ccit_update(crc, BQ76920_I2C_ADDRESS);
  crc = crc8_ccit_update(crc, buf[0]);
  crc = crc8_ccit_update(crc, buf[1]);
  buf[2] = crc;
  if (HAL_OK != HAL_I2C_Master_Transmit(bq->hi2c, BQ76920_I2C_ADDRESS, buf,
                                        sizeof(buf), BQ76920_I2C_TIMEOUT)) {
    return -1;
  }
  return 0;
}

int bq76920_init(bq76920_t* bq, I2C_HandleTypeDef* i2c) {
  bq->hi2c = i2c;

  _I2C_WRITE_BYTE(bq, SYS_STAT, 0xff);
  _I2C_WRITE_BYTE(bq, CC_CFG, 0x19);
  _I2C_WRITE_BYTE(bq, SYS_CTRL1, 0x18);
  _I2C_WRITE_BYTE(bq, SYS_CTRL2, 0x43);

  uint8_t adc_gain_reg[2];
  _I2C_READ_BYTE(bq, ADCGAIN1, &adc_gain_reg[0]);
  _I2C_READ_BYTE(bq, ADCGAIN2, &adc_gain_reg[1]);
  uint8_t adc_gain =
      (((adc_gain_reg[1] & 0xE0) >> 5) | ((adc_gain_reg[0] & 0X0C) << 1));
  bq->gain = adc_gain + BQ76920_GAIN_BASE;

  // 获取ADCOFFSET
  _I2C_READ_BYTE(bq, ADCOFFSET, &(bq->offset));

  // 设置PROTECT1
  uint8_t protect1_reg;
  _I2C_READ_BYTE(bq, PROTECT1, &protect1_reg);
  protect1_reg = protect1_reg | (SCD_DELAY_70us << 3);
  protect1_reg = protect1_reg | SCD_THRESHOLD_22mV;
  _I2C_WRITE_BYTE(bq, PROTECT1, protect1_reg);

  // 设置PROTECT2
  uint8_t protect2_reg;
  _I2C_READ_BYTE(bq, PROTECT2, &protect2_reg);
  protect2_reg = protect2_reg | (OCD_DELAY_8ms << 4);
  protect2_reg = protect2_reg | OCD_THRESHOLD_8mV;  // 8mV / 0.5mΩ = 16A
  _I2C_WRITE_BYTE(bq, PROTECT2, protect2_reg);

  // 设置PROTECT3
  uint8_t protect3_reg;
  _I2C_READ_BYTE(bq, PROTECT3, &protect3_reg);
  protect3_reg = protect3_reg | (UV_DELAY_1s << 6);
  protect3_reg = protect3_reg | (OV_DELAY_1s << 4);
  _I2C_WRITE_BYTE(bq, PROTECT3, protect3_reg);

  // 配置过压阈值
  uint16_t OV = BQ76920_OV_TARGET * 1000.0;  // to mV
  uint16_t OV_tmp =
      (uint16_t)((float)(OV - bq->offset) / (float)(bq->gain / 1000.0));
  OV_tmp = (OV_tmp & 0x0FF0) >> 4;
  uint8_t OV_TRIP_value = OV_tmp & 0xFF;
  _I2C_WRITE_BYTE(bq, OV_TRIP, OV_TRIP_value);

  // 配置欠压阈值
  uint16_t UV = BQ76920_UV_TARGET * 1000.0;  // to mV
  uint16_t UV_tmp = (float)(UV - bq->offset) / (float)(bq->gain / 1000.0);
  UV_tmp = (UV_tmp & 0x0FF0) >> 4;
  uint8_t UV_TRIP_value = UV_tmp & 0xFF;
  _I2C_WRITE_BYTE(bq, UV_TRIP, UV_TRIP_value);
  return 0;
}

int bq76920_sys_stat(bq76920_t* bq, bq76920_sys_stat_t* st) {
  uint8_t data;
  if (0 != _I2C_READ_BYTE(bq, SYS_STAT, &data)) {
    return -1;
  }
  st->OCD = data & 0x01;
  st->SCD = (data >> 1) & 0x01;
  st->OV = (data >> 2) & 0x01;
  st->UV = (data >> 3) & 0x01;
  st->OVRD_ALERT = (data >> 4) & 0x01;
  st->DEVICE_XREADY = (data >> 5) & 0x01;
  st->RSVD = (data >> 6) & 0x01;
  st->CC_READY = (data >> 7) & 0x01;
  return 0;
}

int bq76920_sys_stat_byte(bq76920_t* bq, uint8_t* buf) {
  if (0 != _I2C_READ_BYTE(bq, SYS_STAT, buf)) {
    return -1;
  }
  return 0;
}

int bq76920_sys_ctrl2(bq76920_t* bq, bq76920_sys_ctrl2_t* st) {
  uint8_t data;
  if (0 != _I2C_READ_BYTE(bq, SYS_CTRL2, &data)) {
    return -1;
  }
  st->CHG_ON = data & 0x01;
  st->DSG_ON = (data >> 1) & 0x01;
  st->RSVD_0 = (data >> 2) & 0x01;
  st->RSVD_1 = (data >> 3) & 0x01;
  st->RSVD_2 = (data >> 4) & 0x01;
  st->CC_ONESHOT = (data >> 5) & 0x01;
  st->CC_EN = (data >> 6) & 0x01;
  st->DELAY_DIS = (data >> 7) & 0x01;
  return 0;
}

/**
 * @description: 获取均衡状态
 * @param {BQ76920_T*} bq
 * @param {BQ76920_CELLBAL1_T*} st
 * @return {*}
 */
int bq76920_cellbal1(bq76920_t* bq, bq76920_cellbal1_t* st) {
  uint8_t data;
  if (0 != _I2C_READ_BYTE(bq, CELLBAL1, &data)) {
    return -1;
  }
  st->CB1 = data & 0x01;
  st->CB2 = (data >> 1) & 0x01;
  st->CB3 = (data >> 2) & 0x01;
  st->CB4 = (data >> 3) & 0x01;
  st->CB5 = (data >> 4) & 0x01;
  return 0;
}

/**
 * @description: 更新电压数据
 * @param {BQ76920_T*} bq
 * @return {*}
 */
int bq76920_update_cell_voltage(bq76920_t* bq) {
  uint8_t reg[2];

  uint16_t VC_base = VC1_HI;

  for (int i = 0; i < 5; i++) {
    if (_I2C_READ_BYTE(bq, VC_base + i * 2, &reg[0])) {
      return -1;
    }
    if (_I2C_READ_BYTE(bq, VC_base + i * 2 + 1, &reg[1])) {
      return -1;
    }
    uint16_t v = ((reg[0] & 0x3F) << 8) | reg[1];
    bq->cell_voltage[i] =
        ((float)(bq->gain / 1000.0) * (float)v + (float)(bq->offset)) / 1000.0;
  }
  return 0;
}

int bq76920_shutdown(bq76920_t* bq) {
  _I2C_WRITE_BYTE(bq, SYS_CTRL1, 0x0);
  _I2C_WRITE_BYTE(bq, SYS_CTRL1, 0x1);
  _I2C_WRITE_BYTE(bq, SYS_CTRL1, 0x2);
}

int bq76920_update_pack_voltage(bq76920_t* bq) {
  uint8_t reg[2] = {0u, 0u};
  _I2C_READ_BYTE(bq, BAT_LO, &reg[0]);
  _I2C_READ_BYTE(bq, BAT_HI, &reg[1]);
  bq->v_pack = (float)((reg[1] << 8 | reg[0]) * (4 * (bq->gain / 1000000.0f)) +
                       (4 * (bq->offset / 1000.0f)));
}

int bq76920_update_current(bq76920_t* bq) {
  uint8_t reg[2] = {0u, 0u};
  _I2C_READ_BYTE(bq, CC_LO, &reg[0]);
  _I2C_READ_BYTE(bq, CC_HI, &reg[1]);

  int raw_data = (int16_t)((reg[1] << 8) | reg[0]);

  if (raw_data & 0x8000) {
    raw_data = -(~raw_data + 1);
  }

  if (raw_data == 1 || raw_data == -1) {
    raw_data = 0;
  }

  bq->current = (float)raw_data * 8.44 / 0.5;
}

int bq76920_update_balance_cell(bq76920_t* bq) {
  if (bq->current <= 0.0f) {
    // 不在充电状态下则关闭均衡
    _I2C_WRITE_BYTE(bq, CELLBAL1, 0x00);
    return 0;
  }
  float sum = 0.0f;
  float avg = 0.0f;
  float threshold_h = 0.01;
  float threshold_l = 0.005;
  float v[5];
  for (int i = 0; i < 5; i++) {
    sum += bq->cell_voltage[i];
  }
  avg = sum / 5;
  for (int i = 0; i < 5; i++) {
    v[i] = bq->cell_voltage[i] - avg;
  }
  float max_value = v[0];
  int index = 0;
  for (int i = 1; i < 5; i++) {
    if (max_value < v[i]) {
      index = i;
      max_value = v[i];
    }
  }
  if (max_value > threshold_h) {
    _I2C_WRITE_BYTE(bq, CELLBAL1, 0x01 << index);
  } else if (max_value < threshold_l) {
    _I2C_WRITE_BYTE(bq, CELLBAL1, 0x00);
  }
  return 0;
}

int bq76920_update_temperature(bq76920_t* bq) {
  uint8_t reg[2] = {0u, 0u};
  _I2C_READ_BYTE(bq, TS1_LO, &reg[0]);
  _I2C_READ_BYTE(bq, TS1_HI, &reg[1]);
  int raw_data = (int16_t)((reg[1] << 8) | reg[0]);

  int thermistorBetaValue = 3434;
  uint32_t vtsx = raw_data * 0.382;  // mV
  uint32_t rts = 10000.0 * vtsx / (3300.0 - vtsx);
  float tmp = 1.0 / (1.0 / (273.15 + 25) +
                     1.0 / thermistorBetaValue * log(rts / 10000.0));
  bq->temperature = (tmp - 273.15);
}

int bq76920_step(bq76920_t* bq) {
  bq76920_update_cell_voltage(bq);
  bq76920_update_pack_voltage(bq);
  bq76920_update_current(bq);
  bq76920_update_temperature(bq);
  return 0;
}