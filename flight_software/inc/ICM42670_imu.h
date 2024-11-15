#ifndef ICM42670_IMU_h
#define ICM42670_IMU_h

#include "common.h"

typedef struct {
    float   accel_x_g;
    float   accel_y_g;
    float   accel_z_g;

    float   gyro_x_dps;
    float   gyro_y_dps;
    float   gyro_z_dps;
} imuState_t;

void imu_init(void);
void update_imu_state(void);
imuState_t get_imu_state(void);


#endif // ICM42670_IMU_h