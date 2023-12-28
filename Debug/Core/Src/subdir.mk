################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/PWM_Handler.c \
../Core/Src/SEGGER_RTT.c \
../Core/Src/SEGGER_RTT_Syscalls_GCC.c \
../Core/Src/SEGGER_RTT_printf.c \
../Core/Src/cwpack.c \
../Core/Src/hardware_interface.c \
../Core/Src/main.c \
../Core/Src/power-module.c \
../Core/Src/stm32g4xx_hal_msp.c \
../Core/Src/stm32g4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32g4xx.c \
../Core/Src/uart-escape.c 

OBJS += \
./Core/Src/PWM_Handler.o \
./Core/Src/SEGGER_RTT.o \
./Core/Src/SEGGER_RTT_Syscalls_GCC.o \
./Core/Src/SEGGER_RTT_printf.o \
./Core/Src/cwpack.o \
./Core/Src/hardware_interface.o \
./Core/Src/main.o \
./Core/Src/power-module.o \
./Core/Src/stm32g4xx_hal_msp.o \
./Core/Src/stm32g4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32g4xx.o \
./Core/Src/uart-escape.o 

C_DEPS += \
./Core/Src/PWM_Handler.d \
./Core/Src/SEGGER_RTT.d \
./Core/Src/SEGGER_RTT_Syscalls_GCC.d \
./Core/Src/SEGGER_RTT_printf.d \
./Core/Src/cwpack.d \
./Core/Src/hardware_interface.d \
./Core/Src/main.d \
./Core/Src/power-module.d \
./Core/Src/stm32g4xx_hal_msp.d \
./Core/Src/stm32g4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32g4xx.d \
./Core/Src/uart-escape.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/PWM_Handler.cyclo ./Core/Src/PWM_Handler.d ./Core/Src/PWM_Handler.o ./Core/Src/PWM_Handler.su ./Core/Src/SEGGER_RTT.cyclo ./Core/Src/SEGGER_RTT.d ./Core/Src/SEGGER_RTT.o ./Core/Src/SEGGER_RTT.su ./Core/Src/SEGGER_RTT_Syscalls_GCC.cyclo ./Core/Src/SEGGER_RTT_Syscalls_GCC.d ./Core/Src/SEGGER_RTT_Syscalls_GCC.o ./Core/Src/SEGGER_RTT_Syscalls_GCC.su ./Core/Src/SEGGER_RTT_printf.cyclo ./Core/Src/SEGGER_RTT_printf.d ./Core/Src/SEGGER_RTT_printf.o ./Core/Src/SEGGER_RTT_printf.su ./Core/Src/cwpack.cyclo ./Core/Src/cwpack.d ./Core/Src/cwpack.o ./Core/Src/cwpack.su ./Core/Src/hardware_interface.cyclo ./Core/Src/hardware_interface.d ./Core/Src/hardware_interface.o ./Core/Src/hardware_interface.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/power-module.cyclo ./Core/Src/power-module.d ./Core/Src/power-module.o ./Core/Src/power-module.su ./Core/Src/stm32g4xx_hal_msp.cyclo ./Core/Src/stm32g4xx_hal_msp.d ./Core/Src/stm32g4xx_hal_msp.o ./Core/Src/stm32g4xx_hal_msp.su ./Core/Src/stm32g4xx_it.cyclo ./Core/Src/stm32g4xx_it.d ./Core/Src/stm32g4xx_it.o ./Core/Src/stm32g4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32g4xx.cyclo ./Core/Src/system_stm32g4xx.d ./Core/Src/system_stm32g4xx.o ./Core/Src/system_stm32g4xx.su ./Core/Src/uart-escape.cyclo ./Core/Src/uart-escape.d ./Core/Src/uart-escape.o ./Core/Src/uart-escape.su

.PHONY: clean-Core-2f-Src

