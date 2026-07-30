#ifndef LCD_I2C_H
#define LCD_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

/* Public API */
HAL_StatusTypeDef LCD_Init(void);
HAL_StatusTypeDef LCD_Clear(void);
HAL_StatusTypeDef LCD_Home(void);

HAL_StatusTypeDef LCD_SetCursor(uint8_t row, uint8_t col);
HAL_StatusTypeDef LCD_Print(const char *str);
HAL_StatusTypeDef LCD_PrintAt(uint8_t row,
                              uint8_t col,
                              const char *str);

HAL_StatusTypeDef LCD_ScrollLeft(void);
HAL_StatusTypeDef LCD_ScrollRight(void);

#ifdef __cplusplus
}
#endif

#endif
