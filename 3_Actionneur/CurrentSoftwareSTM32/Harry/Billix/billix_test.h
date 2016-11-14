/*  Club Robot ESEO 2015 - 2016
 *	BIG
 *
 *	Fichier : billix_test.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur BILLIX_TEST
 *  Auteur :
 *  Version 2016
 *  Robot : BIG
 */

#ifndef BILLIX_TEST_H
	#define	BILLIX_TEST_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de BILLIX_TEST.
	 *
	 * Configure l'asservissement de l'AX12
	 */
	void BILLIX_TEST_init();

	/** Initialisation en position du gestionnaire de BILLIX_TEST.
	 *
	 * Initialise la position de l'AX12
	 */
	void BILLIX_TEST_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appel�e en fin de match
	 */
	void BILLIX_TEST_stop();

	/** G�re les messages CAN li�s � la BILLIX_TEST.
	 *
	 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
	 */
	bool_e BILLIX_TEST_CAN_process_msg(CAN_msg_t* msg);

	/** G�re les commandes demand�es.
	 *
	 * Fonction � mettre sur la file pour demander une action.
	 */
	void BILLIX_TEST_run_command(queue_id_t queueId, bool_e init);


	/**
	 * R�initialise la configuration de l'AX12
	 */
	void BILLIX_TEST_reset_config();

#endif	/* BILLIX_TEST_H */

