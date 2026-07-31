#ifndef HEALTH_H
#define HEALTH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "app_types.h"

/*=============================
    Device IDs
=============================*/

typedef enum
{
    DEVICE_ADC = 0,
    DEVICE_SD_CARD,
    DEVICE_LCD,
    DEVICE_IMU,
    DEVICE_RTC,
    DEVICE_VL53L0X,
    DEVICE_MLX90614_LEFT,
    DEVICE_MLX90614_RIGHT,
    DEVICE_I2C_MUX,
    DEVICE_ECU_UART,
	DEVICE_WHEEL_LEFT,
	DEVICE_WHEEL_RIGHT,
	DEVICE_ENGINE_RPM,

    DEVICE_COUNT

} DeviceId_t;

/*=============================
    API
=============================*/

void Health_Init(void);
void Health_Task(void);

void Health_SetDeviceStatus(DeviceId_t device,
                            DeviceStatus_t status);

DeviceStatus_t Health_GetDeviceStatus(DeviceId_t device);

SystemState_t Health_GetSystemState(void);

#ifdef __cplusplus
}
#endif

#endif
