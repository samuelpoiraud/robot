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
	RF_TINY,
	RF_KRUSTY,
	RF_FOE1,
	RF_FOE2,
	RF_BROADCAST = 7
} RF_module_e;

void RF_init();
void RF_can_send(RF_module_e target_id, CAN_msg_t *msg);
void RF_synchro_request(RF_module_e target_id);
void RF_synchro_response(RF_module_e target_id, Uint8 timer_offset);

//void RF_set_channel(Uint8 channel);
//void RF_set_output_power(Uint8 power);
//void RF_set_destination_id(Uint8 address);

//Uint8 RF_get_channel();
//Uint8 RF_get_output_power();
//Uint8 RF_get_rssi();
//Uint8 RF_get_destination_id();

#endif

#endif // QS_RF_H
