/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : IHM_receiver.h
 *	Package : EVE
 *	Description : Gestion de la réception et de l'affichage des uart et du CAN 
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111228
 */

#ifndef IHM_RECEIVER_H
	#define IHM_RECEIVER_H
	
	#include "../EVE_QS/EVE_all.h"
	#include "../global_vars.h"
	#include "IHM_support.h"

	//#ifdef IHM_RECEIVER_C 

		//fonction principale du thread de réception du récepteur
		void* IHM_receiver_thread();

		//fonction de mise à jour du temps
		void IHM_receiver_update_time(void);

		//fonction de mise à jour de l'affichage du CAN
		void IHM_receiver_update_can(EVE_CAN_msg_t can_message);
	
		//fonction de mise à jour de l'affichage de l'UART
		void IHM_receiver_update_uart(void);

		//fonction de mise à jour des data
		void IHM_receiver_update_data(void);

		//fonction d'initialisation du recepteur de l'IHM
		void IHM_receiver_init(void);

		//fonction de mise à jour de l'image de fonctionnement des cartes simulées
		void IHM_receiver_update_picture(void);

		//fonction de destruction des bal
		void IHM_receiver_destroy(void);
	//#endif
#endif
