/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : motor_roller_foam.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur MOTOR_ROLLER_FOAM
 *  Auteur : Arnaud
 *  Version 2017
 *  Robot : BIG
 */


#ifndef MOTOR_ROLLER_FOAM_H
	#define	MOTOR_ROLLER_FOAM_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de MOTOR_ROLLER_FOAM.
	 *
	 * Configure l'asservissement du moteur asservi en vitesse
	 */
	void MOTOR_ROLLER_FOAM_init();

	/** Initialisation en position du gestionnaire de MOTOR_ROLLER_FOAM.
	 *
	 * Initialise la vitesse initiale du moteur
	 */
	void MOTOR_ROLLER_FOAM_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void MOTOR_ROLLER_FOAM_stop();

	/** Gère les messages CAN liés à la MOTOR_ROLLER_FOAM.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e MOTOR_ROLLER_FOAM_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void MOTOR_ROLLER_FOAM_run_command(queue_id_t queueId, bool_e init);

	/**
	 * Réinitialise la configuration du moteur
	 */
	void MOTOR_ROLLER_FOAM_reset_config();

#endif	/* MOTOR_ROLLER_FOAM_H */

