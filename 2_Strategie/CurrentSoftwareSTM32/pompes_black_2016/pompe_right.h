/*  Club Robot ESEO 2015 - 2016
 *
 *	Fichier : pompe_right.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pompe RIGHT
 *  Auteur : Valentin
 *  Version 2016
 *  Robot : BIG
 */


#ifndef POMPE_RIGHT_H
	#define	POMPE_RIGHT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

		#include "../queue.h"

		/** Initialisation du gestionnaire de POMPE_RIGHT.
		 *
		 * Configure du moteur de la POMPE_RIGHT
		 */
		void POMPE_RIGHT_init();


		/** Gère les messages CAN liés au POMPE_RIGHT.
		 *
		 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
		 * @param msg le message CAN
		 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
		 */
		bool_e POMPE_RIGHT_CAN_process_msg(CAN_msg_t* msg);

		/** Réalisation d'une commande
		 */
		void POMPE_RIGHT_command(ACT_order_e command);


#endif	/* POMPE_RIGHT_H */

