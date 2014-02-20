/*
 *  Club Robot ESEO 2008 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Définition de types pour les variables globales
				définies specifiquement pour ce code.
 *  Auteur : Jacen
 */

#ifndef QS_GLOBAL_VARS_H
	#error "config_global_vars_types est inclu par la QS, ne l'incluez pas vous meme"
#endif

typedef enum {
	QUEUE_ACT_Hammer,
	QUEUE_ACT_LongHammer,
	QUEUE_ACT_BallInflater,
	QUEUE_ACT_CandleColor,
	QUEUE_ACT_PlierLeft,
	QUEUE_ACT_PlierRight,
	QUEUE_ACT_Arm,
	NB_ACT
} QUEUE_act_e;
