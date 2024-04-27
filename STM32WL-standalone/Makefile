# Path to your toolchain installation, leave empty if already in system PATH
TOOLCHAIN_ROOT =

# Path to the STM32 codebase, make sure to update the submodule to get the code
VENDOR_ROOT = ./bsp/

###############################################################################

# Project specific
TARGET = STM32WL-standalone
ASM_DIR = Startup/

# Toolchain
CC = $(TOOLCHAIN_ROOT)arm-none-eabi-gcc

# Project sources
SRC_FILES  = $(wildcard Core/*.c)
SRC_FILES += $(wildcard Drivers/BSP/STM32WLxx_Nucleo/*.c)
SRC_FILES += $(wildcard Drivers/Sensors/*.c)
SRC_FILES += $(wildcard Drivers/STM32WLxx_HAL_Driver/Src/*.c)
SRC_FILES += $(wildcard Drivers/Utilities/*.c)
SRC_FILES += $(wildcard LoRaWAN/*/*.c)
SRC_FILES += $(wildcard Middlewares/LoRaWAN/*.c) $(wildcard Middlewares/LoRaWAN/*/*.c)
SRC_FILES += $(wildcard Middlewares/SubGHz_Phy/*.c)

ASM_FILES = $(wildcard $(ASM_DIR)*.s)
LD_SCRIPT = STM32WL55JCIX_FLASH.ld

# Project includes
INCLUDES += -IDrivers/CMSIS/Include
INCLUDES += -IDrivers/BSP/STM32WLxx_Nucleo
INCLUDES += -ILoRaWAN/Target
INCLUDES += -IMiddlewares/LoRaWAN/Packages
INCLUDES += -IMiddlewares/LoRaWAN
INCLUDES += -IMiddlewares/SubGHz_Phy
INCLUDES += -IDrivers/CMSIS/ST/STM32WLxx/Include/
INCLUDES += -ILoRaWAN/App
INCLUDES += -IMiddlewares/LoRaWAN/Utilities
INCLUDES += -IMiddlewares/LoRaWAN/Region
INCLUDES += -IDrivers/STM32WLxx_HAL_Driver/Inc
INCLUDES += -IDrivers/Inc
INCLUDES += -IDrivers/Utilities
INCLUDES += -IDrivers/Sensors

INCLUDES += -IUtilities

# Compiler Flags
CFLAGS  = -g3 -O0 -Wall -std=gnu11 
CFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
CFLAGS += -DCORE_CM4 -DSTM32WL55xx -DUSE_HAL_DRIVER -DDEBUG
CFLAGS += $(INCLUDES)

# Linker Flags
LFLAGS = -T$(LD_SCRIPT) --specs=nosys.specs -Wl,--gc-sections -static --specs=nano.specs -Wl,--start-group -lc -lm -Wl,--end-group

###############################################################################

# This does an in-source build. An out-of-source build that places all object
# files into a build directory would be a better solution, but the goal was to
# keep this file very simple.

CXX_OBJS = $(SRC_FILES:.c=.o)
ASM_OBJS = $(ASM_FILES:.s=.o)
ALL_OBJS = $(ASM_OBJS) $(CXX_OBJS)

.PHONY: clean

all: $(TARGET).elf $(TARGET).bin 

# Compile
$(CXX_OBJS): %.o: %.c
$(ASM_OBJS): %.o: %.s
$(ALL_OBJS):
	@echo "[CC] $@"
	@$(CC) $(CFLAGS) -c $< -o $@

# Link
%.elf: $(ALL_OBJS)
	@echo "[LD] $@"
	@$(CC) $(CFLAGS) $(LFLAGS) $(ALL_OBJS) -o $@

# Clean
clean:
	@rm -f $(ALL_OBJS) $(TARGET).elf  $(TARGET).bin
    

%.bin: $(ALL_OBJS)
	arm-none-eabi-objcopy  -O binary $(TARGET).elf "STM32WL-standalone.bin"
	@echo 'Finished building: $@'
	@echo ' '
