/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech'
 *
 *	Fichier : QS_can.h
 *	Package : Qualité Soft
 *	Description : fonction CAN
 *	Auteur : Jacen
 *	Version 20090809
 */
 

#ifndef QS_CAN_H
	#define QS_CAN_H

	#include "QS_all.h"	

	/* Prototype */
	/*	Pour chaque fonction, on précise si elle doit s'appliquer
		au CAN 1 ou au CAN 2, en passant en argument canID
		respectivement 1 ou 2.*/
		
	/*	Can_init initialise le module CAN passé en argument, en
		filtrant les messages arrivant pour ne conserver que les
		messages correspondant à votre filtre ou au filtre des
		messages broadcast. Au club robot, ces valeurs devraient
		être, pour une carte actionneur : ACT_FILTER et
		BROADCAST_FILTER. Le masque étant systématiquement
		CAN_ESEO_MASK. Se référer à CANmsgList.h puis à un membre
		compétent en cas de question.
	*/
	void CAN_init(	Uint8 canID, 
					Uint11 my_filter, Uint11 my_mask,
					Uint11 broadcast_filter, Uint11 broadcast_mask);

	/*	Pour certaines utilisations, on peut souhaiter ajouter des
		filtres pour accepter plus de messages avec le même masque.
		cette fonction permet d'en ajouter 5 de plus (pour un total de 6).
		Il n'est pas prévu d'utiliser cette fonction au club robot. 
		Se référer à un membre compétent.
	*/
	#warning non-fonctionnel
	void CAN_add_custom_filter(Uint8 canID, Uint11 sid);
	/*	Meme fonction pour ajouter un filtre de plus avec le masque de
		broadcast, pour un total de 2. Même remarque que ci dessus
	*/
	#warning non-fonctionnel
	void CAN_add_broadcast_filter(Uint8 canID, Uint11 sid);

	/*	Permet d'envoyer, via le module CAN passé en argument,
		le message dont l'adresse est passée en argument.
	*/
	void CAN_send(Uint8 canID,CAN_msg_t* can_msg);

	/*	Permet d'envoyer, via le module CAN passé en argument,
		la requete dont le SID est passé en argument.
	*/
	void CAN_send_request(Uint8 canID,Uint11 sid);

	/*	indique si un message CAN est arrivé sur le module CAN passé en
		argument et est à lire
	*/
	bool_e CAN_got_new_message(Uint8 canID);
	
	/*	renvoie le prochain message à lire arrivé sur le module CAN 
		passé en argument. Renvoie un message de sid 0 et de taille 0
		si il n'y avait pas de massage à lire.
	*/
	CAN_msg_t CAN_get_next_msg(Uint8 canID);
	
#endif /* ndef QS_CAN_H */
