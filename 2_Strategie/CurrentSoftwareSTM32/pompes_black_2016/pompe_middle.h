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


		/** G�re les messages CAN li�s au POMPE_MIDDLE.
		 *
		 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
		 */
		bool_e POMPE_MIDDLE_CAN_process_msg(CAN_msg_t* msg);

		/** R�alisation d'une commande
		 */
		void POMPE_MIDDLE_command(ACT_order_e command);


#endif	/* POMPE_MIDDLE_H */

