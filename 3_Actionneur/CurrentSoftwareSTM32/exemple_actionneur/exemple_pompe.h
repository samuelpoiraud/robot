/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : exemple_pompe.c
 *	Package : Carte actionneur
 *	Description : Gestion de exemple pompe
 *  Auteur :
 *  Version 2016
 *  Robot : BIG
 */

#ifndef EXEMPLE_POMPE_H
	#define	EXEMPLE_POMPE_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#ifdef I_AM_ROBOT_BIG

		#include "../queue.h"

		/** Initialisation du gestionnaire de EXEMPLE_POMPE.
		 *
		 * Configure du moteur de la EXEMPLE_POMPE
		 */
		void EXEMPLE_POMPE_init();

		/** Initialisation en position du gestionnaire de EXEMPLE_POMPE.
		 *
		 * Initialise de la position de la EXEMPLE_POMPE (stop�)
		 */
		void EXEMPLE_POMPE_init_pos();

		/** Stoppe l'actionneur.
		 *
		 * Cette fonction est appel�e en fin de match
		 */
		void EXEMPLE_POMPE_stop();

		/** G�re les messages CAN li�s au EXEMPLE_POMPE.
		 *
		 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
		 */
		bool_e EXEMPLE_POMPE_CAN_process_msg(CAN_msg_t* msg);

		/**
		 * R�initialise la configuration de l'AX12
		 */
		void EXEMPLE_POMPE_reset_config();

	#endif
#endif	/* EXEMPLE_POMPE_H */

