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

/** Gère les messages CAN liés au bras.
 *
 * Cette fonction s'occupe toute seule de gérer la queue et d'asservir le bras à la position voulue.
 * @param msg le message CAN
 */
void LONGHAMMER_CAN_process_msg(CAN_msg_t* msg);

/**
 * Change la position du bras.
 * Cette fonction est appelée par QUEUE_run(), elle ne devrait pas être appelée à la main.
 * Quand init est TRUE, la commande de changement de position est faite.
 * Après, quand init est FALSE, on regarde si le bras à atteint sa position ou qu'il y a eu TIMEOUT,
 * si oui on fini la queue associée et si c'est un TIMEOUT on envoie un message CAN indiquant l'évènement (optionel, voir le .c dans LONGHAMMER_run_command).
 * @param queueId le numéro de la queue
 * @param init TRUE si la commande doit être initialisée, FALSE sinon
 */
void LONGHAMMER_run_command(queue_id_t queueId, bool_e init);

#endif	/* LONG_HAMMER_H */

