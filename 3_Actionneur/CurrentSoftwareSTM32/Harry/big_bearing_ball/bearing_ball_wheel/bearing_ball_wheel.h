/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : BEARING_BALL_WHEEL_ax12.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 BEARING_BALL_WHEEL
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */


#ifndef BEARING_BALL_WHEEL_H
    #define	BEARING_BALL_WHEEL_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

    /** Initialisation du gestionnaire de BEARING_BALL_WHEEL.
	 *
	 * Configure l'asservissement de l'AX12
	 */
    void BEARING_BALL_WHEEL_init();

    /** Initialisation en position du gestionnaire de BEARING_BALL_WHEEL.
	 *
	 * Initialise la position de l'AX12
	 */
    void BEARING_BALL_WHEEL_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appel�e en fin de match
	 */
    void BEARING_BALL_WHEEL_stop();

    /** G�re les messages CAN li�s � la BEARING_BALL_WHEEL.
	 *
	 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
	 */
    bool_e BEARING_BALL_WHEEL_CAN_process_msg(CAN_msg_t* msg);

	/** G�re les commandes demand�es.
	 *
	 * Fonction � mettre sur la file pour demander une action.
	 */
    void BEARING_BALL_WHEEL_run_command(queue_id_t queueId, bool_e init);


	/**
	 * R�initialise la configuration de l'AX12
	 */
    void BEARING_BALL_WHEEL_reset_config();

#endif	/* BEARING_BALL_WHEEL_H */

