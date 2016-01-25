/*  Club Robot ESEO 2015 - 2016
 *	SMALL
 *
 *	Fichier : left_arm.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur LEFT_ARM
 *  Auteur : Julien
 *  Version 2016
 *  Robot : SMALL
 */

#ifndef LEFT_ARM_H
	#define	LEFT_ARM_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de LEFT_ARM.
	 *
	 * Configure l'asservissement de l'AX12
	 */
	void LEFT_ARM_init();

	/** Initialisation en position du gestionnaire de LEFT_ARM.
	 *
	 * Initialise la position de l'AX12
	 */
	void LEFT_ARM_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appel�e en fin de match
	 */
	void LEFT_ARM_stop();

	/** G�re les messages CAN li�s � la LEFT_ARM.
	 *
	 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
	 */
	bool_e LEFT_ARM_CAN_process_msg(CAN_msg_t* msg);

	/** G�re les commandes demand�es.
	 *
	 * Fonction � mettre sur la file pour demander une action.
	 */
	void LEFT_ARM_run_command(queue_id_t queueId, bool_e init);


	/**
	 * R�initialise la configuration de l'AX12
	 */
	void LEFT_ARM_reset_config();

#endif	/* LEFT_ARM_H */

