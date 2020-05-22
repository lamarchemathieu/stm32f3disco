



NAME = output

FULLNAME = $(NAME)

$(info Building : $(FULLNAME))

ELF = $(FULLNAME).elf
HEX = $(FULLNAME).hex
BIN = $(FULLNAME).bin
LST = $(FULLNAME).lst

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size

SRCS =	$(wildcard source/*.c) \
		../../STM32Cube_FW_F3_V1.11.0/Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/system_stm32f3xx.c \
		../../STM32Cube_FW_F3_V1.11.0/Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_gpio.c \
		../../STM32Cube_FW_F3_V1.11.0/Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_rcc.c \



SRCS_ASM = ../../STM32Cube_FW_F3_V1.11.0/Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/startup_stm32f303xc.s

		
INCLUDES =	-I./include \
			-I../../STM32Cube_FW_F3_V1.11.0/Drivers/CMSIS/Include \
			-I../../STM32Cube_FW_F3_V1.11.0/Drivers/CMSIS//Device/ST/STM32F3xx/Include \
			-I../../STM32Cube_FW_F3_V1.11.0/Drivers/STM32F3xx_HAL_Driver/Inc/Legacy \
			-I../../STM32Cube_FW_F3_V1.11.0/Drivers/STM32F3xx_HAL_Driver/Inc 

OBJS = $(SRCS:.c=.o) $(SRCS_ASM:.s=.o)
DEPS = $(SRCS:.c=.d) $(SRCS_ASM:.s=.d)

CFLAGS = $(INCLUDES)  -Wall -std=gnu11 -g -ffunction-sections -Os -mthumb -mfloat-abi=softfp -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -DSTM32F303xC -DUSE_FULL_LL_DRIVER
ASFLAGS = $(INCLUDES) -g -mthumb -mfloat-abi=softfp -mcpu=cortex-m4 -mfpu=fpv4-sp-d16
LDSCRIPT = linker.ld
LDFLAGS = -T$(LDSCRIPT) -Wl,-gc-sections -mthumb -mcpu=cortex-m4



all : $(ELF) $(BIN) $(HEX) $(LST) size

$(LST) : $(ELF)
	@echo " OBJDUMP" $(notdir $@)
	@$(OBJDUMP) -x -S $< > $@

$(BIN) : $(ELF)
	@echo " OBJCOPY" $(notdir $@)
	@$(OBJCOPY) -O binary $< $@

$(HEX) : $(ELF)
	@echo " OBJCOPY" $(notdir $@)
	@$(OBJCOPY) -O ihex $< $@

$(ELF) : $(OBJS)
	@echo " LD" $@
	@$(LD) $(LDFLAGS) -o $@ $(OBJS)
	
%.o : %.c
	@echo " CC" $(notdir $@)
	@$(CC) -MMD -MP -c $(CFLAGS) $< -o $@

%.o : %.s
	@echo " AS" $(notdir $@)
	@$(AS) $(ASFLAGS) $< -o $@

flash : $(BIN)
	@ST-LINK_CLI -c SWD -P $(BIN) 0x08000000 -V -Rst

size : $(ELF)
	@echo " SIZE" $(notdir $@)
	@$(SIZE) $<

-include $(DEPS)

clean :
	@echo " Clean binaries"
	@rm -f *.elf
	@rm -f *.bin
	@rm -f *.hex
	@rm -f *.lst
	@echo " Clean objs"
	@rm -f $(OBJS)
	@rm -f $(DEPS)

.PHONY : clean size flash
