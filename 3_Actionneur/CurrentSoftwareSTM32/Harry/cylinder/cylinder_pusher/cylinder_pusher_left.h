/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : CYLINDER_PUSHER_LEFT_ax12.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 CYLINDER_PUSHER_LEFT
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */


#ifndef CYLINDER_PUSHER_LEFT_H
    #define	CYLINDER_PUSHER_LEFT_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

    /** Initialisation du gestionnaire de CYLINDER_PUSHER_LEFT.
	 *
	 * Configure l'asservissement de l'AX12
	 */
    void CYLINDER_PUSHER_LEFT_init();

    /** Initialisation en position du gestionnaire de CYLINDER_PUSHER_LEFT.
	 *
	 * Initialise la position de l'AX12
	 */
    void CYLINDER_PUSHER_LEFT_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appel�e en fin de match
	 */
    void CYLINDER_PUSHER_LEFT_stop();

    /** G�re les messages CAN li�s � la CYLINDER_PUSHER_LEFT.
	 *
	 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
	 */
    bool_e CYLINDER_PUSHER_LEFT_CAN_process_msg(CAN_msg_t* msg);

	/** G�re les commandes demand�es.
	 *
	 * Fonction � mettre sur la file pour demander une action.
	 */
    void CYLINDER_PUSHER_LEFT_run_command(queue_id_t queueId, bool_e init);


	/**
	 * R�initialise la configuration de l'AX12
	 */
    void CYLINDER_PUSHER_LEFT_reset_config();

#endif	/* CYLINDER_PUSHER_LEFT_H */

