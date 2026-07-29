#ifndef MPU6500_H
#define MPU6500_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

/*=============================
    I2C Address
=============================*/

#define MPU6500_I2C_ADDRESS        (0x68U << 1)

/*=============================
    Registers
=============================*/

#define MPU6500_REG_SMPLRT_DIV     0x19U
#define MPU6500_REG_CONFIG         0x1AU
#define MPU6500_REG_GYRO_CONFIG    0x1BU
#define MPU6500_REG_ACCEL_CONFIG   0x1CU
#define MPU6500_REG_ACCEL_CONFIG2  0x1DU

#define MPU6500_REG_INT_PIN_CFG    0x37U
#define MPU6500_REG_INT_ENABLE     0x38U

#define MPU6500_REG_ACCEL_XOUT_H   0x3BU

#define MPU6500_REG_TEMP_OUT_H     0x41U

#define MPU6500_REG_GYRO_XOUT_H    0x43U

#define MPU6500_REG_PWR_MGMT_1     0x6BU
#define MPU6500_REG_PWR_MGMT_2     0x6CU

#define MPU6500_REG_WHO_AM_I       0x75U

/*=============================
    WHO_AM_I
=============================*/

#define MPU6500_DEVICE_ID          0x70U

/*=============================
    Public API
=============================*/

HAL_StatusTypeDef MPU6500_Init(void);
HAL_StatusTypeDef MPU6500_CalibrateGyro(void);
HAL_StatusTypeDef MPU6500_ReadAll(float *ax,
                                  float *ay,
                                  float *az,
                                  float *gx,
                                  float *gy,
                                  float *gz,
                                  float *temperature);

#ifdef __cplusplus
}
#endif

#endif
