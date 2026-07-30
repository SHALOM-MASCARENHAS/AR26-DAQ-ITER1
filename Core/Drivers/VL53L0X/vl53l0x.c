#include "vl53l0x.h"

#include "vl53l0x_api.h"

static VL53L0X_Dev_t vl53l0x_dev;
static VL53L0X_DEV Dev = &vl53l0x_dev;

VL53L0X_Status_t VL53L0X_Init(I2C_HandleTypeDef *hi2c)
{
    VL53L0X_Error status;

    uint32_t refSpadCount;
    uint8_t isApertureSpads;
    uint8_t VhvSettings;
    uint8_t PhaseCal;

    if (hi2c == NULL)
    {
        return VL53L0X_ERROR;
    }

    Dev->I2cHandle = hi2c;
    Dev->I2cDevAddr = 0x52;

    status = VL53L0X_DataInit(Dev);
    if (status != VL53L0X_ERROR_NONE)
    {
        return VL53L0X_ERROR;
    }

    status = VL53L0X_StaticInit(Dev);
    if (status != VL53L0X_ERROR_NONE)
    {
        return VL53L0X_ERROR;
    }

    status = VL53L0X_PerformRefCalibration(
        Dev,
        &VhvSettings,
        &PhaseCal);
    if (status != VL53L0X_ERROR_NONE)
    {
        return VL53L0X_ERROR;
    }

    status = VL53L0X_PerformRefSpadManagement(
        Dev,
        &refSpadCount,
        &isApertureSpads);
    if (status != VL53L0X_ERROR_NONE)
    {
        return VL53L0X_ERROR;
    }

    status = VL53L0X_SetDeviceMode(
        Dev,
        VL53L0X_DEVICEMODE_SINGLE_RANGING);
    if (status != VL53L0X_ERROR_NONE)
    {
        return VL53L0X_ERROR;
    }

    return VL53L0X_OK;
}

VL53L0X_Status_t VL53L0X_ReadDistance(uint16_t *distance_mm)
{
    VL53L0X_Error status;
    VL53L0X_RangingMeasurementData_t rangingData;

    if (distance_mm == NULL)
    {
        return VL53L0X_ERROR;
    }

    status = VL53L0X_PerformSingleRangingMeasurement(Dev, &rangingData);

    if (status != VL53L0X_ERROR_NONE)
    {
        return VL53L0X_ERROR;
    }

    if (rangingData.RangeStatus != 0)
    {
        return VL53L0X_ERROR;
    }

    *distance_mm = rangingData.RangeMilliMeter;

    return VL53L0X_OK;
}
