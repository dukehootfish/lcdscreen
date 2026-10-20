################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ECUAL/MIC/MIC.c 

OBJS += \
./ECUAL/MIC/MIC.o 

C_DEPS += \
./ECUAL/MIC/MIC.d 


# Each subdirectory must supply rules for building sources it contributes
ECUAL/MIC/%.o ECUAL/MIC/%.su ECUAL/MIC/%.cyclo: ../ECUAL/MIC/%.c ECUAL/MIC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_NUCLEO_64 -DUSE_HAL_DRIVER -DSTM32G491xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/BSP/STM32G4xx_Nucleo -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ECUAL-2f-MIC

clean-ECUAL-2f-MIC:
	-$(RM) ./ECUAL/MIC/MIC.cyclo ./ECUAL/MIC/MIC.d ./ECUAL/MIC/MIC.o ./ECUAL/MIC/MIC.su

.PHONY: clean-ECUAL-2f-MIC

