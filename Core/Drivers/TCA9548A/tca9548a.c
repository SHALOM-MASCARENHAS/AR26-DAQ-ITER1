#include "tca9548a.h"

#include "i2c.h"

/*==============================================================================
 * Private Function Prototypes
 *============================================================================*/

static HAL_StatusTypeDef TCA9548A_Write(uint8_t value);
static HAL_StatusTypeDef TCA9548A_Read(uint8_t *value);

/*==============================================================================
 * Private Functions
 *============================================================================*/

/**
 * @brief Write the TCA9548A control register.
 *
 * @param value Control register value.
 *
 * @return HAL status.
 */
static HAL_StatusTypeDef TCA9548A_Write(uint8_t value)
{
    return HAL_I2C_Master_Transmit(&hi2c1,
                                   TCA9548A_I2C_ADDRESS,
                                   &value,
                                   1U,
                                   HAL_MAX_DELAY);
}

/**
 * @brief Read the TCA9548A control register.
 *
 * @param value Pointer to store register value.
 *
 * @return HAL status.
 */
static HAL_StatusTypeDef TCA9548A_Read(uint8_t *value)
{
    if (value == NULL)
    {
        return HAL_ERROR;
    }

    return HAL_I2C_Master_Receive(&hi2c1,
                                  TCA9548A_I2C_ADDRESS,
                                  value,
                                  1U,
                                  HAL_MAX_DELAY);
}

/*==============================================================================
 * Public Functions
 *============================================================================*/

/**
 * @brief Initialize the TCA9548A.
 *
 * Verifies communication and disables all channels.
 *
 * @return HAL status.
 */
HAL_StatusTypeDef TCA9548A_Init(void)
{
    uint8_t control;

    /* Verify communication */
    if (TCA9548A_Read(&control) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Disable all channels */
    if (TCA9548A_DisableAll() != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Verify all channels are disabled */
    if (TCA9548A_Read(&control) != HAL_OK)
    {
        return HAL_ERROR;
    }

    if (control != 0x00U)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

/**
 * @brief Select one channel.
 *
 * @param channel Channel number (0-7).
 *
 * @return HAL status.
 */
HAL_StatusTypeDef TCA9548A_SelectChannel(uint8_t channel)
{
    if (channel > 7U)
    {
        return HAL_ERROR;
    }

    return TCA9548A_Write((uint8_t)(1U << channel));
}

/**
 * @brief Disable all channels.
 *
 * @return HAL status.
 */
HAL_StatusTypeDef TCA9548A_DisableAll(void)
{
    return TCA9548A_Write(0x00U);
}

/**
 * @brief Read the control register.
 *
 * @param control Pointer to store register value.
 *
 * @return HAL status.
 */
HAL_StatusTypeDef TCA9548A_ReadControl(uint8_t *control)
{
    return TCA9548A_Read(control);
}
