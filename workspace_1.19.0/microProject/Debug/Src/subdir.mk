################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/board.c \
../Src/buttons.c \
../Src/fsm.c \
../Src/light.c \
../Src/main.c \
../Src/pot.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/temp.c \
../Src/uart.c \
../Src/ui.c 

OBJS += \
./Src/board.o \
./Src/buttons.o \
./Src/fsm.o \
./Src/light.o \
./Src/main.o \
./Src/pot.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/temp.o \
./Src/uart.o \
./Src/ui.o 

C_DEPS += \
./Src/board.d \
./Src/buttons.d \
./Src/fsm.d \
./Src/light.d \
./Src/main.d \
./Src/pot.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/temp.d \
./Src/uart.d \
./Src/ui.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F411RE -DSTM32 -DSTM32F4 -DSTM32F411RETx -c -I../Inc -I/Users/panaikornmaneeratchakit/Downloads/Library/CMSIS/Core/Include -I/Users/panaikornmaneeratchakit/Downloads/Library/CMSIS-DEVICE-F4/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/board.cyclo ./Src/board.d ./Src/board.o ./Src/board.su ./Src/buttons.cyclo ./Src/buttons.d ./Src/buttons.o ./Src/buttons.su ./Src/fsm.cyclo ./Src/fsm.d ./Src/fsm.o ./Src/fsm.su ./Src/light.cyclo ./Src/light.d ./Src/light.o ./Src/light.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/pot.cyclo ./Src/pot.d ./Src/pot.o ./Src/pot.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/temp.cyclo ./Src/temp.d ./Src/temp.o ./Src/temp.su ./Src/uart.cyclo ./Src/uart.d ./Src/uart.o ./Src/uart.su ./Src/ui.cyclo ./Src/ui.d ./Src/ui.o ./Src/ui.su

.PHONY: clean-Src

