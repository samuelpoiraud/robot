/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : ORE_GUN_rx24.h
*	Package : Carte actionneur
*	Description : Gestion de l'actionneur RX24 ORE_GUN
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#ifndef ORE_GUN_H
	#define	ORE_GUN_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de ORE_GUN.
	 *
	 * Configure l'asservissement du RX24
	 */
	void ORE_GUN_init();

	/** Initialisation en position du gestionnaire de ORE_GUN.
	 *
	 * Initialise la position du RX24
	 */
	void ORE_GUN_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appel�e en fin de match
	 */
	void ORE_GUN_stop();

	/** G�re les messages CAN li�s � la ORE_GUN.
	 *
	 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
	 */
	bool_e ORE_GUN_CAN_process_msg(CAN_msg_t* msg);

	/** G�re les commandes demand�es.
	 *
	 * Fonction � mettre sur la file pour demander une action.
	 */
	void ORE_GUN_run_command(queue_id_t queueId, bool_e init);


	/**
	 * R�initialise la configuration du RX24
	 */
	void ORE_GUN_reset_config();

#endif	/* ORE_GUN_H */

