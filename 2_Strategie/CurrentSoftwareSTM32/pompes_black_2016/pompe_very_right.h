/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_very_right.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe VERY_RIGHT
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */


#ifndef POMPE_VERY_RIGHT_H
	#define	POMPE_VERY_RIGHT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

		#include "../queue.h"

		/** Initialisation du gestionnaire de POMPE_VERY_RIGHT.
		 *
		 * Configure du moteur de la POMPE_VERY_RIGHT
		 */
		void POMPE_VERY_RIGHT_init();


		/** G�re les messages CAN li�s au POMPE_VERY_RIGHT.
		 *
		 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
		 */
		bool_e POMPE_VERY_RIGHT_CAN_process_msg(CAN_msg_t* msg);

#endif	/* POMPE_VERY_RIGHT_H */

