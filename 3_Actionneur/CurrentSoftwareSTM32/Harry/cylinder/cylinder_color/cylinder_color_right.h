/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : cylinder_color_right.h
*	Package : Carte actionneur
*	Description : Gestion de l'actionneur RX24 CYLINDER_COLOR_RIGHT
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#ifndef CYLINDER_COLOR_RIGHT_H
	#define	CYLINDER_COLOR_RIGHT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de CYLINDER_COLOR_RIGHT.
	 *
	 * Configure l'asservissement du RX24
	 */
	void CYLINDER_COLOR_RIGHT_init();

	/** Initialisation en position du gestionnaire de CYLINDER_COLOR_RIGHT.
	 *
	 * Initialise la position du RX24
	 */
	void CYLINDER_COLOR_RIGHT_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appel�e en fin de match
	 */
	void CYLINDER_COLOR_RIGHT_stop();

	/** G�re les messages CAN li�s � la CYLINDER_COLOR_RIGHT.
	 *
	 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
	 */
	bool_e CYLINDER_COLOR_RIGHT_CAN_process_msg(CAN_msg_t* msg);

	/** G�re les commandes demand�es.
	 *
	 * Fonction � mettre sur la file pour demander une action.
	 */
	void CYLINDER_COLOR_RIGHT_run_command(queue_id_t queueId, bool_e init);


	/**
	 * R�initialise la configuration du RX24
	 */
	void CYLINDER_COLOR_RIGHT_reset_config();

#endif	/* CYLINDER_COLOR_RIGHT_H */

