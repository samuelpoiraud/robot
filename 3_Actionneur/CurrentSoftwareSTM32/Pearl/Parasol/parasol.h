/*  Club Robot ESEO 2015 - 2016
 *	SMALL
 *
 *	Fichier : parasol.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur PARASOL
 *  Auteur :
 *  Version 2016
 *  Robot : SMALL
 */

#ifndef PARASOL_H
	#define	PARASOL_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de PARASOL.
	 *
	 * Configure l'asservissement de l'AX12
	 */
	void PARASOL_init();

	/** Initialisation en position du gestionnaire de PARASOL.
	 *
	 * Initialise la position de l'AX12
	 */
	void PARASOL_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
	void PARASOL_stop();

	/** Gère les messages CAN liés à la PARASOL.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
	bool_e PARASOL_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
	void PARASOL_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration de l'AX12
	 */
	void PARASOL_reset_config();

#endif	/* PARASOL_H */

