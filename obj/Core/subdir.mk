################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/core_riscv.c 

OBJS += \
./Core/core_riscv.o 

C_DEPS += \
./Core/core_riscv.d 


# Each subdirectory must supply rules for building sources it contributes
Core/%.o: ../Core/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\GW\Downloads\Aqualarm_Allinone_Cabel-master (1)\Aqualarm_Allinone_Cabel-master\Debug" -I"C:\Users\GW\Downloads\Aqualarm_Allinone_Cabel-master (1)\Aqualarm_Allinone_Cabel-master\Core" -I"C:\Users\GW\Downloads\Aqualarm_Allinone_Cabel-master (1)\Aqualarm_Allinone_Cabel-master\User" -I"C:\Users\GW\Downloads\Aqualarm_Allinone_Cabel-master (1)\Aqualarm_Allinone_Cabel-master\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

