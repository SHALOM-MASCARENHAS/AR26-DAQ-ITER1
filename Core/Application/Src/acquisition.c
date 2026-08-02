#include "acquisition.h"
#include <math.h>
#include "adc.h"
#include "tim.h"
#include "mpu6500.h"
#include "app_config.h"
#include "shared_data.h"
#include "health.h"
#include "tca9548a.h"
#include "mlx90614.h"
#include "vl53l0x.h"
#include "i2c.h"
#include "FreeRTOS.h"
#include "task.h"

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
/*=============================
    Wheel Speed
=============================*/

#define TIM2_CLOCK_HZ    96000000UL

static volatile uint32_t leftPreviousCapture  = 0U;
static volatile uint32_t rightPreviousCapture = 0U;
static volatile uint32_t rpmPreviousCapture   = 0U;

static volatile uint32_t leftPeriodTicks  = 0U;
static volatile uint32_t rightPeriodTicks = 0U;
static volatile uint32_t rpmPeriodTicks   = 0U;

static volatile uint32_t leftLastEdgeTick  = 0U;
static volatile uint32_t rightLastEdgeTick = 0U;
static volatile uint32_t rpmLastEdgeTick   = 0U;

#define TPS_MAX_VOLTAGE        2.85f
static float ADC_To_Voltage(uint16_t raw)
{
	return ((float)raw * ADC_REFERENCE_VOLTAGE) / ADC_MAX_COUNTS;
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
	if (TCA9548A_Init() == HAL_OK)
	{
	    Health_SetDeviceStatus(DEVICE_I2C_MUX, DEVICE_OK);


	    if (TCA9548A_SelectChannel(0) == HAL_OK)
	    {
	        if (MLX90614_Init() == HAL_OK)
	        {
	            Health_SetDeviceStatus(DEVICE_MLX90614_LEFT, DEVICE_OK);
	        }
	        else
	        {
	            Health_SetDeviceStatus(DEVICE_MLX90614_LEFT, DEVICE_ERROR);
	        }

	        TCA9548A_DisableAll();
	    }
	    else
	    {
	        Health_SetDeviceStatus(DEVICE_MLX90614_LEFT, DEVICE_ERROR);
	    }


	    if (TCA9548A_SelectChannel(1) == HAL_OK)
	    {
	        if (MLX90614_Init() == HAL_OK)
	        {
	            Health_SetDeviceStatus(DEVICE_MLX90614_RIGHT, DEVICE_OK);
	        }
	        else
	        {
	            Health_SetDeviceStatus(DEVICE_MLX90614_RIGHT, DEVICE_ERROR);
	        }

	        TCA9548A_DisableAll();
	    }
	    else
	    {
	        Health_SetDeviceStatus(DEVICE_MLX90614_RIGHT, DEVICE_ERROR);
	    }
	}
	else
	{
	    Health_SetDeviceStatus(DEVICE_I2C_MUX, DEVICE_ERROR);

	    Health_SetDeviceStatus(DEVICE_MLX90614_LEFT, DEVICE_ERROR);
	    Health_SetDeviceStatus(DEVICE_MLX90614_RIGHT, DEVICE_ERROR);
	}

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
    if (HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1) == HAL_OK)
    {
        Health_SetDeviceStatus(DEVICE_WHEEL_LEFT, DEVICE_OK);
    }
    else
    {
        Health_SetDeviceStatus(DEVICE_WHEEL_LEFT, DEVICE_ERROR);
    }

    if (HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2) == HAL_OK)
    {
        Health_SetDeviceStatus(DEVICE_WHEEL_RIGHT, DEVICE_OK);
    }
    else
    {
        Health_SetDeviceStatus(DEVICE_WHEEL_RIGHT, DEVICE_ERROR);
    }
    if (HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3) == HAL_OK)
    {
        Health_SetDeviceStatus(DEVICE_ENGINE_RPM,
                               DEVICE_OK);
    }
    else
    {
        Health_SetDeviceStatus(DEVICE_ENGINE_RPM,
                               DEVICE_ERROR);
    }
   if (MPU6500_Init() == HAL_OK)
    {
        if (MPU6500_CalibrateGyro() == HAL_OK)
        {
            Health_SetDeviceStatus(DEVICE_IMU, DEVICE_OK);
        }
        else
        {
            Health_SetDeviceStatus(DEVICE_IMU, DEVICE_ERROR);
        }
    }
    else
    {
        Health_SetDeviceStatus(DEVICE_IMU, DEVICE_ERROR);
    }

    if (VL53L0X_Init(&hi2c1) == VL53L0X_OK)
    {
        Health_SetDeviceStatus(DEVICE_VL53L0X, DEVICE_OK);
    }
    else
    {
        Health_SetDeviceStatus(DEVICE_VL53L0X, DEVICE_ERROR);
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
    /*=============================
        Wheel Speed / Engine RPM
    =============================*/

    uint32_t leftTicks;
    uint32_t rightTicks;
    uint32_t rpmTicks;

    uint32_t leftEdgeTick;
    uint32_t rightEdgeTick;
    uint32_t rpmEdgeTick;

    float engineHz;

    uint32_t now = HAL_GetTick();

    taskENTER_CRITICAL();

    leftTicks     = leftPeriodTicks;
    rightTicks    = rightPeriodTicks;
    rpmTicks      = rpmPeriodTicks;

    leftEdgeTick  = leftLastEdgeTick;
    rightEdgeTick = rightLastEdgeTick;
    rpmEdgeTick   = rpmLastEdgeTick;

    taskEXIT_CRITICAL();

    /* Left Wheel */

    if ((now - leftEdgeTick) > WHEEL_SIGNAL_TIMEOUT_MS ||
        leftTicks == 0U)
    {
        g_daqData.wheelSpeedLeftHz  = 0.0f;
        g_daqData.wheelSpeedLeftKph = 0.0f;
    }
    else
    {
        g_daqData.wheelSpeedLeftHz =
            (float)TIM2_CLOCK_HZ / (float)leftTicks;

        if (g_daqData.wheelSpeedLeftHz > MAX_WHEEL_HZ)
        {
            g_daqData.wheelSpeedLeftHz  = 0.0f;
            g_daqData.wheelSpeedLeftKph = 0.0f;
        }
        else
        {
            g_daqData.wheelSpeedLeftKph =
                (g_daqData.wheelSpeedLeftHz /
                 WHEEL_TOOTH_COUNT) *
                WHEEL_CIRCUMFERENCE_M *
                3.6f;
        }
    }

    /* Right Wheel */

    if ((now - rightEdgeTick) > WHEEL_SIGNAL_TIMEOUT_MS ||
        rightTicks == 0U)
    {
        g_daqData.wheelSpeedRightHz  = 0.0f;
        g_daqData.wheelSpeedRightKph = 0.0f;
    }
    else
    {
        g_daqData.wheelSpeedRightHz =
            (float)TIM2_CLOCK_HZ / (float)rightTicks;

        if (g_daqData.wheelSpeedRightHz > MAX_WHEEL_HZ)
        {
            g_daqData.wheelSpeedRightHz  = 0.0f;
            g_daqData.wheelSpeedRightKph = 0.0f;
        }
        else
        {
            g_daqData.wheelSpeedRightKph =
                (g_daqData.wheelSpeedRightHz /
                 WHEEL_TOOTH_COUNT) *
                WHEEL_CIRCUMFERENCE_M *
                3.6f;
        }
    }

    /* Engine RPM */

    if ((now - rpmEdgeTick) > WHEEL_SIGNAL_TIMEOUT_MS ||
        rpmTicks == 0U)
    {
        g_daqData.engineRPM = 0U;
    }
    else
    {
        engineHz =
            (float)TIM2_CLOCK_HZ /
            (float)rpmTicks;

        g_daqData.engineRPM =
            (uint32_t)((engineHz * 60.0f) /
                       ENGINE_PULSES_PER_REV);

        if (g_daqData.engineRPM > MAX_ENGINE_RPM)
        {
            g_daqData.engineRPM = 0U;
        }
    }
    /* MPU6500 */
    static float rollAngle = 0.0f;
    static float pitchAngle = 0.0f;
    static float yawAngle = 0.0f;

    #define COMPLEMENTARY_ALPHA 0.98f
    #define DT_SECONDS          0.01f
    float ax;
    float ay;
    float az;

    float gx;
    float gy;
    float gz;

    float temperature;

    if (MPU6500_ReadAll(&ax,
                        &ay,
                        &az,
                        &gx,
                        &gy,
                        &gz,
                        &temperature) == HAL_OK)
    {
        /*
         * IMPORTANT:
         * Change these mappings if your MPU orientation differs.
         * This assumes:
         *
         * Sensor X = Vehicle Forward
         * Sensor Y = Vehicle Left
         * Sensor Z = Vehicle Up
         */

        g_daqData.accelLongitudinal = ax;
        g_daqData.accelLateral      = ay;
        g_daqData.accelVertical     = az;

        g_daqData.rollRate  = gx;
        g_daqData.pitchRate = gy;
        g_daqData.yawRate   = gz;

        g_daqData.imuTemperature = temperature;

        float accelRoll =
            atan2f(ay, az) * 57.29578f;

        float accelPitch =
            atan2f(-ax,
                   sqrtf((ay * ay) + (az * az))) * 57.29578f;

        rollAngle += gx * DT_SECONDS;
        pitchAngle += gy * DT_SECONDS;
        yawAngle += gz * DT_SECONDS;

        rollAngle =
            (COMPLEMENTARY_ALPHA * rollAngle) +
            ((1.0f - COMPLEMENTARY_ALPHA) * accelRoll);

        pitchAngle =
            (COMPLEMENTARY_ALPHA * pitchAngle) +
            ((1.0f - COMPLEMENTARY_ALPHA) * accelPitch);

        g_daqData.rollAngle = rollAngle;
        g_daqData.pitchAngle = pitchAngle;
        g_daqData.yawAngle = yawAngle;

        Health_SetDeviceStatus(DEVICE_IMU,
                               DEVICE_OK);
    }
    else
    {
        g_daqData.accelLongitudinal = NAN;
        g_daqData.accelLateral      = NAN;
        g_daqData.accelVertical     = NAN;

        g_daqData.rollRate = NAN;
        g_daqData.pitchRate = NAN;
        g_daqData.yawRate = NAN;

        g_daqData.rollAngle = NAN;
        g_daqData.pitchAngle = NAN;
        g_daqData.yawAngle = NAN;

        g_daqData.imuTemperature = NAN;

        Health_SetDeviceStatus(DEVICE_IMU,
                               DEVICE_ERROR);
    }
    if (TCA9548A_SelectChannel(0) == HAL_OK)
    {
        if (MLX90614_ReadObjectTemperature(&g_daqData.tireTempLeft) == HAL_OK)
        {
            Health_SetDeviceStatus(DEVICE_MLX90614_LEFT, DEVICE_OK);
        }
        else
        {
            g_daqData.tireTempLeft = NAN;
            Health_SetDeviceStatus(DEVICE_MLX90614_LEFT, DEVICE_ERROR);
        }

        TCA9548A_DisableAll();
    }
    else
    {
        g_daqData.tireTempLeft = NAN;
        Health_SetDeviceStatus(DEVICE_MLX90614_LEFT, DEVICE_ERROR);
    }
    if (TCA9548A_SelectChannel(1) == HAL_OK)
    {
        if (MLX90614_ReadObjectTemperature(&g_daqData.tireTempRight) == HAL_OK)
        {
            Health_SetDeviceStatus(DEVICE_MLX90614_RIGHT, DEVICE_OK);
        }
        else
        {
            g_daqData.tireTempRight = NAN;
            Health_SetDeviceStatus(DEVICE_MLX90614_RIGHT, DEVICE_ERROR);
        }

        TCA9548A_DisableAll();
    }
    else
    {
        g_daqData.tireTempRight = NAN;
        Health_SetDeviceStatus(DEVICE_MLX90614_RIGHT, DEVICE_ERROR);
    }
    uint16_t distance;

    if (VL53L0X_ReadDistance(&distance) == VL53L0X_OK)
    {
        g_daqData.rideHeightMm = (float)distance;
        Health_SetDeviceStatus(DEVICE_VL53L0X, DEVICE_OK);
    }
    else
    {
        g_daqData.rideHeightMm = NAN;
        Health_SetDeviceStatus(DEVICE_VL53L0X, DEVICE_ERROR);
    }
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    uint32_t capture;

    if (htim->Instance != TIM2)
    {
        return;
    }

    switch (htim->Channel)
    {
        case HAL_TIM_ACTIVE_CHANNEL_1:

        	capture = HAL_TIM_ReadCapturedValue(htim,
        	                                    TIM_CHANNEL_1);

        	if (leftPreviousCapture != 0U)
        	{
        	    leftPeriodTicks = capture - leftPreviousCapture;
        	}

        	leftPreviousCapture = capture;
        	leftLastEdgeTick = xTaskGetTickCountFromISR();

            break;

        case HAL_TIM_ACTIVE_CHANNEL_2:

        	capture = HAL_TIM_ReadCapturedValue(htim,
        	                                    TIM_CHANNEL_2);

        	if (rightPreviousCapture != 0U)
        	{
        	    rightPeriodTicks = capture - rightPreviousCapture;
        	}

        	rightPreviousCapture = capture;
        	rightLastEdgeTick = xTaskGetTickCountFromISR();

            break;

        case HAL_TIM_ACTIVE_CHANNEL_3:

        	capture = HAL_TIM_ReadCapturedValue(htim,
        	                                    TIM_CHANNEL_3);

        	if (rpmPreviousCapture != 0U)
        	{
        	    rpmPeriodTicks = capture - rpmPreviousCapture;
        	}

        	rpmPreviousCapture = capture;
        	rpmLastEdgeTick = xTaskGetTickCountFromISR();

            break;

        default:
            break;
    }
}
