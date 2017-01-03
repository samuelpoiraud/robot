/*  Club Robot ESEO 2016 - 2017
 *
 *	Fichier : ORE_MIXER_ax12.h
 *	Package : Carte actionneur
 *	Description : Gestion de l'actionneur AX12 ORE_MIXER
 *  Auteur :
 *  Version 2017
 *  Robot : BIG
 */


#ifndef ORE_MIXER_H
	#define	ORE_MIXER_H

	#include "../QS/QS_all.h"
	#include "../QS/QS_CANmsgList.h"

	#include "../queue.h"

	/** Initialisation du gestionnaire de ORE_MIXER.
	 *
	 * Configure l'asservissement de l'AX12
	 */
	void ORE_MIXER_init();

	/** Initialisation en position du gestionnaire de ORE_MIXER.
	 *
	 * Initialise la position de l'AX12
	 */
	void ORE_MIXER_init_pos();

	/** Stoppe l'actionneur.
	 *
	 * Cette fonction est appel�e en fin de match
	 */
	void ORE_MIXER_stop();

	/** G�re les messages CAN li�s � la ORE_MIXER.
	 *
	 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
	 * @param msg le message CAN
	 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
	 */
	bool_e ORE_MIXER_CAN_process_msg(CAN_msg_t* msg);

	/** G�re les commandes demand�es.
	 *
	 * Fonction � mettre sur la file pour demander une action.
	 */
	void ORE_MIXER_run_command(queue_id_t queueId, bool_e init);


	/**
	 * R�initialise la configuration de l'AX12
	 */
	void ORE_MIXER_reset_config();

#endif	/* ORE_MIXER_H */

