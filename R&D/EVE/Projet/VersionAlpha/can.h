/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : can.h
 *	Package : EVE
 *	Description : Thread de gestion du bus CAN
 *	Auteur : Julien Franchineau & François Even
 *	Version 20120114
 */

#ifndef CAN_H
	#define CAN_H

	#include "EVE_QS/EVE_all.h"

	// Thread de gestion du CAN
	void* CAN_thread();
	
	// Fonction qui envoie un message CAN à toutes les cartes (actives) sauf celle qui à envoyé le message (originating_card)
	void CAN_broadcast_msg(EVE_CAN_msg_t can_msg, cards_id_e originating_card);

	#ifdef CAN_C
		#include "IHM/IHM_receiver.h"
		// Fonction d'initialisation et de création des BAL pour le CAN
		// return END_SUCCESS en cas de succès
		// return ERROR_INIT_CAN_BAL en cas d'erreur lors de l'initialisation
		static Uint16 CAN_init_bal();

		// Fonction de mise à zéro de toutes les variables générales du CAN
		static void CAN_set_zero_can_vars();

		// Fonction de chargement des filtres contenus dans le fichier filtres.config
		static void CAN_load_filters();

		// Fonction qui met à jour l'IHM après un filtrage selon les filtres imposés par le fichier
		static Uint16 CAN_ihm_send_with_filter(EVE_CAN_msg_t can_msg);

		// Fonction qui détruit les BALs du CAN
		static Uint16 CAN_destroy_all();

		// Gestion des retours d'erreurs
		static Uint16 CAN_errors(Uint16 error_id, char* commentary);

	#endif /* CAN_H */

#endif /* CAN_H */

