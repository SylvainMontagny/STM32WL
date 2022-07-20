################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/STM32_Secure_Engine/subdir.mk
-include Middlewares/STM32_Key_Management_Services/subdir.mk
-include Drivers/STM32WLxx_LL_Driver/subdir.mk
-include Drivers/STM32WLxx_HAL_Driver/subdir.mk
-include Drivers/CMSIS/subdir.mk
-include Drivers/BSP/STM32WLxx_Nucleo/subdir.mk
-include Application/Startup/subdir.mk
-include Application/Core/subdir.mk
-include Application/BFU/Target/subdir.mk
-include Application/BFU/App/subdir.mk
-include subdir.mk
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


BUILD_ARTIFACT_NAME := BFU
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
BFU.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
BFU.list \

OBJCOPY_BIN += \
BFU.bin \


# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: BFU.elf secondary-outputs

# Tool invocations
BFU.elf: $(OBJS) $(USER_OBJS) D:\Documents\Desktop\fuota_antoine\STM32WL_FUOTA\Projects\NUCLEO-WL55JC\Applications\LoRaWAN_FUOTA\1_Image_BFU\STM32CubeIDE\BFU.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "BFU.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"D:\Documents\Desktop\fuota_antoine\STM32WL_FUOTA\Projects\NUCLEO-WL55JC\Applications\LoRaWAN_FUOTA\1_Image_BFU\STM32CubeIDE\BFU.ld" --specs=nosys.specs -Wl,-Map="BFU.map" -Wl,--gc-sections -static -L ../../../Linker_Common/STM32CubeIDE --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

BFU.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "BFU.list"
	@echo 'Finished building: $@'
	@echo ' '

BFU.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "BFU.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SIZE_OUTPUT)$(OBJDUMP_LIST)$(EXECUTABLES)$(OBJS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS)$(OBJCOPY_BIN) BFU.elf
	-@echo ' '

post-build:
	arm-none-eabi-objcopy -O binary "BFU.elf" "BFU.bin"
	arm-none-eabi-size "BFU.elf"
	arm-none-eabi-objcopy -j .SE_IF_Code "BFU.elf" se_inter.elf > /dev/null 2>>1
	arm-none-eabi-objcopy --extract-symbol se_inter.elf se_interface_app.elf
	arm-none-eabi-objcopy -S --keep-symbols=../se_interface.txt se_interface_app.elf se_interface_app.o
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents fail-specified-linker-script-missing warn-no-linker-script-specified post-build

-include ../makefile.targets
