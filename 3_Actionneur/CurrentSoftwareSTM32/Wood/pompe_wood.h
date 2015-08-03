/*  Club Robot ESEO 2014 - 2015
 *
 *	Fichier : spot_pompe_right.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe pour attraper les spots arrière droite
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef POMPE_WOOD_H
	#define	POMPE_WOOD_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_SMALL

		#include "../queue.h"

		/** Initialisation du gestionnaire de POMPE_WOOD.
		 *
		 * Configure du moteur de la POMPE_WOOD
		 */
		void POMPE_WOOD_init();

		/** Initialisation en position du gestionnaire de POMPE_WOOD.
		 *
		 * Initialise de la position de la POMPE_WOOD (stopé)
		 */
		void POMPE_WOOD_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appelée en fin de match
		 */
		void POMPE_WOOD_stop();

		/** Gère les messages CAN liés au POMPE_WOOD.
		 *
		 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
		 */
		bool_e POMPE_WOOD_CAN_process_msg(CAN_msg_t* msg);

		/**
		 * Réinitialise la configuration de l'AX12
		 */
		void POMPE_WOOD_reset_config();

	#endif

#endif	/* PPOMPE_WOOD_H */

