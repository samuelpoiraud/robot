/*
 *	Club Robot ESEO 2008 - 2009
 *	Archi'Tech
 *
 *	Fichier : Buffer_type.h
 *	Package : Supervision
 *	Description : implémentation des structures du buffer circulaire qui sauvegarde les messages can échangés durant le match 
 *	Auteur : Jixi
 *	Version 20090516
 */


#ifndef BUFFER_TYPES_H
	#define BUFFER_TYPES_H

	
	typedef struct			//définition de la structure qui sera stockée dans le buffer circulaire
	{
		CAN_msg_t message;
		Uint16 temps;
	} CANtime_t;
	
	typedef struct		//définition de la structure du buffer
	{
		CANtime_t tab[BUFFER_SIZE];
		Uint16 indiceDebut;
		Uint16 indiceFin;
	} bufferCirculaire_t;
#endif /* ndef BUFFER_TYPES_H */
