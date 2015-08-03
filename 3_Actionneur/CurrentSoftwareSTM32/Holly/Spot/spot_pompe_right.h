/*  Club Robot ESEO 2014 - 2015
 *
 *	Fichier : spot_pompe_right.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe pour attraper les spots arrière droite
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : HOLLY
 */

#ifndef SPOT_POMPE_RIGHT_H
	#define	SPOT_POMPE_RIGHT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_BIG

		#include "../queue.h"

		/** Initialisation du gestionnaire de SPOT_POMPE_RIGHT.
		 *
		 * Configure du moteur de la SPOT_POMPE_RIGHT
		 */
		void SPOT_POMPE_RIGHT_init();

		/** Initialisation en position du gestionnaire de SPOT_POMPE_RIGHT.
		 *
		 * Initialise de la position de la SPOT_POMPE_RIGHT (stopé)
		 */
		void SPOT_POMPE_RIGHT_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appelée en fin de match
		 */
		void SPOT_POMPE_RIGHT_stop();

		/** Gère les messages CAN liés au SPOT_POMPE_RIGHT.
		 *
		 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
		 */
		bool_e SPOT_POMPE_RIGHT_CAN_process_msg(CAN_msg_t* msg);

		/**
		 * Réinitialise la configuration de l'AX12
		 */
		void SPOT_POMPE_RIGHT_reset_config();

	#endif
#endif	/* PSPOT_POMPE_RIGHT_H */

