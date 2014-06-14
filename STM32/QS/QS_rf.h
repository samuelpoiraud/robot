/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Qualité Soft
 *  Description : Module RF: synchro + envoi messages CAN (attention c'est très lent ! 4,8kbits/sec, 13 octets/paquet + nos données)
 *  Auteur : amurzeau
 */

#ifndef QS_RF_H
#define QS_RF_H

#include "QS_all.h"

#ifdef USE_RF

// !!!!!!!!!!!! MAX 7 modules !!!!!!!!!!!! (0-6, 7 = broadcast)
typedef enum {
	RF_SMALL,
	RF_BIG,
	RF_FOE1,
	RF_FOE2,
	RF_BROADCAST = 7
} RF_module_e;
#define RF_MODULE_COUNT 4

typedef enum {
	RF_PT_SynchroRequest,
	RF_PT_SynchroResponse,
	RF_PT_Can,
	RF_PT_None = 3
} RF_packet_type_e;

typedef union {
	Uint8 raw_data;
	struct {
		RF_packet_type_e type : 2;
		RF_module_e sender_id : 3;
		RF_module_e target_id : 3;
	};
} RF_header_t;

typedef void (*RF_onReceive_ptr)(bool_e for_me, RF_header_t header, Uint8 *data, Uint8 size);
typedef void (*RF_onCanMsg_ptr)(CAN_msg_t* msg);

void RF_init(RF_module_e module, RF_onReceive_ptr onReceiveCallback, RF_onCanMsg_ptr onCanMsgCallback);
RF_module_e RF_get_module_id();
void RF_can_send(RF_module_e target_id, CAN_msg_t *msg);
void RF_synchro_request(RF_module_e target_id);
void RF_synchro_response(RF_module_e target_id, Sint16 timer_offset);

//void RF_set_channel(Uint8 channel);
//void RF_set_output_power(Uint8 power);
//void RF_set_destination_id(Uint8 address);

//Uint8 RF_get_channel();
//Uint8 RF_get_output_power();
//Uint8 RF_get_rssi();
//Uint8 RF_get_destination_id();

#endif

#endif // QS_RF_H
