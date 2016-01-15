/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_black_front_right.c
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe BLACK_FRONT_RIGHT
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : SMALL
 */


#ifndef POMPE_BLACK_FRONT_RIGHT_H
	#define	POMPE_BLACK_FRONT_RIGHT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_BIG

		#include "../queue.h"

		/** Initialisation du gestionnaire de POMPE_BLACK_FRONT_RIGHT.
		 *
		 * Configure du moteur de la POMPE_BLACK_FRONT_RIGHT
		 */
		void POMPE_BLACK_FRONT_RIGHT_init();

		/** Initialisation en position du gestionnaire de POMPE_BLACK_FRONT_RIGHT.
		 *
		 * Initialise de la position de la POMPE_BLACK_FRONT_RIGHT (stop�)
		 */
		void POMPE_BLACK_FRONT_RIGHT_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appel�e en fin de match
		 */
		void POMPE_BLACK_FRONT_RIGHT_stop();

		/** G�re les messages CAN li�s au POMPE_BLACK_FRONT_RIGHT.
		 *
		 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
		 */
		bool_e POMPE_BLACK_FRONT_RIGHT_CAN_process_msg(CAN_msg_t* msg);

		/**
		 * R�initialise la configuration de la pompe
		 */
		void POMPE_BLACK_FRONT_RIGHT_reset_config();

	#endif
#endif	/* POMPE_BLACK_FRONT_RIGHT_H */

