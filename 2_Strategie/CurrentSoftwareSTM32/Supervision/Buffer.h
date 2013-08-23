/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech
 *
 *	Fichier : Buffer.h
 *	Package : Supervision
 *	Description : sp�cification des fonctions du buffer circulaire qui sauvegarde les messages can �chang�s durant le match ainsi que la base de temps
 *	Auteur : Jixi
 *	Version 20090305
 */
 
	#include "QS/QS_all.h"
 #ifndef BUFFER_H
	#define BUFFER_H
	#include "QS/QS_timer.h"
	#include "QS/QS_uart.h"
	#include "QS/QS_can_over_uart.h"
	#include "QS/QS_CANmsgList.h"
	
	void BUFFER_init();
	
	void BUFFER_add(CAN_msg_t * m);
	/* Cette fonction ajoute au buffer circulaire (en queue de la file) un �l�ment de type CANtime (message CAN associ� � son moment d'occurence)
	 */

	void BUFFER_flush();
	/* Cette fonction vide le buffer sur l'UART2
	 */
#endif /* ndef BUFFER_H */
