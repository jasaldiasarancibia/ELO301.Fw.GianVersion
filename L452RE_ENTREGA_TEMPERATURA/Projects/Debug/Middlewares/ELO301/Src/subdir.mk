################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/ASUS/Downloads/Entrega_Temperatura_Completa/L452RE_ENTREGA_TEMPERATURA/Middlewares/ELO301/Src/adc_if.c \
C:/Users/ASUS/Downloads/Entrega_Temperatura_Completa/L452RE_ENTREGA_TEMPERATURA/Middlewares/ELO301/Src/gpio_if.c \
C:/Users/ASUS/Downloads/Entrega_Temperatura_Completa/L452RE_ENTREGA_TEMPERATURA/Middlewares/ELO301/Src/lsm6ds3.c \
C:/Users/ASUS/Downloads/Entrega_Temperatura_Completa/L452RE_ENTREGA_TEMPERATURA/Middlewares/ELO301/Src/pwm.c 

OBJS += \
./Middlewares/ELO301/Src/adc_if.o \
./Middlewares/ELO301/Src/gpio_if.o \
./Middlewares/ELO301/Src/lsm6ds3.o \
./Middlewares/ELO301/Src/pwm.o 

C_DEPS += \
./Middlewares/ELO301/Src/adc_if.d \
./Middlewares/ELO301/Src/gpio_if.d \
./Middlewares/ELO301/Src/lsm6ds3.d \
./Middlewares/ELO301/Src/pwm.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ELO301/Src/adc_if.o: C:/Users/ASUS/Downloads/Entrega_Temperatura_Completa/L452RE_ENTREGA_TEMPERATURA/Middlewares/ELO301/Src/adc_if.c Middlewares/ELO301/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L452xx -DLSM6_ENABLED=0 -DPWM_ENABLED=0 -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"../../Middlewares/ELO301/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/ELO301/Src/gpio_if.o: C:/Users/ASUS/Downloads/Entrega_Temperatura_Completa/L452RE_ENTREGA_TEMPERATURA/Middlewares/ELO301/Src/gpio_if.c Middlewares/ELO301/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L452xx -DLSM6_ENABLED=0 -DPWM_ENABLED=0 -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"../../Middlewares/ELO301/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/ELO301/Src/lsm6ds3.o: C:/Users/ASUS/Downloads/Entrega_Temperatura_Completa/L452RE_ENTREGA_TEMPERATURA/Middlewares/ELO301/Src/lsm6ds3.c Middlewares/ELO301/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L452xx -DLSM6_ENABLED=0 -DPWM_ENABLED=0 -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"../../Middlewares/ELO301/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/ELO301/Src/pwm.o: C:/Users/ASUS/Downloads/Entrega_Temperatura_Completa/L452RE_ENTREGA_TEMPERATURA/Middlewares/ELO301/Src/pwm.c Middlewares/ELO301/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L452xx -DLSM6_ENABLED=0 -DPWM_ENABLED=0 -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"../../Middlewares/ELO301/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-ELO301-2f-Src

clean-Middlewares-2f-ELO301-2f-Src:
	-$(RM) ./Middlewares/ELO301/Src/adc_if.cyclo ./Middlewares/ELO301/Src/adc_if.d ./Middlewares/ELO301/Src/adc_if.o ./Middlewares/ELO301/Src/adc_if.su ./Middlewares/ELO301/Src/gpio_if.cyclo ./Middlewares/ELO301/Src/gpio_if.d ./Middlewares/ELO301/Src/gpio_if.o ./Middlewares/ELO301/Src/gpio_if.su ./Middlewares/ELO301/Src/lsm6ds3.cyclo ./Middlewares/ELO301/Src/lsm6ds3.d ./Middlewares/ELO301/Src/lsm6ds3.o ./Middlewares/ELO301/Src/lsm6ds3.su ./Middlewares/ELO301/Src/pwm.cyclo ./Middlewares/ELO301/Src/pwm.d ./Middlewares/ELO301/Src/pwm.o ./Middlewares/ELO301/Src/pwm.su

.PHONY: clean-Middlewares-2f-ELO301-2f-Src

