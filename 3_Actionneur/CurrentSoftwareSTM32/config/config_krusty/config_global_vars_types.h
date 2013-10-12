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
	QUEUE_ACT_BallLauncher,	//avec QUEUE devant, n'importe ou dans le code on sait que ça fait référence à un actionneur lié à queue.h/c
	QUEUE_ACT_Plate_Rotation,
	QUEUE_ACT_Plate_AX12_Plier,
	QUEUE_ACT_BallSorter,
	QUEUE_ACT_Lift_Left_Translation,
	QUEUE_ACT_Lift_Left_AX12_Plier,
	QUEUE_ACT_Lift_Right_Translation,
	QUEUE_ACT_Lift_Right_AX12_Plier,
	NB_ACT
} QUEUE_act_e;
