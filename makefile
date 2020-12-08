# Set the external clock frequency
HSE_VALUE = 8000000L

# Enable debug compilation
#DEBUG = 1

# [OPTIONAL] Uncomment to link to maths library libm
#LIBM = -lm

export DEBUG
export MESSAGES

TARGET_ARCH = -mcpu=cortex-m3 -mthumb
DEVICE_TYPE = STM32F103xB

INCLUDE_DIRS = -I . -I ./Core/Inc -I ./Drivers/CMSIS/Device/ST/STM32F1xx/Include/\
 -I ./Drivers/CMSIS/Include/ -I ./Drivers/STM32F1xx_HAL_Driver/Inc/\
 -I ./Middlewares/Third_Party/FreeRTOS/Source/include/\
 -I ./Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/\
 -I ./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/\

DEFINES = -D$(DEVICE_TYPE) -DHSE_VALUE=$(HSE_VALUE)

export DEFINES

COMPILE_OPTS = $(WARNINGS) $(TARGET_OPTS) $(MESSAGES) $(INCLUDE_DIRS) $(DEFINES)
WARNINGS = -Wall -W -Wshadow -Wcast-qual -Wwrite-strings -Winline

ifdef DEBUG
 TARGET_OPTS = -O2 -g3
 DEBUG_MACRO = -DDEBUG
else
 TARGET_OPTS = -O2 $(F_INLINE) $(F_INLINE_ONCE) $(F_UNROLL_LOOPS)
 F_INLINE = -finline
 F_INLINE_ONCE = -finline-functions-called-once
 #F_UNROLL_LOOPS = -funroll-loops
endif

CC = arm-none-eabi-gcc
CFLAGS = -std=gnu99 $(COMPILE_OPTS)

AS = $(CC) -x assembler-with-cpp -c $(TARGET_ARCH)
ASFLAGS = $(COMPILE_OPTS)

LD = $(CC)
LDFLAGS = -Wl,--gc-sections,-Map=$(MAIN_MAP),-cref -T ./STM32F103C8TX_FLASH.ld $(INCLUDE_DIRS)\
 $(LIBM)

AR = arm-none-eabi-ar
ARFLAGS = cr

OBJCOPY = arm-none-eabi-objcopy
OBJCOPYFLAGS = -O binary

STARTUP_FILE = stm32f103c8tx
STARTUP_OBJ = Core/Startup/startup_$(STARTUP_FILE).o

MAIN_OUT = main.elf
MAIN_MAP = $(MAIN_OUT:%.elf=%.map)
MAIN_BIN = $(MAIN_OUT:%.elf=%.bin)

MAIN_SRCS = ./Core/Src/system_stm32f1xx.c\
	$(wildcard ./Drivers/STM32F1xx_HAL_Driver/Src/*.c)\
	$(wildcard ./Middlewares/ST/STM32_USB_Device_Library/Core/Src/*.c)\
	$(wildcard ./Middlewares/ST/STM32_USB_Device_Library/Class/CustomHID/Src/*.c)\
	$(wildcard ./Core/Src/*.c)\

MAIN_OBJS = $(sort \
	$(patsubst %.c,%.o,$(MAIN_SRCS)) \
	$(STARTUP_OBJ))

.PHONY: all
all: $(MAIN_BIN)

$(MAIN_OUT): $(MAIN_OBJS) $(FWLIB) $(USBLIB)
	$(LD) $(LDFLAGS) $(TARGET_ARCH) $^ -o $@
	arm-none-eabi-size -x -A main.elf 
	arm-none-eabi-size -B main.elf 

$(MAIN_OBJS): $(wildcard ./Drivers/CMSIS/Device/ST/STM32F1xx/Include/*.h) \
 $(wildcard ./Drivers/CMSIS/Include/*.h)\
 $(wildcard ./Drivers/STM32F1xx_HAL_Driver/Inc/*.h)\
 $(wildcard ./Drivers/STM32F1xx_HAL_Driver/Inc/Legacy/*.h)\
 $(wildcard ./Core/Inc/*.h)\
 $(wildcard ./Middlewares/Third_Party/FreeRTOS/Source/include/*.h)\
 $(wildcard ./Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3/*.h)\
 $(wildcard ./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/*.h)\

$(MAIN_BIN): $(MAIN_OUT)
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

clean:
	find . -name \*.o -exec rm {} \;
	rm -rf $(MAIN_BIN) $(MAIN_OUT) $(MAIN_MAP)
