################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Drivers/VL53L0X/platform/src/vl53l0x_platform.c \
../Core/Drivers/VL53L0X/platform/src/vl53l0x_platform_log.c 

OBJS += \
./Core/Drivers/VL53L0X/platform/src/vl53l0x_platform.o \
./Core/Drivers/VL53L0X/platform/src/vl53l0x_platform_log.o 

C_DEPS += \
./Core/Drivers/VL53L0X/platform/src/vl53l0x_platform.d \
./Core/Drivers/VL53L0X/platform/src/vl53l0x_platform_log.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Drivers/VL53L0X/platform/src/%.o Core/Drivers/VL53L0X/platform/src/%.su Core/Drivers/VL53L0X/platform/src/%.cyclo: ../Core/Drivers/VL53L0X/platform/src/%.c Core/Drivers/VL53L0X/platform/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Core/Drivers/VL53L0X -I../Core/Drivers/VL53L0X/core/inc -I../Core/Drivers/VL53L0X/platform/inc -I../Core/Drivers/LCD -I../Core/Drivers/MLX90614 -I../Core/Drivers/TCA9548A -I../Core/Drivers/MPU6500 -I../Core/Application/Inc -I../FATFS/Target -I../FATFS/App -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Drivers-2f-VL53L0X-2f-platform-2f-src

clean-Core-2f-Drivers-2f-VL53L0X-2f-platform-2f-src:
	-$(RM) ./Core/Drivers/VL53L0X/platform/src/vl53l0x_platform.cyclo ./Core/Drivers/VL53L0X/platform/src/vl53l0x_platform.d ./Core/Drivers/VL53L0X/platform/src/vl53l0x_platform.o ./Core/Drivers/VL53L0X/platform/src/vl53l0x_platform.su ./Core/Drivers/VL53L0X/platform/src/vl53l0x_platform_log.cyclo ./Core/Drivers/VL53L0X/platform/src/vl53l0x_platform_log.d ./Core/Drivers/VL53L0X/platform/src/vl53l0x_platform_log.o ./Core/Drivers/VL53L0X/platform/src/vl53l0x_platform_log.su

.PHONY: clean-Core-2f-Drivers-2f-VL53L0X-2f-platform-2f-src

