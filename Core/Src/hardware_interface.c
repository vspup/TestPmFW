/*
*     _______                   _                          _                       _
*    |__   __|                 | |                        | |                     | |
*       | |      ___     ___   | |__     _ __      ___    | | __   _ __    __ _   | |_
*       | |     / _ \   / __|  | '_ \   | '_ \    / _ \   | |/ /  | '__|  / _` |  | __|
*       | |    |  __/  | (__   | | | |  | | | |  | (_) |  |   <   | |    | (_| |  | |_
*       |_|     \___|   \___|  |_| |_|  |_| |_|   \___/   |_|\_\  |_|     \__,_|  \__|
*
*  Copyright (c) 2021. Technokrat GmbH / Technokrat LLC
*  All rights reserved
*/
/*
*  Project Gelatini - MRI Coil Charger
*
*  Firmware running on STM32 microcontroller on the power modules.
*/


//#include "stm32g0xx_hal.h"

#include "main.h"
#include "hardware_interface.h"


void hw_set_led(bool on)
{
    GPIO_PinState ps = on ? GPIO_PIN_SET : GPIO_PIN_RESET;  // LED is active high
    //HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, ps);
}

void hw_toggle_led()
{
    //HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

void hw_enable_high_current_switch(bool en)
{
    //GPIO_PinState ps = en ? GPIO_PIN_SET : GPIO_PIN_RESET;
    //HAL_GPIO_WritePin(High_Current_En_GPIO_Port, High_Current_En_Pin, ps);
}

void hw_enable_zcd(bool en)
{
    //GPIO_PinState ps = en ? GPIO_PIN_RESET : GPIO_PIN_SET;  // ZCD is active low
    //HAL_GPIO_WritePin(ZCD_En_GPIO_Port, ZCD_En_Pin, ps);
}

bool hw_state_high_current_switch()
{
   // GPIO_PinState ps = HAL_GPIO_ReadPin(High_Current_En_GPIO_Port, High_Current_En_Pin);
    /*if(ps == GPIO_PIN_SET){
    	return true;
    }
    return false;*/
}

bool hw_state_zcd()
{
    //GPIO_PinState ps = HAL_GPIO_ReadPin(ZCD_En_GPIO_Port, ZCD_En_Pin);
   /* if(ps == GPIO_PIN_RESET){
    	return true;
    }
    return false;*/
}

void hw_disable_power_stage(bool disable)
{
    GPIO_PinState ps = disable ? GPIO_PIN_SET : GPIO_PIN_RESET;
    //HAL_GPIO_WritePin(DISABLE_PWR_STAGE_GPIO_Port, DISABLE_PWR_STAGE_Pin, ps);
}

// checks the FPGA's Done pin to check if it has started
bool hw_mach_loaded()
{
	//GPIO_PinState ps;// = HAL_GPIO_ReadPin(DONE_GPIO_Port, DONE_Pin);
	return 1;//(ps == GPIO_PIN_RESET); // FPGA pulls line low when it is ready (not the actual FPGA's DONE pin)
}

// reset fault flag in MACH FPGA
void hw_mach_clear_fault()
{
	//HAL_GPIO_WritePin(STM2MACH_GPIO_Port, STM2MACH_Pin, GPIO_PIN_SET);
	//HAL_GPIO_WritePin(STM2MACH_GPIO_Port, STM2MACH_Pin, GPIO_PIN_RESET);
}

bool hw_get_mach_fault_a()
{
	//GPIO_PinState ps;// = HAL_GPIO_ReadPin(FAULTA_GPIO_Port, FAULTA_Pin);
	return 0;//(ps == GPIO_PIN_SET);
}

bool hw_get_mach_fault_b()
{
	//GPIO_PinState ps;// = HAL_GPIO_ReadPin(FAULTA_GPIO_Port, FAULTB_Pin);
	return 0;//(ps == GPIO_PIN_SET);
}

bool hw_get_thw_a()
{
	//GPIO_PinState ps = HAL_GPIO_ReadPin(THW_A_GPIO_Port, THW_A_Pin);
	//return (ps == GPIO_PIN_RESET); // THW is active low
}

bool hw_get_thw_b()
{
	//GPIO_PinState ps = HAL_GPIO_ReadPin(THW_B_GPIO_Port, THW_B_Pin);
	//return (ps == GPIO_PIN_RESET); // THW is active low
}




