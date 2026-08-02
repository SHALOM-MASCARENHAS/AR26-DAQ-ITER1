################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Application/Src/acquisition.c \
../Core/Application/Src/display.c \
../Core/Application/Src/health.c \
../Core/Application/Src/i2c_manager.c \
../Core/Application/Src/innovation.c \
../Core/Application/Src/logger.c \
../Core/Application/Src/shared_data.c 

OBJS += \
./Core/Application/Src/acquisition.o \
./Core/Application/Src/display.o \
./Core/Application/Src/health.o \
./Core/Application/Src/i2c_manager.o \
./Core/Application/Src/innovation.o \
./Core/Application/Src/logger.o \
./Core/Application/Src/shared_data.o 

C_DEPS += \
./Core/Application/Src/acquisition.d \
./Core/Application/Src/display.d \
./Core/Application/Src/health.d \
./Core/Application/Src/i2c_manager.d \
./Core/Application/Src/innovation.d \
./Core/Application/Src/logger.d \
./Core/Application/Src/shared_data.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Application/Src/%.o Core/Application/Src/%.su Core/Application/Src/%.cyclo: ../Core/Application/Src/%.c Core/Application/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Core/Drivers/VL53L0X -I../Core/Drivers/VL53L0X/core/inc -I../Core/Drivers/VL53L0X/platform/inc -I../Core/Drivers/LCD -I../Core/Drivers/MLX90614 -I../Core/Drivers/TCA9548A -I../Core/Drivers/MPU6500 -I../Core/Application/Inc -I../FATFS/Target -I../FATFS/App -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Application-2f-Src

clean-Core-2f-Application-2f-Src:
	-$(RM) ./Core/Application/Src/acquisition.cyclo ./Core/Application/Src/acquisition.d ./Core/Application/Src/acquisition.o ./Core/Application/Src/acquisition.su ./Core/Application/Src/display.cyclo ./Core/Application/Src/display.d ./Core/Application/Src/display.o ./Core/Application/Src/display.su ./Core/Application/Src/health.cyclo ./Core/Application/Src/health.d ./Core/Application/Src/health.o ./Core/Application/Src/health.su ./Core/Application/Src/i2c_manager.cyclo ./Core/Application/Src/i2c_manager.d ./Core/Application/Src/i2c_manager.o ./Core/Application/Src/i2c_manager.su ./Core/Application/Src/innovation.cyclo ./Core/Application/Src/innovation.d ./Core/Application/Src/innovation.o ./Core/Application/Src/innovation.su ./Core/Application/Src/logger.cyclo ./Core/Application/Src/logger.d ./Core/Application/Src/logger.o ./Core/Application/Src/logger.su ./Core/Application/Src/shared_data.cyclo ./Core/Application/Src/shared_data.d ./Core/Application/Src/shared_data.o ./Core/Application/Src/shared_data.su

.PHONY: clean-Core-2f-Application-2f-Src

