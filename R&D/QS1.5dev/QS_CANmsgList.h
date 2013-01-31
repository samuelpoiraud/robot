/*
 *	Club Robot ESEO 2009 - 2010
 *	PACMAN
 *
 *	Fichier : QS_CANmsgList.h
 *	Package : Qualité Soft
 *	Description : alias des sid des messages CAN
 *	Auteur : Jacen
 *	Version 20091111
 */

#ifndef QS_CANMSGLIST_H
	#define QS_CANMSGLIST_H
	#include "../QS/QS_CANmsgDoc.h"

	/*****************************************************************
	 *
	 *		Ci-apres suivent, dans l'ordre
	 *		- Les masques pour selectionner les messages
	 *		- Les messages d'avancement du match
	 *		- Les messages concerant la stratégie à adopter
	 *		- Les messages de position de notre robot
	 *		- Les messages de position du robot adverse
	 *		- Les messages concernant l'état du terrain
	 *		- Les commandes des actionneurs (hors Propulsion)
	 *		- Les informations des actionneurs (hors Propulsion)
	 *		- Les commandes de la Propulsion
	 *		- Les informations de la Propulsion
	 *		- Les messages de DEBUG
	 *
	 *****************************************************************/


	/****************************************************/	
	/* Masques pour selectionner les messages 			*/
	/****************************************************/	
	#define CAN_ESEO_MASK				0x780
	#define CAN_MATCH_PROGRESS_MASK		0x100
	#define CAN_OUR_CONFIG_MASK			0x200
	#define CAN_OUR_POSITION_MASK		0x300
	#define CAN_FOE_POSITION_MASK		0x380
	#define CAN_AREA_STATE_MASK			0x400
	#define CAN_ACT_CMD_MASK			0x500
	#define CAN_ACT_INFO_MASK			0x580
	#define CAN_PROP_CMD_MASK			0x600
	#define CAN_PROP_INFO_MASK			0x680
	#define CAN_DEBUG_MASK				0x700
	
	
	/****************************************************/	
	/* Messages d'avancement du match					*/
	/****************************************************/	
	#define CAN_MATCH_PROGRESS 			(0x001 | CAN_MATCH_PROGRESS_MASK)
	
	/****************************************************/	
	/* Messages concerant la stratégie à adopter		*/
	/****************************************************/	
	#define CAN_OUR_COLOR				(0x001 | CAN_OUR_CONFIG_MASK)
	
	/****************************************************/	
	/* Messages de position de notre robot				*/
	/****************************************************/	
	#define CAN_OUR_POSITION			(0x001 | CAN_OUR_POSITION_MASK)

	/****************************************************/	
	/* Messages de position du robot adverse			*/
	/****************************************************/	
	#define CAN_FOE_POSITION			(0x001 | CAN_FOE_POSITION_MASK)

	/****************************************************/	
	/* Messages concernant l'état du terrain			*/	
	/****************************************************/	
	
	/****************************************************/	
	/* Commandes des actionneurs (hors Propulsion)		*/
	/****************************************************/	
	
	/****************************************************/	
	/* Informations des actionneurs (hors Propulsion)	*/
	/****************************************************/	
	
	/****************************************************/	
	/* Commandes de la Propulsion						*/
	/****************************************************/	
	
	/****************************************************/	
	/* Informations de la Propulsion					*/
	/****************************************************/	
	
	/****************************************************/	
	/* Messages de DEBUG								*/
	/****************************************************/	
	#define CAN_GENERAL_DEBUG_MESSAGE		(0x001 | CAN_DEBUG_MASK)
	#define CAN_DEBUG_PROP_FICTIVE_POINT	(0x002 | CAN_DEBUG_MASK)


#endif	/* ndef QS_CANMSGLIST_H */
