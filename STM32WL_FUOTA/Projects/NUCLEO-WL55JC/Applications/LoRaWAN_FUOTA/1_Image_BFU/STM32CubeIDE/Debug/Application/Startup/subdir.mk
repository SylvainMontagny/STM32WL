################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Application/Startup/startup_stm32wl55jcix.s 

OBJS += \
./Application/Startup/startup_stm32wl55jcix.o 

S_DEPS += \
./Application/Startup/startup_stm32wl55jcix.d 


# Each subdirectory must supply rules for building sources it contributes
Application/Startup/%.o: ../Application/Startup/%.s Application/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

