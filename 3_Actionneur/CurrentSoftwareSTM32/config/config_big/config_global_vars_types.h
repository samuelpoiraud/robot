/*
 *  Club Robot ESEO 2008 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : D�finition de types pour les variables globales
				d�finies specifiquement pour ce code.
 *  Auteur : Jacen
 */

#ifndef QS_GLOBAL_VARS_H
	#error "config_global_vars_types est inclu par la QS, ne l'incluez pas vous meme"
#endif

typedef enum {
	QUEUE_ACT_Test_Servo,
	QUEUE_ACT_AX12_POP_COLLECT_LEFT,
	QUEUE_ACT_AX12_POP_COLLECT_RIGHT,
	QUEUE_ACT_AX12_POP_DROP_LEFT,
	QUEUE_ACT_AX12_POP_DROP_RIGHT,
	NB_ACT
} QUEUE_act_e;
