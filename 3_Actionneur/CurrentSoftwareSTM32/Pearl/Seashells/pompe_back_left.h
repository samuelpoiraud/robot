/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_back_left.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe arri�re gauche permettant de ventouser un coquillage
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : SMALL
 */


#ifndef POMPE_BACK_LEFT_H
	#define	POMPE_BACK_LEFT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_SMALL

		#include "../queue.h"

		/** Initialisation du gestionnaire de POMPE_BACK_LEFT.
		 *
		 * Configure du moteur de la POMPE_BACK_LEFT
		 */
		void POMPE_BACK_LEFT_init();

		/** Initialisation en position du gestionnaire de POMPE_BACK_LEFT.
		 *
		 * Initialise de la position de la POMPE_BACK_LEFT (stop�)
		 */
		void POMPE_BACK_LEFT_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appel�e en fin de match
		 */
		void POMPE_BACK_LEFT_stop();

		/** G�re les messages CAN li�s au POMPE_BACK_LEFT.
		 *
		 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
		 */
		bool_e POMPE_BACK_LEFT_CAN_process_msg(CAN_msg_t* msg);

		/**
		 * R�initialise la configuration de la pompe
		 */
		void POMPE_BACK_LEFT_reset_config();

	#endif
#endif	/* POMPE_BACK_LEFT_H */

