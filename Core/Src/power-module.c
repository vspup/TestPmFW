#include "power-module.h"
#include <cwpack.h>
#include <uart-escape.h>

// will be encoded in smallest possible integer by cwpack. Make sure this stays <= 0xFF (1 byte)
const uint32_t protocol_version = 0;

// must be implemented externally
uint32_t calculate_crc(const uint8_t *msg, size_t len);

// magic constant to 'secure' jump to bootloader messages
static const uint32_t pm_bl_magic_constant = 0xDEADBEEFUL;



#define FINISH \
	if(pc.end-pc.current < 4){ \
		return -1; \
	} \
	uint32_t crc = calculate_crc(buffer, pc.current-pc.start); \
	uint8_t *ptr = pc.current; \
	for(int i=0; i<4; i++){ \
		*ptr++ = crc&0xff; \
		crc >>= 8; \
	} \
	return uart_escape_encode(buffer, 4+pc.current-pc.start, buf, len);

#define CRC_CHECK \
	uint32_t crc = calculate_crc(buf, len-4); \
	for(int i=len-4; i<len; i++){ \
		if(buf[i] != (crc & 0xff)){ \
			return INTERFACE_STATUS_CRC_INVALID; \
		} \
		crc >>= 8; \
	} \
	len-=4;


//#define CHK(pc) if(pc==false) return INTERFACE_STATUS_GENERIC_ERROR;



// checks the messages CRC, prepares the cw-pack context and decodes the message type
enum INTERFACE_STATUS pm_decode_start(enum pm_msg_type* msg_type_p, cw_unpack_context* context_p, const uint8_t *buf, size_t len) {
	uint64_t prot_version;
	uint64_t message_type;
	CRC_CHECK

	cw_unpack_context_init(context_p, buf, len, 0);
	cw_unpack_next_unsigned(context_p, &prot_version);
	cw_unpack_next_unsigned(context_p, &message_type);

	if(context_p->return_code != CWP_RC_OK){
		return INTERFACE_STATUS_MSG_MALFORMED;
	}

	if (prot_version != protocol_version) {
		return INTERFACE_STATUS_GENERIC_ERROR;
	}
	
	if (message_type > 0xFF) {
		return INTERFACE_STATUS_GENERIC_ERROR;
	}
	
	if (msg_type_p != NULL) {
		*msg_type_p = message_type;
	}
		
	return INTERFACE_STATUS_OK;
}



_ssize_t pm_measurement_encode(const struct Measurements *m, uint8_t *buf, size_t len) {
	cw_pack_context pc;

	static uint8_t buffer[64];
	cw_pack_context_init(&pc, buffer, sizeof(buffer), 0);
	cw_pack_unsigned(&pc, protocol_version);
	cw_pack_unsigned(&pc, PM_MT_MEASUREMENT);
	
	cw_pack_float(&pc, m->T_UC);
	cw_pack_float(&pc, m->T_NTC_A);
	cw_pack_float(&pc, m->T_NTC_B);
	cw_pack_float(&pc, m->V_HB_1);
	cw_pack_float(&pc, m->V_OUT_A);
	cw_pack_float(&pc, m->V_OUT_B);
	

	if(pc.return_code != CWP_RC_OK){
		return -1;
	}
	FINISH
}


enum INTERFACE_STATUS pm_measurement_decode(struct Measurements *m, cw_unpack_context* context_p) {
	if ((context_p == NULL) || (m == NULL)) {
		return INTERFACE_STATUS_GENERIC_ERROR;
	}
	
	 cw_unpack_next_float(context_p, &m->T_UC);
	 cw_unpack_next_float(context_p, &m->T_NTC_A);
	 cw_unpack_next_float(context_p, &m->T_NTC_B );
	 cw_unpack_next_float(context_p, &m->V_HB_1 );
	 cw_unpack_next_float(context_p, &m->V_OUT_A );
	 cw_unpack_next_float(context_p, &m->V_OUT_B);
	 cw_unpack_next_boolean(context_p, &m->fuseON_OFF_State);
	 cw_unpack_next_boolean(context_p, &m->fuseON_ProtectionState);
	 cw_unpack_next_boolean(context_p, &m->fuseEnabled);
	 cw_unpack_next_float(context_p, &m->fuseProtectionCurrent);
	 cw_unpack_next_float(context_p, &m->fuseProtectionTime );
	 cw_unpack_next_float(context_p, &m->moduleCurrent );
	 cw_unpack_next_unsigned(context_p, (uint64_t*)&m->id_uc );

	if(context_p->return_code != CWP_RC_OK) {
		return INTERFACE_STATUS_MSG_MALFORMED; 
	}

	return INTERFACE_STATUS_OK;
}


_ssize_t pm_control_encode(const struct pm_control *m, uint8_t *buf, size_t len) {
	cw_pack_context pc;
	uint8_t buffer[32];
	cw_pack_context_init(&pc, buffer, sizeof(buffer), 0);
	cw_pack_unsigned(&pc, protocol_version);
	cw_pack_unsigned(&pc, PM_MT_CONTROL);
	cw_pack_boolean(&pc, m->powerswitch);
	cw_pack_boolean(&pc, m->zcd);
	cw_pack_boolean(&pc, m->clear_fault);

	if(pc.return_code != CWP_RC_OK){
		return -1;
	}
	FINISH
}


enum INTERFACE_STATUS pm_control_decode(struct pm_control *m, cw_unpack_context* context_p) {
	if ((context_p == NULL) || (m == NULL)) {
		return INTERFACE_STATUS_GENERIC_ERROR;
	}
	
