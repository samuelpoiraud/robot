/*  Club Robot ESEO 2014 - 2015
 *
 *	Fichier : spot_pompe_right.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe pour attraper les spots arri�re droite
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
		 * Initialise de la position de la SPOT_POMPE_RIGHT (stop�)
		 */
		void SPOT_POMPE_RIGHT_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appel�e en fin de match
		 */
		void SPOT_POMPE_RIGHT_stop();

		/** G�re les messages CAN li�s au SPOT_POMPE_RIGHT.
		 *
		 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
		 */
		bool_e SPOT_POMPE_RIGHT_CAN_process_msg(CAN_msg_t* msg);

		/**
		 * R�initialise la configuration de l'AX12
		 */
		void SPOT_POMPE_RIGHT_reset_config();

	#endif
#endif	/* PSPOT_POMPE_RIGHT_H */

