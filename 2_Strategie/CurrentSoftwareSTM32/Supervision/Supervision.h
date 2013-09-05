/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : main.h
 *	Package : Supervision
 *	Description : sequenceur de la supervision
 *	Auteur : Jacen
 *	Version 20100105
 */

#ifndef SUPERVISION_H
	#define SUPERVISION_H
	
	#include "../QS/QS_all.h"



	void Supervision_init(void);
	void Supervision_process_main(void);
	void Supervision_update_led_beacon(CAN_msg_t * can_msg);
	void Supervision_process_1sec(void);
	
#endif /* ndef SUPERVISION_H */
