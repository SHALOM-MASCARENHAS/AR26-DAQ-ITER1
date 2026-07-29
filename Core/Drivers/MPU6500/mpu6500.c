#include "mpu6500.h"
#include <math.h>
#include "i2c.h"
/*=============================
    Configuration
=============================*/

#define MPU6500_RESET_DELAY_MS      100U

#define MPU6500_SMPLRT_DIV_VALUE    9U

#define MPU6500_CONFIG_VALUE        0x04U

#define MPU6500_GYRO_CONFIG_VALUE   0x18U

#define MPU6500_ACCEL_CONFIG_VALUE  0x18U

#define MPU6500_ACCEL_CONFIG2_VALUE 0x04U
/*=============================
    Conversion Factors
=============================*/

#define MPU6500_ACCEL_SENSITIVITY      2048.0f
#define MPU6500_GYRO_SENSITIVITY       16.4f
#define MPU6500_TEMPERATURE_OFFSET     21.0f
#define MPU6500_TEMPERATURE_SENSITIVITY 333.87f
#define MPU6500_GYRO_CALIBRATION_SAMPLES    500U
#define MPU6500_GYRO_MOTION_THRESHOLD_DPS   3.0f
#define MPU6500_GYRO_CALIBRATION_DELAY_MS   5U
/*=============================
    Private Function Prototypes
=============================*/

static HAL_StatusTypeDef MPU6500_WriteRegister(uint8_t reg,
                                               uint8_t value);

static HAL_StatusTypeDef MPU6500_ReadRegister(uint8_t reg,
                                              uint8_t *value);

static HAL_StatusTypeDef MPU6500_ReadRegisters(uint8_t reg,
                                               uint8_t *buffer,
                                               uint16_t length);
static float gyroBiasX = 0.0f;
static float gyroBiasY = 0.0f;
static float gyroBiasZ = 0.0f;
/*=============================
    Private Functions
=============================*/

static HAL_StatusTypeDef MPU6500_WriteRegister(uint8_t reg,
                                               uint8_t value)
{
    return HAL_I2C_Mem_Write(&hi2c1,
                             MPU6500_I2C_ADDRESS,
                             reg,
                             I2C_MEMADD_SIZE_8BIT,
                             &value,
                             1U,
                             HAL_MAX_DELAY);
}

static HAL_StatusTypeDef MPU6500_ReadRegister(uint8_t reg,
                                              uint8_t *value)
{
    return HAL_I2C_Mem_Read(&hi2c1,
                            MPU6500_I2C_ADDRESS,
                            reg,
                            I2C_MEMADD_SIZE_8BIT,
                            value,
                            1U,
                            HAL_MAX_DELAY);
}

