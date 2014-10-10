/*
 *  Club Robot ESEO 2008 - 2014
 *
 *  $Id: config_global_vars_types.h 2554 2014-09-06 20:28:58Z aguilmet $
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
	//avec QUEUE devant, n'importe ou dans le code on sait que �a fait r�f�rence � un actionneur li� � queue.h/c
	QUEUE_ACT_AX12_Fruit,
	QUEUE_ACT_POMPE_Fruit,
	QUEUE_ACT_lancelauncher,
	QUEUE_ACT_AX12_Filet,
	QUEUE_ACT_AX12_Gache,
	QUEUE_ACT_Arm,
	QUEUE_ACT_AX12_Small_Arm,
	QUEUE_ACT_Pompe,
	QUEUE_ACT_Torch_Locker,
	QUEUE_ACT_Test_Servo,
	NB_ACT
} QUEUE_act_e;
