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
	QUEUE_ACT_AX12_PINCE_GAUCHE,
	QUEUE_ACT_AX12_PINCE_DROITE,
	QUEUE_ACT_AX12_CLAP,
	QUEUE_ACT_AX12_PINCE_DEVANT,
	QUEUE_ACT_AX12_POP_DROP_LEFT_WOOD,
	QUEUE_ACT_AX12_POP_DROP_RIGHT_WOOD,
	NB_ACT
} QUEUE_act_e;
