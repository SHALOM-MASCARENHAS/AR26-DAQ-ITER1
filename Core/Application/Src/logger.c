#include "logger.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "app_config.h"
#include "shared_data.h"
#include "health.h"

#include "fatfs.h"
#include "rtc.h"
#include "gpio.h"
/* Private Defines ----------------------------------------------------------*/

#define LOGGER_SYNC_PERIOD_MS      1000U

#define LOGGER_FILENAME_LENGTH       32U
#define LOGGER_LINE_BUFFER_SIZE     256U
/* Private Types ------------------------------------------------------------*/

typedef struct
{
    bool sdMounted;
    bool fileOpen;

    uint32_t loggerStartTick;
    uint32_t lastRetryTick;
    uint32_t lastSyncTick;

    uint32_t writeIndex;

    char filename[LOGGER_FILENAME_LENGTH];

    char lineBuffer[LOGGER_LINE_BUFFER_SIZE];
    char writeBuffer[LOGGER_BUFFER_SIZE];

} LoggerContext_t;
/* Private Variables --------------------------------------------------------*/

static LoggerContext_t logger;
/* Private Function Prototypes ----------------------------------------------*/

static bool Logger_Service(void);

static bool Logger_Mount(void);
static bool Logger_OpenFile(void);
static void Logger_Close(void);

static bool Logger_GetRTC(RTC_DateTypeDef *date,
                          RTC_TimeTypeDef *time);
static bool Logger_FileExists(const char *filename);
static void Logger_CreateFilename(void);

static bool Logger_WriteHeader(void);
static bool Logger_WriteRecord(void);
static bool Logger_Flush(void);

static void Logger_UpdateStatusLED(void);
static void Logger_UpdateSafeRemoveLED(void);
void Logger_Init(void)
{
    memset(&logger, 0, sizeof(logger));

    logger.lastRetryTick = HAL_GetTick() - SD_RETRY_PERIOD_MS;
    logger.lastSyncTick = HAL_GetTick();

    HAL_GPIO_WritePin(SD_DEBUG_GPIO_Port,
                      SD_DEBUG_Pin,
                      GPIO_PIN_RESET);

    HAL_GPIO_WritePin(EXT_LED1_GPIO_Port,
                      EXT_LED1_Pin,
                      GPIO_PIN_RESET);

    HAL_GPIO_WritePin(EXT_LED2_GPIO_Port,
                      EXT_LED2_Pin,
                      GPIO_PIN_RESET);
}

