#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <cwpack.h>

enum pm_msg_type {PM_MT_MEASUREMENT = 0, PM_MT_CONTROL = 1, PM_MT_REQUEST_MSG = 2, PM_MT_STATUS_FLAGS = 3, PM_MT_START_BOOTLOADER = 4};


// TODO: use int instead of float to allow hardware processing in FPGA?

struct Measurements{
	/// Half Bridge 1 output voltage in volts
	float V_HB_1;
	/// Output voltage A in volts
	float V_OUT_A;
	/// Output voltage B in volts
	float V_OUT_B;
	/// Temperature of NTC near power stage A in °C
	float T_NTC_A;
	/// Temperature of NTC near power stage B in °C
	float T_NTC_B;
	/// Temperature of Microcontroller in °C
	float T_UC;

	bool  fuseON_OFF_State;
	bool  fuseON_ProtectionState;
	bool  fuseEnabled;
	float fuseProtectionCurrent;
	float fuseProtectionTime;
	float moduleCurrent;
	uint32_t id_uc;

};

struct pm_control{
	/// Switch the port B to ground with mosfet
	bool powerswitch;
	/// Set all SICs to diode emulation
	bool zcd;
	/// clear PWM supervisor (MACH FPGA) fault
	bool clear_fault;
};


struct pm_request_messages {
//	/// set true to request a 'Measurements' message
//	bool measurements;
//	/// request only binary fault and status flags
//	bool flags;

	bool measurements;
	/// request only binary fault and status flags
	bool flags;
	// added YS

	bool  fuseRestart;
	bool  fuseParamSave;
	bool  fuseON_OFF;
	bool  fuseEN;
	float fuseProtectionCurrent;
	float fuseProtectionTime;

};




struct pm_status_flags {
	/// Temperature Warning flags of A & B SIC power stages. True means warning
	bool THW_A;
	bool THW_B;
	// The current state of the high current switch
	bool powerswitch;
	// The current state of the diode emulation
	bool zcd;
	// Fault flags of PWM supervisor
	bool fault_a;
	bool fault_b;
};
	

enum INTERFACE_STATUS{
	INTERFACE_STATUS_OK,
	INTERFACE_STATUS_CRC_INVALID,
	INTERFACE_STATUS_MSG_MALFORMED,
	INTERFACE_STATUS_GENERIC_ERROR
};





enum INTERFACE_STATUS pm_decode_start(enum pm_msg_type* msg_type_p, cw_unpack_context* context_p, const uint8_t *buf, size_t len);

_ssize_t pm_measurement_encode(const struct Measurements *m, uint8_t *buf, size_t len);

enum INTERFACE_STATUS pm_measurement_decode(struct Measurements *m, cw_unpack_context* context_p);

_ssize_t pm_control_encode(const struct pm_control *m, uint8_t *buf, size_t len);

enum INTERFACE_STATUS pm_control_decode(struct pm_control *m, cw_unpack_context* context_p);

_ssize_t pm_request_messages_encode (const struct pm_request_messages *r_m_p, uint8_t *buf, size_t len);

enum INTERFACE_STATUS pm_request_messages_decode(struct pm_request_messages *r_m_p, cw_unpack_context* context_p);

_ssize_t pm_status_flags_encode(const struct pm_status_flags *flags_p, uint8_t *buf, size_t len);

enum INTERFACE_STATUS pm_status_flags_decode(struct pm_status_flags *flags_p, cw_unpack_context* context_p);

_ssize_t pm_start_bootloader_encode(uint8_t *buf, size_t len);

enum INTERFACE_STATUS pm_start_bootloader_decode(cw_unpack_context* context_p);





