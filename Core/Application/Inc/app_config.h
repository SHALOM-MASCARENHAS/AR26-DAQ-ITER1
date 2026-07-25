#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*=============================
    Task Periods (ms)
=============================*/

#define ACQUISITION_TASK_PERIOD_MS      10U
#define LOGGER_TASK_PERIOD_MS           10U
#define DISPLAY_TASK_PERIOD_MS         200U
#define HEALTH_TASK_PERIOD_MS          500U
#define INNOVATION_TASK_PERIOD_MS       20U

/*=============================
    ADC
=============================*/

#define ADC_CHANNEL_COUNT               4U
#define ADC_MAX_VALUE               4095.0f
#define ADC_REFERENCE_VOLTAGE         3.3f
#define BRAKE_SENSOR_ZERO_VOLTAGE      0.5f
#define BRAKE_SENSOR_FULL_SCALE_BAR  100.0f
#define BRAKE_SENSOR_SPAN_VOLTAGE      4.0f
/*=============================
    Logger
=============================*/

#define LOGGER_BUFFER_SIZE           4096U

/*=============================
    SD Card
=============================*/

#define SD_RETRY_PERIOD_MS           500U

/*=============================
    Battery Divider
    Update after hardware calibration
=============================*/

#define VBAT_DIVIDER_RATIO           4.03f

#ifdef __cplusplus
}
#endif

#endif
