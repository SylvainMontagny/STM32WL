################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/Startup/data_init.c 

OBJS += \
./Application/Startup/data_init.o 

C_DEPS += \
./Application/Startup/data_init.d 


# Each subdirectory must supply rules for building sources it contributes
Application/Startup/%.o: ../Application/Startup/%.c Application/Startup/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DKMS_ENABLED -DUSE_HAL_DRIVER -DSTM32WL55xx -DCORE_CM4 '-DMBEDTLS_CONFIG_FILE=<mbed_crypto_config.h>' -DDEBUG -DIT_MANAGEMENT_DISABLED -c -I.. -I../../Inc -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../../../../../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/BSP/STM32WLxx_Nucleo -I../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../Middlewares/ST/STM32_Secure_Engine/Key -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface/CryptoApi -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Modules -I../../../../../../../Middlewares/Third_Party/mbed-crypto/include -I../../../1_Image_BFU/BFU/App -I../../../1_Image_BFU/BFU/Target -I../../../Linker_Common/STM32CubeIDE -I../../../LoRaWAN_End_Node/LoRaWAN/App -I../../../LoRaWAN_End_Node/LoRaWAN/Target -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

