/*  Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : Long_hammer.c
 *	Package : Carte actionneur
 *	Description : Gestion du marteau appuyant sur les bougies du haut
 *  Auteur : Alexis
 *  Version 20130207
 */

#ifndef LONG_HAMMER_H
#define	LONG_HAMMER_H

#include "QS/QS_all.h"
#include "queue.h"

/** Initialisation du gestionnaire du bras.
 *
 * Configure DCMotor pour l'asservissement du bras.
 */
void LONGHAMMER_init();

/** G�re les messages CAN li�s au bras.
 *
 * Cette fonction s'occupe toute seule de g�rer la queue et d'asservir le bras � la position voulue.
 * @param msg le message CAN
 */
void LONGHAMMER_CAN_process_msg(CAN_msg_t* msg);

/**
 * Change la position du bras.
 * Cette fonction est appel�e par QUEUE_run(), elle ne devrait pas �tre appel�e � la main.
 * Quand init est TRUE, la commande de changement de position est faite.
 * Apr�s, quand init est FALSE, on regarde si le bras � atteint sa position ou qu'il y a eu TIMEOUT,
 * si oui on fini la queue associ�e et si c'est un TIMEOUT on envoie un message CAN indiquant l'�v�nement (optionel, voir le .c dans LONGHAMMER_run_command).
 * @param queueId le num�ro de la queue
 * @param init TRUE si la commande doit �tre initialis�e, FALSE sinon
 */
void LONGHAMMER_run_command(queue_id_t queueId, bool_e init);

#endif	/* LONG_HAMMER_H */

