#include "./custom.h"

// 低电平进入睡眠模式，高电平使能，8~40uS低脉冲重置故障
#define _ENABLE(x) HAL_GPIO_WritePin(DRV_ENABLE_GPIO_Port, DRV_ENABLE_Pin, x)
// 放大器较准输入，逻辑高时执行自动偏移较准，校准后需要拉低
#define _CAL(x) HAL_GPIO_WritePin(DRV_CAL_GPIO_Port, DRV_CAL_Pin, x)

void motor_custom_init(drv8323_t* self) {
  _ENABLE(0);
  _CAL(1);
  HAL_Delay(10);
  _ENABLE(1);
  _CAL(0);
  HAL_Delay(10);
  drv8323_calibrate(self);
  HAL_Delay(10);
  drv8323_write_dcr(self, 0x0, DIS_GDF_DIS, 0x0, PWM_MODE_3X, 0x0, 0x0, 0x0,
                    0x0, 0x1);
  HAL_Delay(10);
  drv8323_write_csacr(self, 0x0, 0x1, 0x0, CSA_GAIN_40, 0x0, 0x1, 0x1, 0x1,
                      SEN_LVL_1_0);
  HAL_Delay(10);
  drv8323_write_csacr(self, 0x0, 0x1, 0x0, CSA_GAIN_40, 0x1, 0x0, 0x0, 0x0,
                      SEN_LVL_1_0);
  HAL_Delay(10);
  drv8323_write_ocpcr(self, TRETRY_50US, DEADTIME_50NS, OCP_NONE, OCP_DEG_8US,
                      VDS_LVL_1_88);

  drv8323_enable_gd(self);

  // drv8323_reg_val(self);
  // drv8323_print_faults(self);
}