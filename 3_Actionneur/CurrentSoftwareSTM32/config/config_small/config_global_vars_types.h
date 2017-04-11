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

typedef enum {
	QUEUE_ACT_AX12_SMALL_BALL_BACK,
	QUEUE_ACT_AX12_SMALL_BALL_FRONT_LEFT,
	QUEUE_ACT_AX12_SMALL_BALL_FRONT_RIGHT,
	QUEUE_ACT_RX24_SMALL_CYLINDER_ARM,
	QUEUE_ACT_RX24_SMALL_CYLINDER_ELEVATOR,
	QUEUE_ACT_AX12_SMALL_CYLINDER_SLIDER,
	QUEUE_ACT_RX24_SMALL_CYLINDER_COLOR,
	QUEUE_ACT_AX12_SMALL_CYLINDER_DISPOSE,
	QUEUE_ACT_AX12_SMALL_CYLINDER_SLOPE,
	QUEUE_ACT_AX12_SMALL_CYLINDER_BALANCER,
	QUEUE_ACT_AX12_SMALL_CYLINDER_MULTIFONCTION,
	QUEUE_ACT_RX24_SMALL_MAGIC_ARM,
	QUEUE_ACT_RX24_SMALL_MAGIC_COLOR,
	QUEUE_ACT_AX12_SMALL_ORE,
	QUEUE_ACT_SMALL_POMPE_DISPOSE,
	QUEUE_ACT_SMALL_POMPE_PRISE,

	NB_ACT
} QUEUE_act_e;
