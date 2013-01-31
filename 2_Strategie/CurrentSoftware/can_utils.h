/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, CHOMP
 *
 *	Fichier : can_utils.h
 *	Package : Carte Principale
 *	Description : Fonctions pratiques pour le CAN
 *	Auteur : Jacen
 *	Version 20100509
 */

#include "QS/QS_all.h"

#ifndef CAN_UTILS_H
	#define CAN_UTILS_H

	#include "QS/QS_CANmsgList.h"
	
	/* envoie un message CAN sans données avec le sid spécifié */
	void CAN_send_sid(Uint11 sid);
	
	/* envoie un message CAN contenant le texte spécifié */
	void CAN_send_debug(char* string);
	
	
	#ifdef CAN_UTILS_C
		#include "QS/QS_can.h"
	#endif /* def CAN_UTILS_C */
	
#endif /* def CAN_UTILS_H */
