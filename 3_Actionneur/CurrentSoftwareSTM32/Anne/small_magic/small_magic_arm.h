/*  Club Robot ESEO 2016 - 2017
*
*	Fichier : SMALL_MAGIC_ARM_rx24.h
*	Package : Carte actionneur
*	Description : Gestion de l'actionneur RX24 SMALL_MAGIC_ARM
*	Auteur :
*	Version 2017
*	Robot : BIG
*/

#ifndef SMALL_MAGIC_ARM_H
	#define	SMALL_MAGIC_ARM_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de SMALL_MAGIC_ARM.
	 *
	 * Configure l'asservissement du RX24
	 */
	void SMALL_MAGIC_ARM_init();

	/** Initialisation en position du gestionnaire de SMALL_MAGIC_ARM.
	 *
	 * Initialise la position du RX24
	 */
	void SMALL_MAGIC_ARM_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appel�e en fin de match
	 */
	void SMALL_MAGIC_ARM_stop();

	/** G�re les messages CAN li�s � la SMALL_MAGIC_ARM.
	 *
	 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
	 */
	bool_e SMALL_MAGIC_ARM_CAN_process_msg(CAN_msg_t* msg);

	/** G�re les commandes demand�es.
	 *
	 * Fonction � mettre sur la file pour demander une action.
	 */
	void SMALL_MAGIC_ARM_run_command(queue_id_t queueId, bool_e init);


	/**
	 * R�initialise la configuration du RX24
	 */
	void SMALL_MAGIC_ARM_reset_config();

#endif	/* SMALL_MAGIC_ARM_H */

