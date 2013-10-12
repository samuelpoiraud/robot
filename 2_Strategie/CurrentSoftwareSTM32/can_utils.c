/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech, CHOMP
 *
 *	Fichier : can_utils.c
 *	Package : Carte Principale
 *	Description : Fonctions pratiques pour le CAN
 *	Auteur : Jacen
 *	Version 20100509
 */

#define CAN_UTILS_C

#include "can_utils.h"
#include "config_debug.h"



/* envoie un message CAN contenant le texte spécifié */
void CAN_send_debug(char* string)
{
#ifdef USE_SEND_CAN_DEBUG
	Uint8 i;
	CAN_msg_t msg;
	msg.sid = DEBUG_CARTE_P;
	for(i=0;i<8;i++)
	{
		msg.data[i] = (Uint8)*(string+i);
	}
	msg.size = 8;
	CAN_send(&msg);
#endif /* def USE_SEND_CAN_DEBUG */
}
