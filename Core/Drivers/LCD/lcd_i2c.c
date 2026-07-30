#include "lcd_i2c.h"
#include "i2c.h"
#include "tca9548a.h"

/*----------------------------------------------------------
 * Configuration
 *---------------------------------------------------------*/
#define LCD_TCA_CHANNEL     2U
#define LCD_I2C_ADDRESS     (0x3F << 1)

#define LCD_RS              0x01
#define LCD_RW              0x02
#define LCD_EN              0x04
#define LCD_BL              0x08

#define LCD_CLEAR_CMD       0x01
#define LCD_HOME_CMD        0x02

#define LCD_ENTRY_MODE      0x06
#define LCD_DISPLAY_ON      0x0C
#define LCD_DISPLAY_OFF     0x08
#define LCD_FUNCTION_SET    0x28

#define LCD_SHIFT_LEFT      0x18
#define LCD_SHIFT_RIGHT     0x1C

/*----------------------------------------------------------
 * Private Prototypes
 *---------------------------------------------------------*/

static HAL_StatusTypeDef LCD_Select(void);
static void LCD_Deselect(void);

static HAL_StatusTypeDef LCD_Write(uint8_t value);

static HAL_StatusTypeDef LCD_SendNibble(uint8_t nibble,
                                        uint8_t rs);

static HAL_StatusTypeDef LCD_SendByte(uint8_t data,
                                      uint8_t rs);

static HAL_StatusTypeDef LCD_CommandInternal(uint8_t cmd);

static HAL_StatusTypeDef LCD_DataInternal(uint8_t data);

/*----------------------------------------------------------
 * TCA9548A Helpers
 *---------------------------------------------------------*/

static HAL_StatusTypeDef LCD_Select(void)
{
    return TCA9548A_SelectChannel(LCD_TCA_CHANNEL);
}

static void LCD_Deselect(void)
{
    TCA9548A_DisableAll();
}

/*----------------------------------------------------------
 * Low Level Write
 *---------------------------------------------------------*/

static HAL_StatusTypeDef LCD_Write(uint8_t value)
{
    return HAL_I2C_Master_Transmit(&hi2c1,
                                   LCD_I2C_ADDRESS,
                                   &value,
                                   1,
                                   HAL_MAX_DELAY);
}

/*----------------------------------------------------------
 * Send 4-bit nibble
 *---------------------------------------------------------*/

