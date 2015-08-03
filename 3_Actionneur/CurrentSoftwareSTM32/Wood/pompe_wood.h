/*  Club Robot ESEO 2014 - 2015
 *
 *	Fichier : spot_pompe_right.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe pour attraper les spots arri�re droite
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
		 * Initialise de la position de la POMPE_WOOD (stop�)
		 */
		void POMPE_WOOD_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appel�e en fin de match
		 */
		void POMPE_WOOD_stop();

		/** G�re les messages CAN li�s au POMPE_WOOD.
		 *
		 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
		 */
		bool_e POMPE_WOOD_CAN_process_msg(CAN_msg_t* msg);

		/**
		 * R�initialise la configuration de l'AX12
		 */
		void POMPE_WOOD_reset_config();

	#endif

#endif	/* PPOMPE_WOOD_H */

