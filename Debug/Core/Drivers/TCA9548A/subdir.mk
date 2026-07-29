################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Drivers/TCA9548A/tca9548a.c 

OBJS += \
./Core/Drivers/TCA9548A/tca9548a.o 

C_DEPS += \
./Core/Drivers/TCA9548A/tca9548a.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Drivers/TCA9548A/%.o Core/Drivers/TCA9548A/%.su Core/Drivers/TCA9548A/%.cyclo: ../Core/Drivers/TCA9548A/%.c Core/Drivers/TCA9548A/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Core/Drivers/MLX90614 -I../Core/Drivers/TCA9548A -I../Core/Drivers/MPU6500 -I../Core/Application/Inc -I../FATFS/Target -I../FATFS/App -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Middlewares/Third_Party/FatFs/src -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Drivers-2f-TCA9548A

clean-Core-2f-Drivers-2f-TCA9548A:
	-$(RM) ./Core/Drivers/TCA9548A/tca9548a.cyclo ./Core/Drivers/TCA9548A/tca9548a.d ./Core/Drivers/TCA9548A/tca9548a.o ./Core/Drivers/TCA9548A/tca9548a.su

.PHONY: clean-Core-2f-Drivers-2f-TCA9548A

