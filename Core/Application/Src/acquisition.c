#include "acquisition.h"

#include "adc.h"

#include "app_config.h"
#include "shared_data.h"
#include "health.h"

/*=============================
    ADC Channel Indices
=============================*/

typedef enum
{
    ADC_FRONT_BRAKE = 0,
    ADC_REAR_BRAKE,
    ADC_TPS,
    ADC_BATTERY

} ADC_ChannelIndex_t;

/*=============================
    Private Variables
=============================*/

static uint16_t adcRaw[ADC_CHANNEL_COUNT];
#define ADC_VREF                3.3f
#define ADC_MAX_VALUE           4095.0f


#define TPS_MAX_VOLTAGE        2.85f
static float ADC_To_Voltage(uint16_t raw)
{
    return ((float)raw * ADC_VREF) / ADC_MAX_VALUE;
}

static float ADC_To_BrakePressureBar(uint16_t raw)
{
    float voltage;
    float pressureBar;

    voltage = ADC_To_Voltage(raw);

    pressureBar =
        ((voltage - BRAKE_SENSOR_ZERO_VOLTAGE)
        * BRAKE_SENSOR_FULL_SCALE_BAR)
        / BRAKE_SENSOR_SPAN_VOLTAGE;

    if (pressureBar < 0.0f)
    {
        pressureBar = 0.0f;
    }
    else if (pressureBar > BRAKE_SENSOR_FULL_SCALE_BAR)
    {
        pressureBar = BRAKE_SENSOR_FULL_SCALE_BAR;
    }

    return pressureBar;
}
static float ADC_To_ThrottlePercent(uint16_t raw)
{
    float throttlePercent;

    throttlePercent = (ADC_To_Voltage(raw) / TPS_MAX_VOLTAGE) * 100.0f;

    if (throttlePercent < 0.0f)
    {
        throttlePercent = 0.0f;
    }
    else if (throttlePercent > 100.0f)
    {
        throttlePercent = 100.0f;
    }

    return throttlePercent;
}

static float ADC_To_BatteryVoltage(uint16_t raw)
{
    /* After your PCB modification, this measures the 3.3 V rail */
    return ADC_To_Voltage(raw);
}
/*=============================
    Public Functions
=============================*/

void Acquisition_Init(void)
{
    if (HAL_ADC_Start_DMA(&hadc1,
                          (uint32_t *)adcRaw,
                          ADC_CHANNEL_COUNT) == HAL_OK)
    {
        Health_SetDeviceStatus(DEVICE_ADC, DEVICE_OK);
    }
    else
    {
        Health_SetDeviceStatus(DEVICE_ADC, DEVICE_ERROR);
    }
}

void Acquisition_Task(void)
{
    g_daqData.timestamp_ms = HAL_GetTick();

    /* Raw ADC values */
    g_daqData.frontBrakeRaw = adcRaw[ADC_FRONT_BRAKE];
    g_daqData.rearBrakeRaw  = adcRaw[ADC_REAR_BRAKE];
    g_daqData.tpsRaw        = adcRaw[ADC_TPS];
    g_daqData.batteryRaw    = adcRaw[ADC_BATTERY];

    /* Engineering units */
    g_daqData.frontBrakeBar = ADC_To_BrakePressureBar(g_daqData.frontBrakeRaw);
    g_daqData.rearBrakeBar  = ADC_To_BrakePressureBar(g_daqData.rearBrakeRaw);
    g_daqData.throttlePercent = ADC_To_ThrottlePercent(g_daqData.tpsRaw);
    g_daqData.batteryVoltage  = ADC_To_BatteryVoltage(g_daqData.batteryRaw);
}
