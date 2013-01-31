/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech'
 *
 *	Fichier : QS_can.h
 *	Package : Qualit� Soft
 *	Description : fonction CAN
 *	Auteur : Jacen
 *	Version 20090809
 */
 

#ifndef QS_CAN_H
	#define QS_CAN_H

	#include "QS_all.h"	

	/* Prototype */
	/*	Pour chaque fonction, on pr�cise si elle doit s'appliquer
		au CAN 1 ou au CAN 2, en passant en argument canID
		respectivement 1 ou 2.*/
		
	/*	Can_init initialise le module CAN pass� en argument, en
		filtrant les messages arrivant pour ne conserver que les
		messages correspondant � votre filtre ou au filtre des
		messages broadcast. Au club robot, ces valeurs devraient
		�tre, pour une carte actionneur : ACT_FILTER et
		BROADCAST_FILTER. Le masque �tant syst�matiquement
		CAN_ESEO_MASK. Se r�f�rer � CANmsgList.h puis � un membre
		comp�tent en cas de question.
	*/
	void CAN_init(	Uint8 canID, 
					Uint11 my_filter, Uint11 my_mask,
					Uint11 broadcast_filter, Uint11 broadcast_mask);

	/*	Pour certaines utilisations, on peut souhaiter ajouter des
		filtres pour accepter plus de messages avec le m�me masque.
		cette fonction permet d'en ajouter 5 de plus (pour un total de 6).
		Il n'est pas pr�vu d'utiliser cette fonction au club robot. 
		Se r�f�rer � un membre comp�tent.
	*/
	#warning non-fonctionnel
	void CAN_add_custom_filter(Uint8 canID, Uint11 sid);
	/*	Meme fonction pour ajouter un filtre de plus avec le masque de
		broadcast, pour un total de 2. M�me remarque que ci dessus
	*/
	#warning non-fonctionnel
	void CAN_add_broadcast_filter(Uint8 canID, Uint11 sid);

	/*	Permet d'envoyer, via le module CAN pass� en argument,
		le message dont l'adresse est pass�e en argument.
	*/
	void CAN_send(Uint8 canID,CAN_msg_t* can_msg);

	/*	Permet d'envoyer, via le module CAN pass� en argument,
		la requete dont le SID est pass� en argument.
	*/
	void CAN_send_request(Uint8 canID,Uint11 sid);

	/*	indique si un message CAN est arriv� sur le module CAN pass� en
		argument et est � lire
	*/
	bool_e CAN_got_new_message(Uint8 canID);
	
	/*	renvoie le prochain message � lire arriv� sur le module CAN 
		pass� en argument. Renvoie un message de sid 0 et de taille 0
		si il n'y avait pas de massage � lire.
	*/
	CAN_msg_t CAN_get_next_msg(Uint8 canID);
	
#endif /* ndef QS_CAN_H */
