################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/Documents/Desktop/fuota_antoine/STM32WL_FUOTA/Projects/NUCLEO-WL55JC/Applications/LoRaWAN_FUOTA/1_Image_BFU/BFU/Target/sfu_low_level.c \
D:/Documents/Desktop/fuota_antoine/STM32WL_FUOTA/Projects/NUCLEO-WL55JC/Applications/LoRaWAN_FUOTA/1_Image_BFU/BFU/Target/sfu_low_level_flash.c \
D:/Documents/Desktop/fuota_antoine/STM32WL_FUOTA/Projects/NUCLEO-WL55JC/Applications/LoRaWAN_FUOTA/1_Image_BFU/BFU/Target/sfu_low_level_flash_ext.c \
D:/Documents/Desktop/fuota_antoine/STM32WL_FUOTA/Projects/NUCLEO-WL55JC/Applications/LoRaWAN_FUOTA/1_Image_BFU/BFU/Target/sfu_low_level_flash_int.c \
D:/Documents/Desktop/fuota_antoine/STM32WL_FUOTA/Projects/NUCLEO-WL55JC/Applications/LoRaWAN_FUOTA/1_Image_BFU/BFU/Target/sfu_low_level_security.c 

OBJS += \
./Application/BFU/Target/sfu_low_level.o \
./Application/BFU/Target/sfu_low_level_flash.o \
./Application/BFU/Target/sfu_low_level_flash_ext.o \
./Application/BFU/Target/sfu_low_level_flash_int.o \
./Application/BFU/Target/sfu_low_level_security.o 

C_DEPS += \
./Application/BFU/Target/sfu_low_level.d \
./Application/BFU/Target/sfu_low_level_flash.d \
./Application/BFU/Target/sfu_low_level_flash_ext.d \
./Application/BFU/Target/sfu_low_level_flash_int.d \
./Application/BFU/Target/sfu_low_level_security.d 


# Each subdirectory must supply rules for building sources it contributes
Application/BFU/Target/sfu_low_level.o: D:/Documents/Desktop/fuota_antoine/STM32WL_FUOTA/Projects/NUCLEO-WL55JC/Applications/LoRaWAN_FUOTA/1_Image_BFU/BFU/Target/sfu_low_level.c Application/BFU/Target/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DKMS_ENABLED -DUSE_HAL_DRIVER -DSTM32WL55xx -DCORE_CM4 -DDEBUG -c -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../../../../../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/BSP/STM32WLxx_Nucleo -I../../Core/Inc -I../../BFU/App -I../../BFU/Target -I../../../1_Image_SECoreBin/Inc -I../../../Linker_Common/STM32CubeIDE -I../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../Middlewares/ST/STM32_Secure_Engine/Interface -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/iKMS -Os -ffunction-sections -fdata-sections -Wall -Wno-format -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Application/BFU/Target/sfu_low_level_flash.o: D:/Documents/Desktop/fuota_antoine/STM32WL_FUOTA/Projects/NUCLEO-WL55JC/Applications/LoRaWAN_FUOTA/1_Image_BFU/BFU/Target/sfu_low_level_flash.c Application/BFU/Target/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DKMS_ENABLED -DUSE_HAL_DRIVER -DSTM32WL55xx -DCORE_CM4 -DDEBUG -c -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../../../../../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/BSP/STM32WLxx_Nucleo -I../../Core/Inc -I../../BFU/App -I../../BFU/Target -I../../../1_Image_SECoreBin/Inc -I../../../Linker_Common/STM32CubeIDE -I../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../Middlewares/ST/STM32_Secure_Engine/Interface -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/iKMS -Os -ffunction-sections -fdata-sections -Wall -Wno-format -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Application/BFU/Target/sfu_low_level_flash_ext.o: D:/Documents/Desktop/fuota_antoine/STM32WL_FUOTA/Projects/NUCLEO-WL55JC/Applications/LoRaWAN_FUOTA/1_Image_BFU/BFU/Target/sfu_low_level_flash_ext.c Application/BFU/Target/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DKMS_ENABLED -DUSE_HAL_DRIVER -DSTM32WL55xx -DCORE_CM4 -DDEBUG -c -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../../../../../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/BSP/STM32WLxx_Nucleo -I../../Core/Inc -I../../BFU/App -I../../BFU/Target -I../../../1_Image_SECoreBin/Inc -I../../../Linker_Common/STM32CubeIDE -I../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../Middlewares/ST/STM32_Secure_Engine/Interface -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/iKMS -Os -ffunction-sections -fdata-sections -Wall -Wno-format -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Application/BFU/Target/sfu_low_level_flash_int.o: D:/Documents/Desktop/fuota_antoine/STM32WL_FUOTA/Projects/NUCLEO-WL55JC/Applications/LoRaWAN_FUOTA/1_Image_BFU/BFU/Target/sfu_low_level_flash_int.c Application/BFU/Target/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DKMS_ENABLED -DUSE_HAL_DRIVER -DSTM32WL55xx -DCORE_CM4 -DDEBUG -c -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../../../../../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/BSP/STM32WLxx_Nucleo -I../../Core/Inc -I../../BFU/App -I../../BFU/Target -I../../../1_Image_SECoreBin/Inc -I../../../Linker_Common/STM32CubeIDE -I../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../Middlewares/ST/STM32_Secure_Engine/Interface -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/iKMS -Os -ffunction-sections -fdata-sections -Wall -Wno-format -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Application/BFU/Target/sfu_low_level_security.o: D:/Documents/Desktop/fuota_antoine/STM32WL_FUOTA/Projects/NUCLEO-WL55JC/Applications/LoRaWAN_FUOTA/1_Image_BFU/BFU/Target/sfu_low_level_security.c Application/BFU/Target/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DKMS_ENABLED -DUSE_HAL_DRIVER -DSTM32WL55xx -DCORE_CM4 -DDEBUG -c -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../../../../../../../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/BSP/STM32WLxx_Nucleo -I../../Core/Inc -I../../BFU/App -I../../BFU/Target -I../../../1_Image_SECoreBin/Inc -I../../../Linker_Common/STM32CubeIDE -I../../../../../../../Middlewares/ST/STM32_Secure_Engine/Core -I../../../../../../../Middlewares/ST/STM32_Secure_Engine/Interface -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/Core -I../../../../../../../Middlewares/ST/STM32_Key_Management_Services/iKMS -Os -ffunction-sections -fdata-sections -Wall -Wno-format -Wno-strict-aliasing -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

