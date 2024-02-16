#include "bms.h"

#include "code.h"
#include "endian.h"
#include "utils.h"

void protocol_write_bms_info(device_t* device, protocol_bms_t* t) {
  for (int i = 0; i < SIZEOF_ARRAY(t->cell_voltage); i++) {
    t->cell_voltage[i] = endian_u32(t->cell_voltage[i]);
  }
  t->voltage = endian_u32(t->voltage);
  t->current = endian_u32(t->current);
  t->temperature = endian_u32(t->temperature);
  t->soc = endian_u32(t->soc);
  t->soh = endian_u32(t->soh);
  protocol_header_write(device, main_code_bms, sub_code_bms_info, 0,
                        sizeof(protocol_bms_t));
  device_write(device, t, sizeof(protocol_bms_t));
}