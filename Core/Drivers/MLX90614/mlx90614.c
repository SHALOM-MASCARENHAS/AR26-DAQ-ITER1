#include "mlx90614.h"

#include "i2c.h"

#define MLX90614_TIMEOUT_MS            100U

static HAL_StatusTypeDef MLX90614_ReadRegister(uint8_t reg, uint16_t *value)
{
    HAL_StatusTypeDef status;
    uint8_t rxBuffer[3];

    if (value == NULL)
    {
        return HAL_ERROR;
    }

    status = HAL_I2C_Mem_Read(&hi2c1,
                              MLX90614_I2C_ADDRESS,
                              reg,
                              I2C_MEMADD_SIZE_8BIT,
                              rxBuffer,
                              sizeof(rxBuffer),
                              MLX90614_TIMEOUT_MS);

    if (status != HAL_OK)
    {
        return status;
    }

    *value = ((uint16_t)rxBuffer[1] << 8) | rxBuffer[0];

    return HAL_OK;
}

HAL_StatusTypeDef MLX90614_Init(void)
{
    uint16_t temperatureRaw;

    return MLX90614_ReadRegister(MLX90614_REG_TA, &temperatureRaw);
}

HAL_StatusTypeDef MLX90614_ReadAmbientTemperature(float *temperature)
{
    uint16_t raw;
    HAL_StatusTypeDef status;

    if (temperature == NULL)
    {
        return HAL_ERROR;
    }

    status = MLX90614_ReadRegister(MLX90614_REG_TA, &raw);

    if (status != HAL_OK)
    {
        return status;
    }

    if (raw & 0x8000U)
    {
        return HAL_ERROR;
    }

    raw &= 0x7FFFU;

    *temperature = ((float)raw * 0.02f) - 273.15f;

    return HAL_OK;
}

HAL_StatusTypeDef MLX90614_ReadObjectTemperature(float *temperature)
{
    uint16_t raw;
    HAL_StatusTypeDef status;

    if (temperature == NULL)
    {
        return HAL_ERROR;
    }

    status = MLX90614_ReadRegister(MLX90614_REG_TOBJ1, &raw);

    if (status != HAL_OK)
    {
        return status;
    }

    if (raw & 0x8000U)
    {
        return HAL_ERROR;
    }

    raw &= 0x7FFFU;

    *temperature = ((float)raw * 0.02f) - 273.15f;

    return HAL_OK;
}
