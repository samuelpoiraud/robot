/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi'Tech
 *
 *	Fichier : Buffer.h
 *	Package : Supervision
 *	Description : spécification des fonctions du buffer circulaire qui sauvegarde les messages can échangés durant le match ainsi que la base de temps
 *	Auteur : Jixi
 *	Version 20090305
 */
 
	#include "../QS/QS_all.h"
 #ifndef BUFFER_H
	#define BUFFER_H

	

//Taille du buffer de mémorisation des messages CAN pendant le match...
	//ATTENTION : il faut qu'il soit grand... mais pas trop, sinon il n'y a plus assez de place pour la pile...
	#define BUFFER_SIZE		20


	void BUFFER_init();
	
	void BUFFER_add(CAN_msg_t * m);
	/* Cette fonction ajoute au buffer circulaire (en queue de la file) un élément de type CANtime (message CAN associé à son moment d'occurence)
	 */

	void BUFFER_flush();
	/* Cette fonction vide le buffer sur l'UART2
	 */



#endif /* ndef BUFFER_H */
