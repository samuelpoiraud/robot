/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : CIRCLE.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur CIRCLE
 *  Auteur :
 *  Version 2016
 *  Robot : BIG
 */

#ifndef CIRCLE_H
	#define	CIRCLE_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de CIRCLE.
	 *
	 * Configure l'asservissement de l'AX12
	 */
	void CIRCLE_init();

	/** Initialisation en position du gestionnaire de CIRCLE.
	 *
	 * Initialise la position de l'AX12
	 */
	void CIRCLE_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appel�e en fin de match
	 */
	void CIRCLE_stop();

	/** G�re les messages CAN li�s � la CIRCLE.
	 *
	 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
	 */
	bool_e CIRCLE_CAN_process_msg(CAN_msg_t* msg);

	/** G�re les commandes demand�es.
	 *
	 * Fonction � mettre sur la file pour demander une action.
	 */
	void CIRCLE_run_command(queue_id_t queueId, bool_e init);


	/**
	 * R�initialise la configuration de l'AX12
	 */
	void CIRCLE_reset_config();

#endif	/* CIRCLE_H */

