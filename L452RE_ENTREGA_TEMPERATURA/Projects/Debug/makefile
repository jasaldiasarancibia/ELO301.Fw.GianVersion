################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/ELO301/Src/subdir.mk
-include Drivers/STM32L4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := L452RE_ENTREGA_TEMPERATURA
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
L452RE_ENTREGA_TEMPERATURA.elf \

MAP_FILES += \
L452RE_ENTREGA_TEMPERATURA.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
L452RE_ENTREGA_TEMPERATURA.list \


# All Target
all: main-build

# Main-build Target
main-build: L452RE_ENTREGA_TEMPERATURA.elf secondary-outputs

# Tool invocations
L452RE_ENTREGA_TEMPERATURA.elf L452RE_ENTREGA_TEMPERATURA.map: $(OBJS) $(USER_OBJS) C:\Users\ASUS\Downloads\Entrega_Temperatura_Completa\L452RE_ENTREGA_TEMPERATURA\Projects\STM32L452RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "L452RE_ENTREGA_TEMPERATURA.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\ASUS\Downloads\Entrega_Temperatura_Completa\L452RE_ENTREGA_TEMPERATURA\Projects\STM32L452RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="L452RE_ENTREGA_TEMPERATURA.map" -Wl,--gc-sections -static -L../../Middlewares/ELO301/Lib --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

L452RE_ENTREGA_TEMPERATURA.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "L452RE_ENTREGA_TEMPERATURA.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) L452RE_ENTREGA_TEMPERATURA.elf L452RE_ENTREGA_TEMPERATURA.list L452RE_ENTREGA_TEMPERATURA.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