	cw_unpack_next_boolean(context_p, &m->powerswitch);
	cw_unpack_next_boolean(context_p, &m->zcd);
	cw_unpack_next_boolean(context_p, &m->clear_fault);

	if(context_p->return_code != CWP_RC_OK) {
		return INTERFACE_STATUS_MSG_MALFORMED; 
	}
	return INTERFACE_STATUS_OK;
}


_ssize_t pm_request_messages_encode (const struct pm_request_messages *r_m_p, uint8_t *buf, size_t len) {
	/*cw_pack_context pc;
	uint8_t buffer[32];
	cw_pack_context_init(&pc, buffer, sizeof(buffer), 0);
	cw_pack_unsigned(&pc, protocol_version);
	cw_pack_unsigned(&pc, PM_MT_REQUEST_MSG);
	cw_pack_boolean(&pc, r_m_p->measurements);
	cw_pack_boolean(&pc, r_m_p->flags);

	if(pc.return_code != CWP_RC_OK){
		return -1;
	}
	FINISH*/

	cw_pack_context pc;
	uint8_t buffer[32];
	cw_pack_context_init(&pc, buffer, sizeof(buffer), 0);
	cw_pack_unsigned(&pc, protocol_version);
	cw_pack_unsigned(&pc, PM_MT_REQUEST_MSG);
	cw_pack_boolean(&pc, r_m_p->measurements);
	cw_pack_boolean(&pc, r_m_p->flags);

	cw_pack_boolean(&pc, r_m_p->fuseRestart);
	cw_pack_boolean(&pc, r_m_p->fuseParamSave);
	cw_pack_boolean(&pc, r_m_p->fuseON_OFF);
	cw_pack_boolean(&pc, r_m_p->fuseEN);
	cw_pack_float(&pc,   r_m_p->fuseProtectionCurrent);
	cw_pack_float(&pc,   r_m_p->fuseProtectionTime);

	if(pc.return_code != CWP_RC_OK){
			return -1;
	}
	FINISH
}


enum INTERFACE_STATUS pm_request_messages_decode(struct pm_request_messages *r_m_p, cw_unpack_context* context_p) {
	if ((context_p == NULL) || (r_m_p == NULL)) {
		return INTERFACE_STATUS_GENERIC_ERROR;
	}
			
	cw_unpack_next_boolean(context_p, &r_m_p->measurements);
	cw_unpack_next_boolean(context_p, &r_m_p->flags);

	if(context_p->return_code != CWP_RC_OK) {
		return INTERFACE_STATUS_MSG_MALFORMED; 
	}
	
	return INTERFACE_STATUS_OK;
}


_ssize_t pm_status_flags_encode(const struct pm_status_flags *flags_p, uint8_t *buf, size_t len) {
	cw_pack_context pc;

	static uint8_t buffer[64];
	cw_pack_context_init(&pc, buffer, sizeof(buffer), 0);
	cw_pack_unsigned(&pc, protocol_version);
	cw_pack_unsigned(&pc, PM_MT_STATUS_FLAGS);
	
	cw_pack_boolean(&pc, flags_p->THW_A);
	cw_pack_boolean(&pc, flags_p->THW_B);
	cw_pack_boolean(&pc, flags_p->powerswitch);
	cw_pack_boolean(&pc, flags_p->zcd);
	cw_pack_boolean(&pc, flags_p->fault_a);
	cw_pack_boolean(&pc, flags_p->fault_b);

	if(pc.return_code != CWP_RC_OK){
		return -1;
	}
	FINISH
}


enum INTERFACE_STATUS pm_status_flags_decode(struct pm_status_flags *flags_p, cw_unpack_context* context_p) {
	if ((context_p == NULL) || (flags_p == NULL)) {
		return INTERFACE_STATUS_GENERIC_ERROR;
	}
			
	cw_unpack_next_boolean(context_p, &flags_p->THW_A);
	cw_unpack_next_boolean(context_p, &flags_p->THW_B);
	cw_unpack_next_boolean(context_p, &flags_p->powerswitch);
	cw_unpack_next_boolean(context_p, &flags_p->zcd);
	cw_unpack_next_boolean(context_p, &flags_p->fault_a);
	cw_unpack_next_boolean(context_p, &flags_p->fault_b);

	if(context_p->return_code != CWP_RC_OK) {
		return INTERFACE_STATUS_MSG_MALFORMED; 
	}
	
	return INTERFACE_STATUS_OK;
}


_ssize_t pm_start_bootloader_encode(uint8_t *buf, size_t len) {
	cw_pack_context pc;

	static uint8_t buffer[64];
	cw_pack_context_init(&pc, buffer, sizeof(buffer), 0);
	cw_pack_unsigned(&pc, protocol_version);
	cw_pack_unsigned(&pc, PM_MT_START_BOOTLOADER);

	cw_pack_unsigned(&pc, pm_bl_magic_constant);

	if(pc.return_code != CWP_RC_OK){
		return -1;
	}
	FINISH
}


enum INTERFACE_STATUS pm_start_bootloader_decode(cw_unpack_context* context_p) {
	if (context_p == NULL) {
		return INTERFACE_STATUS_GENERIC_ERROR;
	}

	uint64_t magic = 0;
	cw_unpack_next_unsigned(context_p, &magic);

	if(context_p->return_code != CWP_RC_OK) {
		return INTERFACE_STATUS_MSG_MALFORMED;
	}

	// check that we received the expected magic constant
	if (magic == pm_bl_magic_constant) {
		return INTERFACE_STATUS_OK;
	} else {
		return INTERFACE_STATUS_MSG_MALFORMED;
	}
}










