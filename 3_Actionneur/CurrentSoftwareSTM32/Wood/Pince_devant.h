/*  Club Robot ESEO 2014 - 2015
 *	SMALL
 *
 *	Fichier : Pince_devant.h
 *	Package : Carte actionneur
 *	Description : Gestion de la pince de devant
 *  Auteur : Valentin
 *  Version 20130219
 *  Robot : SMALL
 */

#ifndef PINCE_DEVANT_H
#define	PINCE_DEVANT_H

#include "../QS/QS_all.h"

#include "../queue.h"

/** Initialisation du gestionnaire de PINCE_DEVANT.
 *
 * Configure l'asservissement de l'AX12
 */
void PINCE_DEVANT_init();

/** Initialisation en position du gestionnaire de PINCE_DEVANT.
 *
 * Initialise la position de l'AX12
 */
void PINCE_DEVANT_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appel�e en fin de match
 */
void PINCE_DEVANT_stop();

/** G�re les messages CAN li�s au PINCE_DEVANT.
 *
 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
 */
bool_e PINCE_DEVANT_CAN_process_msg(CAN_msg_t* msg);

/** G�re les commandes demand�es.
 *
 * Fonction � mettre sur la file pour demander une action.
 */
void PINCE_DEVANT_run_command(queue_id_t queueId, bool_e init);


/**
 * R�initialise la configuration de l'AX12
 */
void PINCE_DEVANT_reset_config();

#endif	/* PINCE_DEVANT_H */