static HAL_StatusTypeDef LCD_SendNibble(uint8_t nibble,
                                        uint8_t rs)
{
    uint8_t value;

    value = (nibble << 4) | LCD_BL;

    if (rs)
    {
        value |= LCD_RS;
    }

    if (LCD_Write(value) != HAL_OK)
        return HAL_ERROR;

    if (LCD_Write(value | LCD_EN) != HAL_OK)
        return HAL_ERROR;

    if (LCD_Write(value) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

/*----------------------------------------------------------
 * Send complete byte
 *---------------------------------------------------------*/

static HAL_StatusTypeDef LCD_SendByte(uint8_t data,
                                      uint8_t rs)
{
    if (LCD_SendNibble(data >> 4, rs) != HAL_OK)
        return HAL_ERROR;

    if (LCD_SendNibble(data & 0x0F, rs) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

/*----------------------------------------------------------
 * Internal Command
 *---------------------------------------------------------*/

static HAL_StatusTypeDef LCD_CommandInternal(uint8_t cmd)
{
    if (LCD_SendByte(cmd, 0) != HAL_OK)
        return HAL_ERROR;

    if ((cmd == LCD_CLEAR_CMD) ||
        (cmd == LCD_HOME_CMD))
    {
        HAL_Delay(2);
    }

    return HAL_OK;
}

/*----------------------------------------------------------
 * Internal Data
 *---------------------------------------------------------*/

static HAL_StatusTypeDef LCD_DataInternal(uint8_t data)
{
    return LCD_SendByte(data, 1);
}
/*----------------------------------------------------------
 * LCD Initialization
 *---------------------------------------------------------*/

HAL_StatusTypeDef LCD_Init(void)
{
    if (LCD_Select() != HAL_OK)
    {
        return HAL_ERROR;
    }

    HAL_Delay(50);

    /* Reset sequence */
    if (LCD_SendNibble(0x03, 0) != HAL_OK) goto error;
    HAL_Delay(5);

    if (LCD_SendNibble(0x03, 0) != HAL_OK) goto error;
    HAL_Delay(5);

    if (LCD_SendNibble(0x03, 0) != HAL_OK) goto error;
    HAL_Delay(1);

    /* Enter 4-bit mode */
    if (LCD_SendNibble(0x02, 0) != HAL_OK) goto error;
    HAL_Delay(1);

    if (LCD_CommandInternal(LCD_FUNCTION_SET) != HAL_OK)
        goto error;

    if (LCD_CommandInternal(LCD_DISPLAY_OFF) != HAL_OK)
        goto error;

    if (LCD_CommandInternal(LCD_CLEAR_CMD) != HAL_OK)
        goto error;

    if (LCD_CommandInternal(LCD_ENTRY_MODE) != HAL_OK)
        goto error;

    if (LCD_CommandInternal(LCD_DISPLAY_ON) != HAL_OK)
        goto error;

    LCD_Deselect();
    return HAL_OK;

error:
    LCD_Deselect();
    return HAL_ERROR;
}

/*----------------------------------------------------------
 * Clear Display
 *---------------------------------------------------------*/

HAL_StatusTypeDef LCD_Clear(void)
{
    if (LCD_Select() != HAL_OK)
    {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status = LCD_CommandInternal(LCD_CLEAR_CMD);

    LCD_Deselect();

    return status;
}

/*----------------------------------------------------------
 * Cursor Home
 *---------------------------------------------------------*/

HAL_StatusTypeDef LCD_Home(void)
{
    if (LCD_Select() != HAL_OK)
    {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status = LCD_CommandInternal(LCD_HOME_CMD);

    LCD_Deselect();

    return status;
}

/*----------------------------------------------------------
 * Set Cursor Position
 *---------------------------------------------------------*/

HAL_StatusTypeDef LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t address;

    if (row > 1U)
    {
        return HAL_ERROR;
    }

    if (col > 15U)
    {
        return HAL_ERROR;
    }

    address = (row == 0U) ? col : (0x40U + col);

    if (LCD_Select() != HAL_OK)
    {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status = LCD_CommandInternal(0x80U | address);

    LCD_Deselect();

    return status;
}
/*----------------------------------------------------------
 * Print String
 *---------------------------------------------------------*/

HAL_StatusTypeDef LCD_Print(const char *str)
{
    if (str == NULL)
    {
        return HAL_ERROR;
    }

    if (LCD_Select() != HAL_OK)
    {
        return HAL_ERROR;
    }

    while (*str != '\0')
    {
        if (LCD_DataInternal((uint8_t)*str++) != HAL_OK)
        {
            LCD_Deselect();
            return HAL_ERROR;
        }
    }

    LCD_Deselect();
    return HAL_OK;
}

/*----------------------------------------------------------
 * Print at Position
 *---------------------------------------------------------*/

HAL_StatusTypeDef LCD_PrintAt(uint8_t row,
                              uint8_t col,
                              const char *str)
{
    uint8_t address;

    if (str == NULL)
    {
        return HAL_ERROR;
    }

    if (row > 1U)
    {
        return HAL_ERROR;
    }

    if (col > 15U)
    {
        return HAL_ERROR;
    }

    address = (row == 0U) ? col : (0x40U + col);

    if (LCD_Select() != HAL_OK)
    {
        return HAL_ERROR;
    }

    if (LCD_CommandInternal(0x80U | address) != HAL_OK)
    {
        LCD_Deselect();
        return HAL_ERROR;
    }

    while ((*str != '\0') && (col < 16U))
    {
        if (LCD_DataInternal((uint8_t)*str++) != HAL_OK)
        {
            LCD_Deselect();
            return HAL_ERROR;
        }

        col++;
    }

    LCD_Deselect();

    return HAL_OK;
}

/*----------------------------------------------------------
 * Scroll Left
 *---------------------------------------------------------*/

HAL_StatusTypeDef LCD_ScrollLeft(void)
{
    if (LCD_Select() != HAL_OK)
    {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status =
        LCD_CommandInternal(LCD_SHIFT_LEFT);

    LCD_Deselect();

    return status;
}

/*----------------------------------------------------------
 * Scroll Right
 *---------------------------------------------------------*/

HAL_StatusTypeDef LCD_ScrollRight(void)
{
    if (LCD_Select() != HAL_OK)
    {
        return HAL_ERROR;
    }

    HAL_StatusTypeDef status =
        LCD_CommandInternal(LCD_SHIFT_RIGHT);

    LCD_Deselect();

    return status;
}
