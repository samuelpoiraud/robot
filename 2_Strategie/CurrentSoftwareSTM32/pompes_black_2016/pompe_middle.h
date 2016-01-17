/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_middle.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe MIDDLE
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */


#ifndef POMPE_MIDDLE_H
	#define	POMPE_MIDDLE_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

		#include "../queue.h"

		/** Initialisation du gestionnaire de POMPE_MIDDLE.
		 *
		 * Configure du moteur de la POMPE_MIDDLE
		 */
		void POMPE_MIDDLE_init();


		/** Gère les messages CAN liés au POMPE_MIDDLE.
		 *
		 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
		 */
		bool_e POMPE_MIDDLE_CAN_process_msg(CAN_msg_t* msg);

#endif	/* POMPE_MIDDLE_H */

