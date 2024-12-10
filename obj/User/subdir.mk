################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/Device_GPIO_I2C.c \
../User/PD_Process.c \
../User/button.c \
../User/ch32x035_it.c \
../User/factory.c \
../User/key.c \
../User/led.c \
../User/main.c \
../User/my_I2C_Device.c \
../User/my_PD_Device.c \
../User/my_PD_Valve.c \
../User/my_SPI_Slave.c \
../User/signal_led.c \
../User/system_ch32x035.c \
../User/valve_work.c \
../User/water_work.c 

OBJS += \
./User/Device_GPIO_I2C.o \
./User/PD_Process.o \
./User/button.o \
./User/ch32x035_it.o \
./User/factory.o \
./User/key.o \
./User/led.o \
./User/main.o \
./User/my_I2C_Device.o \
./User/my_PD_Device.o \
./User/my_PD_Valve.o \
./User/my_SPI_Slave.o \
./User/signal_led.o \
./User/system_ch32x035.o \
./User/valve_work.o \
./User/water_work.o 

C_DEPS += \
./User/Device_GPIO_I2C.d \
./User/PD_Process.d \
./User/button.d \
./User/ch32x035_it.d \
./User/factory.d \
./User/key.d \
./User/led.d \
./User/main.d \
./User/my_I2C_Device.d \
./User/my_PD_Device.d \
./User/my_PD_Valve.d \
./User/my_SPI_Slave.d \
./User/signal_led.d \
./User/system_ch32x035.d \
./User/valve_work.d \
./User/water_work.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\MRS_DATA\workspace\Aqualarm_Allinone_Cabel-master\Debug" -I"C:\MRS_DATA\workspace\Aqualarm_Allinone_Cabel-master\Core" -I"C:\MRS_DATA\workspace\Aqualarm_Allinone_Cabel-master\User" -I"C:\MRS_DATA\workspace\Aqualarm_Allinone_Cabel-master\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

