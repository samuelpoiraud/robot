/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : led.h
 *	Package : Carte IHM
 *	Description : Contrôle les leds
 *	Auteur : Anthony
 *	Version 2012/01/14
 */

#ifndef _DETECTION_H
	#define _DETECTION_H

	#include "QS/QS_all.h"

	#include "QS/QS_CANmsgList.h"

	void DETECTION_receiveCanMsg(CAN_msg_t *msg);

#endif /* ndef _DETECTION_H */
