#ifndef APP_TYPES_H
#define APP_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/*=============================
    System State
=============================*/

typedef enum
{
    SYSTEM_INIT = 0,
    SYSTEM_RUNNING,
    SYSTEM_DEGRADED,
    SYSTEM_FAULT

} SystemState_t;

/*=============================
    Peripheral Status
=============================*/

typedef enum
{
    DEVICE_UNKNOWN = 0,
    DEVICE_OK,
    DEVICE_ERROR

} DeviceStatus_t;

/*=============================
    Shared DAQ Data
=============================*/

typedef struct
{
    /* Timestamp */
    uint32_t timestamp_ms;

    /*-------------------------
      Analog Sensors
    -------------------------*/

    uint16_t frontBrakeRaw;
    uint16_t rearBrakeRaw;
    uint16_t tpsRaw;
    uint16_t batteryRaw;

    float frontBrakeBar;
    float rearBrakeBar;
    float throttlePercent;
    float batteryVoltage;

    /*-------------------------
      Wheel Speed
    -------------------------*/

    float wheelSpeedLeftKph;
    float wheelSpeedRightKph;

    uint32_t wheelSpeedLeftHz;
    uint32_t wheelSpeedRightHz;

    /*-------------------------
      Engine
    -------------------------*/

    uint32_t engineRPM;

    /*-------------------------
      Ride Height
    -------------------------*/

    float rideHeightMm;

    /*-------------------------
      IMU
    -------------------------*/

    float accelLongitudinal;
    float accelLateral;
    float accelVertical;

    float rollRate;
    float pitchRate;
    float yawRate;

    float rollAngle;
    float pitchAngle;
    float yawAngle;

    float imuTemperature;
    /*-------------------------
      Tire Temperature
    -------------------------*/

    float tireTempLeft;
    float tireTempRight;

} DAQ_Data_t;

#ifdef __cplusplus
}
#endif

#endif /* APP_TYPES_H */
