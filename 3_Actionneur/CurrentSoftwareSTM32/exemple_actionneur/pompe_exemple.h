/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_exemple.c
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe exemple
 *  Auteur :
 *  Version 2016
 *  Robot : SMALL
 */


#ifndef POMPE_EXEMPLE_H
	#define	POMPE_EXEMPLE_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_BIG

		#include "../queue.h"

		/** Initialisation du gestionnaire de POMPE_EXEMPLE.
		 *
		 * Configure du moteur de la POMPE_EXEMPLE
		 */
		void POMPE_EXEMPLE_init();

		/** Initialisation en position du gestionnaire de POMPE_EXEMPLE.
		 *
		 * Initialise de la position de la POMPE_EXEMPLE (stopé)
		 */
		void POMPE_EXEMPLE_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appelée en fin de match
		 */
		void POMPE_EXEMPLE_stop();

		/** Gère les messages CAN liés au POMPE_EXEMPLE.
		 *
		 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
		 */
		bool_e POMPE_EXEMPLE_CAN_process_msg(CAN_msg_t* msg);

		/**
		 * Réinitialise la configuration de la pompe
		 */
		void POMPE_EXEMPLE_reset_config();

	#endif
#endif	/* POMPE_EXEMPLE_H */

