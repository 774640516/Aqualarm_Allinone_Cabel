################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
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

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
CH32X035G8U6_AIO_Master.hex \

SECONDARY_LIST += \
CH32X035G8U6_AIO_Master.lst \

SECONDARY_SIZE += \
CH32X035G8U6_AIO_Master.siz \


# All Target
all: CH32X035G8U6_AIO_Master.elf secondary-outputs

# Tool invocations
CH32X035G8U6_AIO_Master.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -T "C:\MRS_DATA\workspace\Aqualarm_Allinone_Cabel-master\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"CH32X035G8U6_AIO_Master.map" --specs=nano.specs --specs=nosys.specs -o "CH32X035G8U6_AIO_Master.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
CH32X035G8U6_AIO_Master.hex: CH32X035G8U6_AIO_Master.elf
	@	riscv-none-embed-objcopy -O ihex "CH32X035G8U6_AIO_Master.elf"  "CH32X035G8U6_AIO_Master.hex"
	@	@
CH32X035G8U6_AIO_Master.lst: CH32X035G8U6_AIO_Master.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "CH32X035G8U6_AIO_Master.elf" > "CH32X035G8U6_AIO_Master.lst"
	@	@
CH32X035G8U6_AIO_Master.siz: CH32X035G8U6_AIO_Master.elf
	@	riscv-none-embed-size --format=berkeley "CH32X035G8U6_AIO_Master.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) CH32X035G8U6_AIO_Master.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
