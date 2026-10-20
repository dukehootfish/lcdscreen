################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ECUAL/BUZZER/BUZZER.c 

OBJS += \
./ECUAL/BUZZER/BUZZER.o 

C_DEPS += \
./ECUAL/BUZZER/BUZZER.d 


# Each subdirectory must supply rules for building sources it contributes
ECUAL/BUZZER/%.o ECUAL/BUZZER/%.su ECUAL/BUZZER/%.cyclo: ../ECUAL/BUZZER/%.c ECUAL/BUZZER/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_NUCLEO_64 -DUSE_HAL_DRIVER -DSTM32G491xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/BSP/STM32G4xx_Nucleo -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ECUAL-2f-BUZZER

clean-ECUAL-2f-BUZZER:
	-$(RM) ./ECUAL/BUZZER/BUZZER.cyclo ./ECUAL/BUZZER/BUZZER.d ./ECUAL/BUZZER/BUZZER.o ./ECUAL/BUZZER/BUZZER.su

.PHONY: clean-ECUAL-2f-BUZZER

