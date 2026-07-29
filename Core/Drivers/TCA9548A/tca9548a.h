#ifndef TCA9548A_H
#define TCA9548A_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

/*==============================================================================
 * TCA9548A I2C Multiplexer
 *============================================================================*/

/* 7-bit I2C Address: 0x70
 * HAL expects the address left-shifted by one bit.
 */
#define TCA9548A_I2C_ADDRESS         (0x70U << 1)

/*==============================================================================
 * Channel Definitions
 *============================================================================*/

#define TCA9548A_CHANNEL_LEFT_MLX    0U
#define TCA9548A_CHANNEL_RIGHT_MLX   1U
#define TCA9548A_CHANNEL_LCD         2U

/*==============================================================================
 * Public Function Prototypes
 *============================================================================*/

/**
 * @brief Initialize the TCA9548A.
 *
 * Verifies communication with the device and disables all channels.
 *
 * @return HAL_OK on success.
 * @return HAL_ERROR on failure.
 */
HAL_StatusTypeDef TCA9548A_Init(void);

/**
 * @brief Select a single TCA9548A channel.
 *
 * Any previously enabled channel is automatically disabled.
 *
 * @param channel Channel number (0-7).
 *
 * @return HAL_OK on success.
 * @return HAL_ERROR on invalid channel or I2C failure.
 */
HAL_StatusTypeDef TCA9548A_SelectChannel(uint8_t channel);

/**
 * @brief Disable all channels.
 *
 * @return HAL_OK on success.
 * @return HAL_ERROR on I2C failure.
 */
HAL_StatusTypeDef TCA9548A_DisableAll(void);

/**
 * @brief Read the TCA9548A control register.
 *
 * @param control Pointer to store the control register value.
 *
 * @return HAL_OK on success.
 * @return HAL_ERROR on failure.
 */
HAL_StatusTypeDef TCA9548A_ReadControl(uint8_t *control);

#ifdef __cplusplus
}
#endif

#endif /* TCA9548A_H */
