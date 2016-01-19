/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_middle_left.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe MIDDLE_LEFT
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */


#ifndef POMPE_MIDDLE_LEFT_H
	#define	POMPE_MIDDLE_LEFT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

		#include "../queue.h"

		/** Initialisation du gestionnaire de POMPE_MIDDLE_LEFT.
		 *
		 * Configure du moteur de la POMPE_MIDDLE_LEFT
		 */
		void POMPE_MIDDLE_LEFT_init();


		/** Gère les messages CAN liés au POMPE_MIDDLE_LEFT.
		 *
		 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
		 */
		bool_e POMPE_MIDDLE_LEFT_CAN_process_msg(CAN_msg_t* msg);

		/** Réalisation d'une commande
		 */
		void POMPE_MIDDLE_LEFT_command(ACT_order_e command);


#endif	/* POMPE_MIDDLE_LEFT_H */

