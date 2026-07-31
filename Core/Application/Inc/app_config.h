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
#define DISPLAY_TASK_PERIOD_MS         500U
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
#define ADC_MAX_COUNTS           4095.0f
/*=============================
    Logger
=============================*/
/*=============================
    Wheel Speed
=============================*/

#define WHEEL_TOOTH_COUNT            10U
#define WHEEL_DIAMETER_M             0.534f
#define WHEEL_SIGNAL_TIMEOUT_MS    200U
#define WHEEL_CIRCUMFERENCE_M \
    (3.14159265f * WHEEL_DIAMETER_M)
#define MAX_VEHICLE_SPEED_KPH      200.0f
#define MAX_WHEEL_HZ               331.0f
#define MAX_ENGINE_RPM             15000U
/*=============================
    Engine RPM
=============================*/

#define ENGINE_PULSES_PER_REV        1U
/*=============================
    VL53L0X
=============================*/

#define VL53L0X_I2C_ADDRESS             0x52U
#define VL53L0X_TIMING_BUDGET_US    33000U
#define VL53L0X_SIGMA_LIMIT_MM       60.0f
#define VL53L0X_SIGNAL_RATE_LIMIT     0.1f
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
/*=============================
    Display
=============================*/

#define DISPLAY_BOOT_HOLD_MS        2000U
#define DISPLAY_TYPING_DELAY_MS      120U
#define DISPLAY_PAGE_PERIOD_MS      2000U

#ifdef __cplusplus
}
#endif

#endif
