################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/Documents/Desktop/fuota_antoine/STM32WL_FUOTA/Projects/NUCLEO-WL55JC/Applications/LoRaWAN_FUOTA/LoRaWAN_End_Node/Core/Src/system_stm32wlxx.c 

OBJS += \
./Drivers/CMSIS/system_stm32wlxx.o 

C_DEPS += \
./Drivers/CMSIS/system_stm32wlxx.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/system_stm32wlxx.o: D:/Documents/Desktop/fuota_antoine/STM32WL_FUOTA/Projects/NUCLEO-WL55JC/Applications/LoRaWAN_FUOTA/LoRaWAN_End_Node/Core/Src/system_stm32wlxx.c Drivers/CMSIS/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32WL55xx -DCORE_CM4 -DKMS_ENABLED -DDISABLE_IMAGE_STATE_HANDLING -c -I../../Core/Inc -I../../LoRaWAN/App -I../../LoRaWAN/Target -I../../KMS/App -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../../../../../../Utilities/trace/adv_trace -I../../../../../../../Utilities/misc -I../../../../../../../Utilities/sequencer -I../../../../../../../Utilities/timer -I../../../../../../../Utilities/lpm/tiny_lpm -I../../../../../../../Middlewares/Third_Party/LoRaWAN/LmHandler/Packages -I../../../../../../../Middlewares/Third_Party/SubGHz_Phy -I../../../../../../../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../../../../../../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../../../../../../Middlewares/Third_Party/LoRaWAN/Crypto -I../../../../../../../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../../../../../../../Middlewares/Third_Party/LoRaWAN/Mac -I../../../../../../../Middlewares/Third_Party/LoRaWAN/LmHandler -I../../../../../../../Middlewares/Third_Party/LoRaWAN/Utilities -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/BSP/STM32WLxx_Nucleo -I../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/iKMS -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Interface -I../../../1_Image_SECoreBin/Inc -I../../../1_Image_BFU/BFU/App -I../../../Linker_Common/STM32CubeIDE -Os -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

