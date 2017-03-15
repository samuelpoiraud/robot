/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : motor_roller_foam.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur MOTOR_TURN_TRIHOLE
 *  Auteur : Arnaud
 *  Version 2017
 *  Robot : BIG
 */


#ifndef MOTOR_TURN_TRIHOLE_H
	#define	MOTOR_TURN_TRIHOLE_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de MOTOR_TURN_TRIHOLE.
	 *
	 * Configure l'asservissement du moteur asservi en vitesse
	 */
	void MOTOR_TURN_TRIHOLE_init();

	/** Initialisation en position du gestionnaire de MOTOR_TURN_TRIHOLE.
	 *
	 * Initialise la vitesse initiale du moteur
	 */
	void MOTOR_TURN_TRIHOLE_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appel�e en fin de match
	 */
	void MOTOR_TURN_TRIHOLE_stop();

	/** G�re les messages CAN li�s � la MOTOR_TURN_TRIHOLE.
	 *
	 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
	 */
	bool_e MOTOR_TURN_TRIHOLE_CAN_process_msg(CAN_msg_t* msg);

	/** G�re les commandes demand�es.
	 *
	 * Fonction � mettre sur la file pour demander une action.
	 */
	void MOTOR_TURN_TRIHOLE_run_command(queue_id_t queueId, bool_e init);

	/**
	 * R�initialise la configuration du moteur
	 */
	void MOTOR_TURN_TRIHOLE_reset_config();

#endif	/* MOTOR_TURN_TRIHOLE_H */

