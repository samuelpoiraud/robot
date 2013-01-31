/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi-Tech'
 *
 *  Fichier : QS_CANmsgDoc.h
 *  Package : Qualité Soft
 *  Description : descriptif des messages CAN
 *  Auteur : Jacen
 *  Version 20091111
 */
#if 0 // c'est que de la doc
#ifndef QS_CANMSGDOC_H
	#define QS_CANMSGDOC_H

	/****************************************************/	
	/* Messages d'avancement du match 					*/
	/****************************************************/	
	#define CAN_MATCH_PROGRESS
	
	/****************************************************/	
	/* Messages concerant la stratégie à adopter 		*/
	/****************************************************/	
	#define CAN_OUR_COLOR
	/*
		arguments :
		couleur de notre robot : Uint8 (QS_color_e)
	*/
	
	/****************************************************/	
	/* Messages de position de notre robot 				*/
	/****************************************************/	
	#define CAN_OUR_POSITION

	/****************************************************/	
	/* Messages de position du robot adverse 			*/
	/****************************************************/	
	#define CAN_FOE_POSITION

	/****************************************************/	
	/* Messages concernant l'état du terrain 			*/	
	/****************************************************/	
	
	/****************************************************/	
	/* Commandes des actionneurs (hors Propulsion) 		*/
	/****************************************************/	
	
	/****************************************************/	
	/* Informations des actionneurs (hors Propulsion) 	*/
	/****************************************************/	
	
	/****************************************************/	
	/* Commandes de la Propulsion 						*/
	/****************************************************/	
	
	/****************************************************/	
	/* Informations de la Propulsion 					*/
	/****************************************************/	
	
	/****************************************************/	
	/* Messages de DEBUG 								*/
	/****************************************************/	
	#define CAN_STRAT_DEBUG_MESSAGE
	/*
		Message utilisé par le code Strat pour le debug...
		arguments :
		non-spécifié
	*/
	
	#define CAN_DEBUG_PROP_FICTIVE_POINT
	/*
		Message utilisé par le code Prop pour le debug...
		arguments :
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
	*/



/*************************************************************************************
**************************************************************************************
**************************************************************************************
**************************************************************************************
**************************************************************************************
**************************************************************************************
**************************************************************************************
**************************************************************************************
**************************************************************************************
**************************************************************************************
*************************************************************************************/

/*****************************************************************
 *
 *		Messages echangés entre la carte Supervision
 *		et la carte P
 *
 *****************************************************************/

	/* Carte super vers carte P */
	#define SUPER_ASK_COLOR				0x400
	/* argument
		couleur de notre robot : (Uint8):(VERT=0, ROUGE =1)
	*/
	#define SUPER_ASK_CONFIG
	/* arguments :
		strategie 		: Uint8
		depot_zone_1	: Uint8 (bool_e)
		depot_zone_2	: Uint8 (bool_e)
		depot_zone_3	: Uint8 (bool_e)
		evitement		: Uint8 (bool_e)
		balises			: Uint8 (bool_e)
	*/

	/* Carte carte P vers super */
	#define SUPER_CONFIG_IS
	/* arguments :
		strategie 		: Uint8
		depot_zone_1	: Uint8 (bool_e)
		depot_zone_2	: Uint8 (bool_e)
		depot_zone_3	: Uint8 (bool_e)
		evitement		: Uint8 (bool_e)
		balises			: Uint8 (bool_e)
	*/


/******************************************************************
 *
 *		Petits mots doux echangés entre la carte Asser 
 *		et la carte P
 *
 *			typedef enum
 *			{
 *				ANY_WAY,						// Peu importe le sens de déplacement
 *				REAR,							// En arrière uniquement
 *				FORWARD						// En avant uniquement
 *			}ASSER_way_e;
 *
 *			typedef enum
 *			{
 *				FAST, SLOW
 *	 		}ASSER_speed_e;
 *****************************************************************/

  	/* Carte asser vers carte P */
  	#define CARTE_P_TRAJ_FINIE
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
	*/
	#define CARTE_P_ASSER_ERREUR
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
	*/
	#define CARTE_P_POSITION_ROBOT
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
	*/
  	#define CARTE_P_ROBOT_FREINE
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
	*/


	
/******************************************************************
 *
 *		Messages echangés entre la carte Actionneur1
 *		et la carte P
 *
 *****************************************************************/

#define LEFT			0
#define RIGHT			1
#define BOTH			2
#define CLOSED			3
#define OPENED			4
#define WIDELY_OPENED	5
#define IN				6
#define OUT				7
#define STORED_LEFT		8
#define STORED_RIGHT	9
#define BLOCKED			19
#define DO_NOT_UPDATE	255

	#define ACT1_DCM_STATE
	/* arguments :
		Actuator		: Uint8		(0:LEFT_LIFT/1:RIGHT_LIFT/2:FISH)
		#if Actuator == LIFT
			Lift Pos 	: Uint8		(0-10:LEVEL/19:BLOCKED)
		#elif Actuator == FISH
			Fish Pos 	: Uint8		(0:LEFT/1:RIGHT/8:STORED_LEFT/9:STORED_RIGHT19:BLOCKED)
		#endif
	*/

#endif	/* ndef QS_CANMSGLIST_H */
#endif	/* 0 */
