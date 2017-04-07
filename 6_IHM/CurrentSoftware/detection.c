/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : led.c
 *	Package : Carte IHM
 *	Description : Contrôle les leds
 *	Auteur : Anthony
 *	Version 2012/01/14
 */

#include "detection.h"


void DETECTION_receiveCanMsg(CAN_msg_t *msg){
	Uint8 i;

	switch(msg->sid){
		case BROADCAST_BEACON_ADVERSARY_POSITION_IR:
			for(i=0;i<2;i++){
				global.foe[i].dist = msg->data.broadcast_beacon_adversary_position_ir.adv[i].dist * 20;
				global.foe[i].angle = msg->data.broadcast_beacon_adversary_position_ir.adv[i].angle;
				global.foe[i].fiability_error = msg->data.broadcast_beacon_adversary_position_ir.adv[i].error;
			}
			break;

		case BROADCAST_ADVERSARIES_POSITION:
			if(msg->data.broadcast_adversaries_position.adversary_number < 2){
				Uint8 id = msg->data.broadcast_adversaries_position.adversary_number;
				global.foe[2+id].dist = msg->data.broadcast_adversaries_position.dist;
				global.foe[2+id].angle = msg->data.broadcast_adversaries_position.teta;
				global.foe[2+id].fiability_error = 0;
			}
			break;
		default:
			break;
	}
}
