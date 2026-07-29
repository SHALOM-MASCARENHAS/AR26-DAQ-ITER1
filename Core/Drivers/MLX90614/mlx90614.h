#ifndef MLX90614_H
#define MLX90614_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

/* MLX90614 I2C Address */
#define MLX90614_I2C_ADDRESS           (0x5AU << 1)

/* RAM Register Addresses */
#define MLX90614_REG_TA                0x06U
#define MLX90614_REG_TOBJ1             0x07U
#define MLX90614_REG_TOBJ2             0x08U

HAL_StatusTypeDef MLX90614_Init(void);

HAL_StatusTypeDef MLX90614_ReadAmbientTemperature(float *temperature);

HAL_StatusTypeDef MLX90614_ReadObjectTemperature(float *temperature);

#ifdef __cplusplus
}
#endif

#endif /* MLX90614_H */
