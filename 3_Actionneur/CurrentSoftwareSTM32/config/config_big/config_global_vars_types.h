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
	QUEUE_ACT_Test_Servo,
	QUEUE_ACT_AX12_POP_COLLECT_LEFT,
	QUEUE_ACT_AX12_POP_COLLECT_RIGHT,
	QUEUE_ACT_AX12_POP_DROP_LEFT,
	QUEUE_ACT_AX12_POP_DROP_RIGHT,
	QUEUE_ACT_AX12_BACK_SPOT_RIGHT,
	QUEUE_ACT_AX12_BACK_SPOT_LEFT,
	QUEUE_ACT_SPOT_POMPE_RIGHT,
	QUEUE_ACT_SPOT_POMPE_LEFT,
	QUEUE_ACT_AX12_CARPET_LAUNCHER_RIGHT,
	QUEUE_ACT_AX12_CARPET_LAUNCHER_LEFT,
	QUEUE_ACT_ELEVATOR,
	QUEUE_ACT_RX24_PINCEMI_LEFT,
	QUEUE_ACT_RX24_PINCEMI_RIGHT,
	NB_ACT
} QUEUE_act_e;