static HAL_StatusTypeDef MPU6500_ReadRegisters(uint8_t reg,
                                               uint8_t *buffer,
                                               uint16_t length)
{
    return HAL_I2C_Mem_Read(&hi2c1,
                            MPU6500_I2C_ADDRESS,
                            reg,
                            I2C_MEMADD_SIZE_8BIT,
                            buffer,
                            length,
                            HAL_MAX_DELAY);
}
HAL_StatusTypeDef MPU6500_Init(void)
{
    uint8_t whoAmI;

    /* Verify device */
    if (MPU6500_ReadRegister(MPU6500_REG_WHO_AM_I,
                             &whoAmI) != HAL_OK)
    {
        return HAL_ERROR;
    }

    if (whoAmI != MPU6500_DEVICE_ID)
    {
        return HAL_ERROR;
    }

    /* Software reset */
    if (MPU6500_WriteRegister(MPU6500_REG_PWR_MGMT_1,
                              0x80U) != HAL_OK)
    {
        return HAL_ERROR;
    }

    HAL_Delay(MPU6500_RESET_DELAY_MS);

    /* Select PLL with X Gyro reference */
    if (MPU6500_WriteRegister(MPU6500_REG_PWR_MGMT_1,
                              0x01U) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Enable all accel and gyro axes */
    if (MPU6500_WriteRegister(MPU6500_REG_PWR_MGMT_2,
                              0x00U) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Digital Low Pass Filter */
    if (MPU6500_WriteRegister(MPU6500_REG_CONFIG,
                              MPU6500_CONFIG_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Sample Rate Divider */
    if (MPU6500_WriteRegister(MPU6500_REG_SMPLRT_DIV,
                              MPU6500_SMPLRT_DIV_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Gyroscope ±2000 dps */
    if (MPU6500_WriteRegister(MPU6500_REG_GYRO_CONFIG,
                              MPU6500_GYRO_CONFIG_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Accelerometer ±16 g */
    if (MPU6500_WriteRegister(MPU6500_REG_ACCEL_CONFIG,
                              MPU6500_ACCEL_CONFIG_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Accelerometer DLPF */
    if (MPU6500_WriteRegister(MPU6500_REG_ACCEL_CONFIG2,
                              MPU6500_ACCEL_CONFIG2_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }
    uint8_t accelConfig = 0;
    MPU6500_ReadRegister(MPU6500_REG_ACCEL_CONFIG, &accelConfig);
    uint8_t accelConfig2 = 0;
    MPU6500_ReadRegister(MPU6500_REG_ACCEL_CONFIG2, &accelConfig2);
    return HAL_OK;
}
HAL_StatusTypeDef MPU6500_CalibrateGyro(void)
{
    float ax, ay, az;
    float gx, gy, gz;
    float temp;

    float sumX = 0.0f;
    float sumY = 0.0f;
    float sumZ = 0.0f;

    gyroBiasX = 0.0f;
    gyroBiasY = 0.0f;
    gyroBiasZ = 0.0f;

    for (uint32_t i = 0; i < MPU6500_GYRO_CALIBRATION_SAMPLES; i++)
    {
        if (MPU6500_ReadAll(&ax,
                            &ay,
                            &az,
                            &gx,
                            &gy,
                            &gz,
                            &temp) != HAL_OK)
        {
            return HAL_ERROR;
        }

        if ((fabsf(gx) > MPU6500_GYRO_MOTION_THRESHOLD_DPS) ||
            (fabsf(gy) > MPU6500_GYRO_MOTION_THRESHOLD_DPS) ||
            (fabsf(gz) > MPU6500_GYRO_MOTION_THRESHOLD_DPS))
        {
            return HAL_ERROR;
        }

        sumX += gx;
        sumY += gy;
        sumZ += gz;

        HAL_Delay(MPU6500_GYRO_CALIBRATION_DELAY_MS);
    }

    gyroBiasX = sumX / MPU6500_GYRO_CALIBRATION_SAMPLES;
    gyroBiasY = sumY / MPU6500_GYRO_CALIBRATION_SAMPLES;
    gyroBiasZ = sumZ / MPU6500_GYRO_CALIBRATION_SAMPLES;

    return HAL_OK;
}
HAL_StatusTypeDef MPU6500_ReadAll(float *ax,
                                  float *ay,
                                  float *az,
                                  float *gx,
                                  float *gy,
                                  float *gz,
                                  float *temperature)
{
    uint8_t buffer[14];

    int16_t accelX;
    int16_t accelY;
    int16_t accelZ;

    int16_t temp;

    int16_t gyroX;
    int16_t gyroY;
    int16_t gyroZ;

    if (MPU6500_ReadRegisters(MPU6500_REG_ACCEL_XOUT_H,
                              buffer,
                              sizeof(buffer)) != HAL_OK)
    {
        return HAL_ERROR;
    }

    accelX = (int16_t)((buffer[0] << 8) | buffer[1]);
    accelY = (int16_t)((buffer[2] << 8) | buffer[3]);
    accelZ = (int16_t)((buffer[4] << 8) | buffer[5]);

    temp = (int16_t)((buffer[6] << 8) | buffer[7]);

    gyroX = (int16_t)((buffer[8] << 8) | buffer[9]);
    gyroY = (int16_t)((buffer[10] << 8) | buffer[11]);
    gyroZ = (int16_t)((buffer[12] << 8) | buffer[13]);

    *ax = (float)accelX / MPU6500_ACCEL_SENSITIVITY;
    *ay = (float)accelY / MPU6500_ACCEL_SENSITIVITY;
    *az = (float)accelZ / MPU6500_ACCEL_SENSITIVITY;

    *gx = ((float)gyroX / MPU6500_GYRO_SENSITIVITY) - gyroBiasX;
    *gy = ((float)gyroY / MPU6500_GYRO_SENSITIVITY) - gyroBiasY;
    *gz = ((float)gyroZ / MPU6500_GYRO_SENSITIVITY) - gyroBiasZ;

    *temperature = ((float)temp / MPU6500_TEMPERATURE_SENSITIVITY)
                   + MPU6500_TEMPERATURE_OFFSET;

    return HAL_OK;
}
