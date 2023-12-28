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

#ifndef __HARDWARE_INTERFACE__
#define __HARDWARE_INTERFACE__

#include <stdbool.h>

void hw_set_led(bool on);
void hw_toggle_led();
void hw_enable_high_current_switch(bool en);
void hw_enable_zcd(bool en);
bool hw_state_high_current_switch();
bool hw_state_zcd();
void hw_disable_power_stage(bool disable);
void hw_mach_clear_fault();
bool hw_get_mach_fault_a();
bool hw_get_mach_fault_b();
bool hw_mach_loaded();
bool hw_get_thw_a();
bool hw_get_thw_b();


#endif
