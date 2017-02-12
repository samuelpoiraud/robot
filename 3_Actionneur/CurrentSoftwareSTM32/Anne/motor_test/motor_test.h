/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : MOTOR_TEST_ax12.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 MOTOR_TEST
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */


#ifndef MOTOR_TEST_H
	#define	MOTOR_TEST_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de MOTOR_TEST.
	 *
	 * Configure l'asservissement de l'AX12
	 */
	void MOTOR_TEST_init();

	/** Initialisation en position du gestionnaire de MOTOR_TEST.
	 *
	 * Initialise la position de l'AX12
	 */
	void MOTOR_TEST_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void MOTOR_TEST_stop();

	/** Gère les messages CAN liés à la MOTOR_TEST.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e MOTOR_TEST_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void MOTOR_TEST_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration de l'AX12
	 */
	void MOTOR_TEST_reset_config();

	void MOTOR_TEST_test();

	void MOTOR_TEST_process_it();

#endif	/* MOTOR_TEST_H */

