#include "health.h"

static DeviceStatus_t deviceStatus[DEVICE_COUNT];
static SystemState_t systemState;

void Health_Init(void)
{
    for(uint32_t i = 0; i < DEVICE_COUNT; i++)
    {
        deviceStatus[i] = DEVICE_UNKNOWN;
    }

    systemState = SYSTEM_INIT;
}

void Health_SetDeviceStatus(DeviceId_t device,
                            DeviceStatus_t status)
{
    if(device >= DEVICE_COUNT)
    {
        return;
    }

    deviceStatus[device] = status;

    systemState = SYSTEM_RUNNING;

    for(uint32_t i = 0; i < DEVICE_COUNT; i++)
    {
        if(deviceStatus[i] == DEVICE_ERROR)
        {
            systemState = SYSTEM_DEGRADED;
            return;
        }
    }
}

DeviceStatus_t Health_GetDeviceStatus(DeviceId_t device)
{
    if(device >= DEVICE_COUNT)
    {
        return DEVICE_UNKNOWN;
    }

    return deviceStatus[device];
}

SystemState_t Health_GetSystemState(void)
{
    return systemState;
}
void Health_Task(void)
{
    /* Reserved for future health monitoring */
}
