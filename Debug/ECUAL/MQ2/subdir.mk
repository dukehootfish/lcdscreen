################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ECUAL/MQ2/MQ2.c 

OBJS += \
./ECUAL/MQ2/MQ2.o 

C_DEPS += \
./ECUAL/MQ2/MQ2.d 


# Each subdirectory must supply rules for building sources it contributes
ECUAL/MQ2/%.o ECUAL/MQ2/%.su ECUAL/MQ2/%.cyclo: ../ECUAL/MQ2/%.c ECUAL/MQ2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_NUCLEO_64 -DUSE_HAL_DRIVER -DSTM32G491xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/BSP/STM32G4xx_Nucleo -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ECUAL-2f-MQ2

clean-ECUAL-2f-MQ2:
	-$(RM) ./ECUAL/MQ2/MQ2.cyclo ./ECUAL/MQ2/MQ2.d ./ECUAL/MQ2/MQ2.o ./ECUAL/MQ2/MQ2.su

.PHONY: clean-ECUAL-2f-MQ2

