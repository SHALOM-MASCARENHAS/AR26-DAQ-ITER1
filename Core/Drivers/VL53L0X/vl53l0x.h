#ifndef VL53L0X_H
#define VL53L0X_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

/* Status codes */
typedef enum
{
    VL53L0X_OK = 0,
    VL53L0X_ERROR,
    VL53L0X_TIMEOUT,
    VL53L0X_NOT_PRESENT
} VL53L0X_Status_t;

/**
 * @brief Initialize the VL53L0X sensor.
 *
 * @param hi2c Pointer to the I2C peripheral used by the sensor.
 * @return VL53L0X_Status_t
 */
VL53L0X_Status_t VL53L0X_Init(I2C_HandleTypeDef *hi2c);

/**
 * @brief Read a single distance measurement.
 *
 * @param distance_mm Pointer to store measured distance in millimetres.
 * @return VL53L0X_Status_t
 */
VL53L0X_Status_t VL53L0X_ReadDistance(uint16_t *distance_mm);

#ifdef __cplusplus
}
#endif

#endif /* VL53L0X_H */
