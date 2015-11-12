/*  Club Robot ESEO 2013 - 2014
 *	SMALL
 *
 *	Fichier : PSmallArm.h
 *	Package : Carte actionneur
 *	Description : Gestion du petit bras
 *  Auteur : Arnaud
 *  Version 20130219
 *  Robot : SMALL
 */

#ifndef FISH_BRUSH_H
#define	FISH_BRUSH_H



#include "../QS/QS_all.h"
#include "../QS/QS_CANmsgList.h"
#include "../queue.h"

/** Initialisation du gestionnaire de FISH_BRUSH.
 *
 * Configure l'asservissement de l'AX12
 */
void FISH_BRUSH_init();

/** Initialisation en position du gestionnaire de FISH_BRUSH.
 *
 * Initialise la position de l'AX12
 */
void FISH_BRUSH_init_pos();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appel�e en fin de match
 */
void FISH_BRUSH_stop();

/** G�re les messages CAN li�s au FISH_BRUSH.
 *
 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
 */
bool_e FISH_BRUSH_CAN_process_msg(CAN_msg_t* msg);

/** G�re les commandes demand�es.
 *
 * Fonction � mettre sur la file pour demander une action.
 */
void FISH_BRUSH_run_command(queue_id_t queueId, bool_e init);


/**
 * R�initialise la configuration de l'AX12
 */
void FISH_BRUSH_reset_config();


#endif	/* PFISH_BRUSH_H */

