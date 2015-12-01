/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_back_right.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe arrière droite permettant de ventouser un coquillage
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : SMALL
 */


#ifndef POMPE_BACK_RIGHT_H
	#define	POMPE_BACK_RIGHT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_SMALL

		#include "../queue.h"

		/** Initialisation du gestionnaire de POMPE_BACK_RIGHT.
		 *
		 * Configure du moteur de la POMPE_BACK_RIGHT
		 */
		void POMPE_BACK_RIGHT_init();

		/** Initialisation en position du gestionnaire de POMPE_BACK_RIGHT.
		 *
		 * Initialise de la position de la POMPE_BACK_RIGHT (stopé)
		 */
		void POMPE_BACK_RIGHT_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appelée en fin de match
		 */
		void POMPE_BACK_RIGHT_stop();

		/** Gère les messages CAN liés au POMPE_BACK_RIGHT.
		 *
		 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
		 */
		bool_e POMPE_BACK_RIGHT_CAN_process_msg(CAN_msg_t* msg);

		/**
		 * Réinitialise la configuration de la pompe
		 */
		void POMPE_BACK_RIGHT_reset_config();

	#endif
#endif	/* POMPE_BACK_RIGHT_H */

