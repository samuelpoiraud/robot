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

typedef enum {
	LH_CMD_GoDown = 0,
	LH_CMD_GoUp = 1,
	LH_CMD_Park = 2,
	LH_CMD_StopAsser
} LONGHAMMER_command_e;

void LONGHAMMER_init();
void LONGHAMMER_run_command(queue_id_t queueId, bool_e init);

#endif	/* LONG_HAMMER_H */

