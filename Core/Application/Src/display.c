#include "display.h"
#include "lcd_i2c.h"
#include "shared_data.h"
#include "app_config.h"

#include <stdio.h>
#include <string.h>

typedef enum
{
    DISPLAY_BOOT_ARION = 0,
    DISPLAY_BOOT_READY,
    DISPLAY_BRAKE_BIAS
} DisplayState_t;

static DisplayState_t displayState;

static uint32_t lastUpdate;
static uint32_t stateStart;

static uint8_t textPos;

static const char arionText[] = "TEAM ARION";
static const char readyText[] = "READY TO RACE";

void Display_Init(void)
{
    if (LCD_Init() != HAL_OK)
    {
        return;
    }

    LCD_Clear();

    displayState = DISPLAY_BOOT_ARION;

    lastUpdate = HAL_GetTick();
    stateStart = HAL_GetTick();

    textPos = 0;
}

void Display_Task(void)
{
    char buffer[17];
    char line[17];
    float total;
    float frontBias;
    float rearBias;

    if ((HAL_GetTick() - lastUpdate) < DISPLAY_TASK_PERIOD_MS)
    {
        return;
    }

    lastUpdate = HAL_GetTick();

    switch (displayState)
    {
        case DISPLAY_BOOT_ARION:

            if (textPos == 0U)
            {
                LCD_Clear();
            }

            if (textPos < sizeof(arionText) - 1U)
            {
                memset(buffer, 0, sizeof(buffer));

                memcpy(buffer, arionText, textPos + 1U);

                LCD_PrintAt(0, 0, buffer);

                textPos++;
            }
            else if ((HAL_GetTick() - stateStart) >= DISPLAY_BOOT_HOLD_MS)
            {
                LCD_Clear();

                displayState = DISPLAY_BOOT_READY;

                stateStart = HAL_GetTick();

                textPos = 0U;
            }

            break;

        case DISPLAY_BOOT_READY:

            if (textPos == 0U)
            {
                LCD_Clear();
            }

            if (textPos < sizeof(readyText) - 1U)
            {
                memset(buffer, 0, sizeof(buffer));

                memcpy(buffer, readyText, textPos + 1U);

                LCD_PrintAt(0, 0, buffer);

                textPos++;
            }
            else if ((HAL_GetTick() - stateStart) >= DISPLAY_BOOT_HOLD_MS)
            {
                LCD_Clear();

                displayState = DISPLAY_BRAKE_BIAS;
            }

            break;

        case DISPLAY_BRAKE_BIAS:

            total = g_daqData.frontBrakeBar +
                    g_daqData.rearBrakeBar;

            if (total > 0.1f)
            {
                frontBias = (g_daqData.frontBrakeBar / total) * 100.0f;
                rearBias = 100.0f - frontBias;
            }
            else
            {
                frontBias = 50.0f;
                rearBias = 50.0f;
            }

            LCD_PrintAt(0, 0, " Brake Bias   ");

            snprintf(line,
                     sizeof(line),
                     "%2.0f%% | %2.0f%% ",
                     frontBias,
                     rearBias);

            LCD_PrintAt(1, 0, line);

            break;

        default:
            break;
    }
}