void Logger_Task(void)
{
    (void)Logger_Service();
}
static bool Logger_GetRTC(RTC_DateTypeDef *date,
                          RTC_TimeTypeDef *time)
{
    if (HAL_RTC_GetTime(&hrtc, time, RTC_FORMAT_BIN) != HAL_OK)
    {
        return false;
    }

    if (HAL_RTC_GetDate(&hrtc, date, RTC_FORMAT_BIN) != HAL_OK)
    {
        return false;
    }

    return true;
}
static bool Logger_FileExists(const char *filename)
{
    FILINFO fileInfo;

    return (f_stat(filename, &fileInfo) == FR_OK);
}
static void Logger_CreateFilename(void)
{
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;

    uint32_t index = 1U;

    bool rtcValid =
        Logger_GetRTC(&date, &time) &&
        (date.Month >= 1U) &&
        (date.Month <= 12U) &&
        (date.Date >= 1U) &&
        (date.Date <= 31U);

    if (rtcValid)
    {
        do
        {
            snprintf(logger.filename,
                     sizeof(logger.filename),
                     "%02u%02u%02u_%02u%02u%02u_DAQ%lu.CSV",
                     date.Year,
                     date.Month,
                     date.Date,
                     time.Hours,
                     time.Minutes,
                     time.Seconds,
                     (unsigned long)index);

            index++;

        } while (Logger_FileExists(logger.filename));
    }
    else
    {
        do
        {
            snprintf(logger.filename,
                     sizeof(logger.filename),
                     "DAQ%lu.CSV",
                     (unsigned long)index);

            index++;

        } while (Logger_FileExists(logger.filename));
    }
}
static bool Logger_Mount(void)
{
    if (logger.sdMounted)
    {
        return true;
    }

    if (f_mount(&SDFatFS, SDPath, 1) != FR_OK)
    {
        Health_SetDeviceStatus(DEVICE_SD_CARD, DEVICE_ERROR);
        return false;
    }

    logger.sdMounted = true;

    Health_SetDeviceStatus(DEVICE_SD_CARD, DEVICE_OK);

    return true;
}
static bool Logger_OpenFile(void)
{
    if (logger.fileOpen)
    {
        return true;
    }

    Logger_CreateFilename();

    if (f_open(&SDFile,
               logger.filename,
			   FA_CREATE_NEW | FA_WRITE) != FR_OK)
    {
        Health_SetDeviceStatus(DEVICE_SD_CARD, DEVICE_ERROR);
        return false;
    }

    logger.fileOpen = true;
    logger.loggerStartTick = HAL_GetTick();
    logger.writeIndex = 0U;

    if (!Logger_WriteHeader())
    {
        (void)f_close(&SDFile);
        (void)f_mount(NULL, SDPath, 1);

        logger.fileOpen = false;
        logger.sdMounted = false;

        return false;
    }

    Health_SetDeviceStatus(DEVICE_SD_CARD, DEVICE_OK);

    return true;
}
static void Logger_Close(void)
{
    if (!logger.fileOpen)
    {
        return;
    }

    if (logger.writeIndex > 0U)
    {
        UINT bytesWritten;

        (void)f_write(&SDFile,
                      logger.writeBuffer,
                      logger.writeIndex,
                      &bytesWritten);

        logger.writeIndex = 0U;
    }

    (void)f_sync(&SDFile);
    (void)f_close(&SDFile);
    (void)f_mount(NULL, SDPath, 1);

    logger.fileOpen = false;
    logger.sdMounted = false;
}
static bool Logger_WriteHeader(void)
{
    UINT bytesWritten;

    const char header[] =
        "Time_ms,"
        "FrontBrake_bar,"
        "RearBrake_bar,"
        "Throttle_percent,"
        "Battery_V,"
        "WheelLeft_kph,"
        "WheelRight_kph,"
        "EngineRPM,"
    	"RideHeight_mm,"
    	"LongAccel_G,"
    	"LatAccel_G,"
    	"VertAccel_G,"
    	"RollRate_dps,"
    	"PitchRate_dps,"
    	"YawRate_dps,"
    	"Roll_deg,"
    	"Pitch_deg,"
    	"Yaw_deg,"
    	"IMUTemp_C,"
    	"TireTempLeft_C,"
    	"TireTempRight_C\r\n";

    UINT headerLength = (UINT)strlen(header);

    if ((f_write(&SDFile,
                 header,
                 headerLength,
                 &bytesWritten) != FR_OK) ||
        (bytesWritten != headerLength))
    {
        return false;
    }
    (void)f_sync(&SDFile);
    return true;
}
static bool Logger_WriteRecord(void)
{
    int length;

    length = snprintf(
        logger.lineBuffer,
        sizeof(logger.lineBuffer),

        "%lu,"
        "%.2f,"
        "%.2f,"
        "%.2f,"
        "%.2f,"
        "%.2f,"
        "%.2f,"
        "%lu,"
        "%.2f,"
		"%.3f,"
		"%.3f,"
		"%.3f,"
		"%.3f,"
		"%.3f,"
		"%.3f,"
		"%.2f,"
		"%.2f,"
		"%.2f,"
		"%.2f,"
		"%.2f,"
		"%.2f\r\n",

        (unsigned long)(g_daqData.timestamp_ms - logger.loggerStartTick),

        g_daqData.frontBrakeBar,
        g_daqData.rearBrakeBar,
        g_daqData.throttlePercent,
        g_daqData.batteryVoltage,

        g_daqData.wheelSpeedLeftKph,
        g_daqData.wheelSpeedRightKph,

        (unsigned long)g_daqData.engineRPM,

        g_daqData.rideHeightMm,

		g_daqData.accelLongitudinal,
		g_daqData.accelLateral,
		g_daqData.accelVertical,

		g_daqData.rollRate,
		g_daqData.pitchRate,
		g_daqData.yawRate,

		g_daqData.rollAngle,
		g_daqData.pitchAngle,
		g_daqData.yawAngle,

		g_daqData.imuTemperature,

        g_daqData.tireTempLeft,
        g_daqData.tireTempRight);

    if ((length < 0) || ((size_t)length >= sizeof(logger.lineBuffer)))
    {
        return false;
    }
    if ((logger.writeIndex + (uint32_t)length) >= LOGGER_BUFFER_SIZE)
    {
        if (!Logger_Flush())
        {
            return false;
        }
    }

    memcpy(&logger.writeBuffer[logger.writeIndex],
           logger.lineBuffer,
           (size_t)length);

    logger.writeIndex += (uint32_t)length;

    return true;
}
static bool Logger_Flush(void)
{
    UINT bytesWritten;

    if (!logger.fileOpen)
    {
        return false;
    }

    if (logger.writeIndex == 0U)
    {
        return true;
    }

    if (f_write(&SDFile,
                logger.writeBuffer,
                logger.writeIndex,
                &bytesWritten) != FR_OK)
    {
        (void)f_close(&SDFile);
        (void)f_mount(NULL, SDPath, 1);

        logger.fileOpen = false;
        logger.sdMounted = false;
        logger.writeIndex = 0U;

        Health_SetDeviceStatus(DEVICE_SD_CARD,
                               DEVICE_ERROR);

        return false;
    }
    if (bytesWritten != logger.writeIndex)
    {
        (void)f_close(&SDFile);
        (void)f_mount(NULL, SDPath, 1);

        logger.fileOpen = false;
        logger.sdMounted = false;
        logger.writeIndex = 0U;

        Health_SetDeviceStatus(DEVICE_SD_CARD,
                               DEVICE_ERROR);

        return false;
    }
    logger.writeIndex = 0U;

    if ((HAL_GetTick() - logger.lastSyncTick) >= LOGGER_SYNC_PERIOD_MS)
    {
        f_sync(&SDFile);
        logger.lastSyncTick = HAL_GetTick();
    }

    return true;
}
static void Logger_UpdateStatusLED(void)
{
    static uint32_t lastToggle = 0U;
    static GPIO_PinState ledState = GPIO_PIN_RESET;

    if (!logger.fileOpen)
    {
        HAL_GPIO_WritePin(SD_DEBUG_GPIO_Port,
                          SD_DEBUG_Pin,
                          GPIO_PIN_RESET);
        return;
    }

    if ((HAL_GetTick() - lastToggle) >= 500U)
    {
        lastToggle = HAL_GetTick();

        ledState = (ledState == GPIO_PIN_SET) ?
                   GPIO_PIN_RESET :
                   GPIO_PIN_SET;

        HAL_GPIO_WritePin(SD_DEBUG_GPIO_Port,
                          SD_DEBUG_Pin,
                          ledState);
    }
}
static bool Logger_Service(void)
{
    if (!logger.sdMounted)
    {
        if ((HAL_GetTick() - logger.lastRetryTick) >= SD_RETRY_PERIOD_MS)
        {
            logger.lastRetryTick = HAL_GetTick();

            (void)Logger_Mount();

            if (logger.sdMounted)
            {
                if (!Logger_OpenFile())
                {
                    Logger_UpdateStatusLED();
                    Logger_UpdateSafeRemoveLED();
                    return false;
                }
            }
        }

        Logger_UpdateStatusLED();
        Logger_UpdateSafeRemoveLED();

        return false;
    }

    if (!logger.fileOpen)
    {
        if (!Logger_OpenFile())
        {
            Logger_UpdateStatusLED();
            Logger_UpdateSafeRemoveLED();
            return false;
        }
    }

    if (!Logger_WriteRecord())
    {
        Logger_Close();
        Health_SetDeviceStatus(DEVICE_SD_CARD,
                               DEVICE_ERROR);

        return false;
    }

    if (!Logger_Flush())
    {
        return false;
    }

    Logger_UpdateStatusLED();
    Logger_UpdateSafeRemoveLED();

    return true;
}
static void Logger_UpdateSafeRemoveLED(void)
{
    static uint32_t safeStartTick = 0U;
    static bool safeState = false;

    bool stationary =
        (g_daqData.engineRPM < 50U) &&
        (g_daqData.wheelSpeedLeftKph < 1.0f) &&
        (g_daqData.wheelSpeedRightKph < 1.0f);

    if (!logger.fileOpen)
    {
        HAL_GPIO_WritePin(EXT_LED1_GPIO_Port,
                          EXT_LED1_Pin,
                          GPIO_PIN_RESET);

        HAL_GPIO_WritePin(EXT_LED2_GPIO_Port,
                          EXT_LED2_Pin,
                          GPIO_PIN_RESET);

        safeStartTick = 0U;
        safeState = false;
        return;
    }

    if (stationary)
    {
        if (!safeState)
        {
            if (safeStartTick == 0U)
            {
                safeStartTick = HAL_GetTick();
            }

            if ((HAL_GetTick() - safeStartTick) >= 2000U)
            {
                safeState = true;
            }
        }
    }
    else
    {
        safeStartTick = 0U;
        safeState = false;
    }

    HAL_GPIO_WritePin(EXT_LED1_GPIO_Port,
                      EXT_LED1_Pin,
                      safeState ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(EXT_LED2_GPIO_Port,
                      EXT_LED2_Pin,
                      safeState ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
