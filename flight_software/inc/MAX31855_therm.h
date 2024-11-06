#ifndef MAX31855_therm_h
#define MAX31855_therm_h

#include "common.h"

typedef struct {
    float   tc_temp_C;
    float   cjc_temp_C;
    uint8_t oc_sc_flags;
} thermocoupleState_t;



void thermo_init(void);
void update_thermo_state(void);
thermocoupleState_t get_thermo_state(void);

#endif // ICM42670_IMU_h