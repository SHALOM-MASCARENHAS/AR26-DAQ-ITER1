################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Drivers/MPU6500/mpu6500.c 

OBJS += \
./Core/Drivers/MPU6500/mpu6500.o 

C_DEPS += \
./Core/Drivers/MPU6500/mpu6500.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Drivers/MPU6500/%.o Core/Drivers/MPU6500/%.su Core/Drivers/MPU6500/%.cyclo: ../Core/Drivers/MPU6500/%.c Core/Drivers/MPU6500/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Core/Drivers/MPU6500 -I../Core/Application/Inc -I../FATFS/Target -I../FATFS/App -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Drivers-2f-MPU6500

clean-Core-2f-Drivers-2f-MPU6500:
	-$(RM) ./Core/Drivers/MPU6500/mpu6500.cyclo ./Core/Drivers/MPU6500/mpu6500.d ./Core/Drivers/MPU6500/mpu6500.o ./Core/Drivers/MPU6500/mpu6500.su

.PHONY: clean-Core-2f-Drivers-2f-MPU6500

