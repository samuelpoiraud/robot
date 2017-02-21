/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : ROCKET_ax12.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 ROCKET
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */


#ifndef ROCKET_H
    #define	ROCKET_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

    /** Initialisation du gestionnaire de ROCKET.
	 *
	 * Configure l'asservissement de l'AX12
	 */
    void ROCKET_init();

    /** Initialisation en position du gestionnaire de ROCKET.
	 *
	 * Initialise la position de l'AX12
	 */
    void ROCKET_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appelée en fin de match
	 */
    void ROCKET_stop();

    /** Gère les messages CAN liés à la ROCKET.
	 *
	 * Cette fonction s'occupe toute seule de gérer la queue et d'agir en conséquence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a été géré par cet actionneur, FALSE sinon
	 */
    bool_e ROCKET_CAN_process_msg(CAN_msg_t* msg);

	/** Gère les commandes demandées.
	 *
	 * Fonction à mettre sur la file pour demander une action.
	 */
    void ROCKET_run_command(queue_id_t queueId, bool_e init);


	/**
	 * Réinitialise la configuration de l'AX12
	 */
    void ROCKET_reset_config();

#endif	/* ROCKET_H */

