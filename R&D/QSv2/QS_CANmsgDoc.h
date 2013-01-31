/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi-Tech'
 *
 *  Fichier : QS_CANmsgDoc.h
 *  Package : Qualité Soft
 *  Description : descriptif des messages CAN
 *  Auteur : Jacen
 *  Version 20090409
 */
#if 0 // c'est que de la doc
#ifndef QS_CANMSGDOC_H
	#define QS_CANMSGDOC_H

	/* Masque des cartes (des destinataires) */
	
  	/* Message pour tous */
  	#define BROADCAST_START
  	#define BROADCAST_STOP_ALL
	#define BROADCAST_COULEUR
	/* argument
		couleur de notre robot : (Uint8):(VERT=0, ROUGE =1)
	*/


	/* Message pour personne */
	#define DEBUG_CAN_MESSAGE
	
	#define DEBUG_ASSER_POINT_FICTIF
	/*
	Message utilisé par le code Asser pour le debug...
		argument
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
	*/
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


/************************************************************************************/

  	/* Carte P vers carte asser */

	#define ASSER_GO_POSITION
	/* arguments :
		X : Sint16 (mm)
		Y : Sint16 (mm)
		SENS : ASSER_way_e (Uint8)
		VITESSE : ASSER_speed_e (Uint8)
	*/
	#define ASSER_STOP
	/* pas d'argument */
  	#define ASSER_GO_ANGLE
	/* argument :
		Angle : Sint16 (RAD4096)
		VITESSE : ASSER_speed_e (Uint8)
	*/
	#define ASSER_MULTI
 	/* arguments :
		X : Sint16 (mm)
		Y : Sint16 (mm)
	*/
	#define ASSER_ANGLE_MULTI
	/* arguments :
		Angle : Sint16 (RAD4096)
		VITESSE : ASSER_speed_e (Uint8)
	*/
	#define ASSER_TELL_POSITION
	/* pas d'argument */
	#define CARTE_ASSER_FIN_ERREUR
	/* pas d'argument */
	#define ASSER_SET_POSITION
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
	*/
	#define ASSER_TYPE_ASSERVISSEMENT
	/* argument :
		asservissement en translation on (1)/off(0) : Uint8
		asservissement en rotation on (1)/off(0) : Uint8
		au demarrage du robot les deux asservissements sont actifs
	*/
	#define ASSER_RUSH_IN_THE_WALL
	/*
		SENS : ASSER_way_e (Uint8)
		asservissement en rotation on (1)/off(0) : Uint8
	*/
	#define ASSER_GO_RELATIVE_ANGLE
	/* argument :
		Angle : Sint16 (RAD4096)
		VITESSE : ASSER_speed_e (Uint8)
	*/

	#define ASSER_WARN_ANGLE
	/* argument :
		Angle : Sint16 (RAD4096)
	*/
		
	#define ASSER_WARN_X
	/* argument :
		x : Sint16 (mm)
	*/
	
	#define ASSER_WARN_Y
	/* argument :
		y : Sint16 (mm)
	*/

	#define ASSER_GO_RELATIVE_POSITION
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
	*/
	
	#define ASSER_GO_COURBE
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)		
		SENS : ASSER_way_e (Uint8) (sens obligatoire, soit avancer (1) soit reculer (2)... Si (0) -> avancer !)
		VITESSE : ASSER_speed_e (Uint8)		(0)RAPIDE,(1)LENTE,(2)TRES_LENTE,(3)EXTREMEMENT_LENTE
		Rapport de vitesses : 1 à 255 (Uint8)
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
	#define ACT1_CLAMP_STATE
	/* arguments :
		Clamp :	Uint8	(0:left clamp/1:right clamp)
		Clamp State :	Uint8	(3:CLOSED/4:OPENED/5:WIDELY OPENED/19:BLOCKED)
		Clamp Content : Uint8	(nombre de palets (0, 1 ou 2))
	*/
	
	#define ACT1_GOT
	/* arguments :
		Nombre de palets à gauche : Uint8
		Nombre de palets à droite : Uint8
	*/
	#define ACT1_DROPPED
	/* arguments :
		aucun
	*/
	#define ACT1_ASK_ROTATION
	/* arguments :
		Coté duquel faire la rotation : Uint8 (0:LEFT/1:RIGHT)
	*/


	/* Carte P vers Carte Actionneur 1*/
	#define	ACT1_CLAMP_SET_STATE
	/* arguments :
		Clamp : Uint8 ( 0:LEFT/1:RIGHT )
		State : Uint8 ( 3:CLOSED/4:OPENED/5:WIDELY OPENED)
	*/
	#define ACT1_FISH_SET_POS
	/* arguments :
		FISH Pos :		Uint8	(0:LEFT/1:RIGHT/8:STORED LEFT/9:STORED RIGHT)
	*/
	#define ACT1_LIFT_SET_LEVEL
	/* arguments :
		Lift : 	Uint8 ( 0:LEFT/1:RIGHT )
		Level : Uint8
	*/
	#define ACT1_GET
	/* arguments :
		Nombre de palets à recupérer : 	Uint8
	*/
	#define ACT1_DROP
	/* arguments :
		Side : 	Uint8 ( 0:LEFT/1:RIGHT/2:BOTH )
		Level : Uint8 (same for both side)
		Count:  Uint8 (0: 2 / 1: 4)
	*/
	#define ACT1_MADE_ROTATION
	/* arguments :
		aucun
	*/


/******************************************************************
 *
 *		Messages echangés entre la carte Actionneur1
 *		et la carte P
 *
 *****************************************************************/

	#define ACT2_LIFT_STATE
	/* arguments :
		Lift State :	Uint8	(0-10:LEVEL/16:IN TRANSLATION/19:BLOCKED)
	*/
	#define ACT2_BAR_STATE
	/* arguments :
		Bar Position :	Uint8	(5:IN/6:OUT/16:IN TRANSLATION/19:BLOCKED)
	*/
	#define ACT2_LINTEL_DROPED
	/* arguments :
		Number of Lintel in stock :	Uint8
	*/
	#define ACT2_GOT_LINTEL
	/* arguments :
		Number of Lintel in stock :	Uint8
	*/

	/* Carte P vers Carte Actionneur 2*/
	#define ACT2_LIFT_SET_LEVEL
	/* arguments :
		Level : Uint8
	*/
	#define ACT2_BAR_SET_STATE
	/* arguments :
		Bar Position :	Uint8	(5:IN/6:OUT)
	*/
	#define ACT2_DROP_LINTEL
	/* arguments :
		Level : Uint8
	*/
	#define ACT2_GET_LINTEL					
	/* arguments :
		aucun
	*/


/******************************************************************
 *
 *		Messages echangés entre la carte capteurs
 *		et la carte P
 *
 *****************************************************************/


	/* Carte P vers carte capteurs */
	#define SENSOR_RUN_SCAN
	/* arguments :
		Uint8 Mode : 
			0 => arrête le scan
			1 => commence la détection sur le côté, averti dès qu'il y a une détection sur le côté
			2 => commence la détection au milieu et retourne SENSOR_MIDDLE_SCAN_RESULT à la fin
			3 => commence la détection des palets au sol et retourne SENSOR_MAP_IS à la fin
	*/
	#define SENSOR_GET_VALUE
	/*
		Demande les distances des capteurs avant et arrière en cm
		Attend un message de retour SENSOR_SPOT
	*/
	#define SENSOR_REARM
	/*
		Demande un réarmement de la détection en continu de robot adverse devant ou derrière
	*/
	#define SENSOR_SET_TRESHOLD
	/* arguments :
		Uint8 seuil avant en cm
		Uint8 seuil arrière en cm
		Si valeur 0 : ne jamais prévenir d'une détection
		Si valeur 255 : ne pas changer de valeur
	*/
	#define SENSOR_GET_SIDE_VALUE
	/*
		Demande un retour d'information au niveau des capteurs sur le côté
		arguments :
		 Uint8 mode de retour : 
		  Si 0: retour SENSOR_SEE
		  Si 1: retour SENSOR_VALUE
	*/
	#define SENSOR_STOP
	/*
		Arrête toutes détections en cours sur le côté (continu ou échantionné) sauf le retour avant/arrière
	*/


	/* Carte capteurs vers carte P */
	#define SENSOR_SEE
	/* arguments :
		Uint8 hauteur en niveau de palets (de 0 à 9)
		Sint16 position x du robot si size = 5
		Sint16 position y du robot si size = 5
		Fonctionne lorsque le mode de scan est 0 : continu
	*/
	#define SENSOR_TOPO							// HAUTEUR_POS_0, ..., HAUTEUR_POS_7 | Mode 1 : échantillonné
	/* arguments :
		Uint8 hauteur pour chaque position sur une zone
		On considère 3 zones (la zone du milieu n'étant pas prise en compte ici)
		Les zones considérés ici sont de la taille de la zone de dépot 1, elles peuvent contenir 8 palets
		côte à côte et donc la carte capteurs scanne cette et renvoi la hauteur en divisant cette zone en 8
		Fonctionne lorsque le mode de scan est 1 : échantillonné
	*/
	#define SENSOR_VALUE						// DISTANCE_SENSOR_1, ..., DISTANCE_SENSOR_8 (cm)
	/* arguments :
		Uint8 distance évalué pour chaque capteur (les 8 du bas) en cm
	*/
	#define SENSOR_SPOT
	/* arguments :
		Uint8 distance du capteur avant en cm
		Uint8 distance du capteur arrière en cm
	*/
	#define SENSOR_MAP_IS
	/* arguments :
		Uint8	Map au sol détectée	- 255 si détection nulle
		Uint8	Map secondaire possible	(si 3 captures au lieu de 4) - 255 si aucune
	*/
	#define SENSOR_MIDDLE_SCAN_RESULT
	/* arguments :
		bool_e	renvoi si la construction est possible ou non
		Uint8 	niveau minimum auquel on peut poser
		Sint16	position x de dépot, correspond à la position du milieu de l'avant du linteau détecté
		Sint16	position y de dépot, correspond à la position du milieu de l'avant du linteau détecté
		Sint16	angle du linteau détecté
	*/

/*****************************************************************
 *
 *		Messages echangés entre la carte balise
 *		et la carte P
 *
 *****************************************************************/

	/* Carte P vers carte balises */
	#define BEACON_TELL_DISPENSER_POSITION

	/* Carte balises vers carte P */
	#define BEACON_POS
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
	*/
	#define BEACON_DISPENSER_POSITION_IS
	/*
		position distributeur : Uint8 	(0: proche des emplacement de depart robot
										/1: proche des emplacement de construction)
	*/

#endif	/* ndef QS_CANMSGLIST_H */
#endif	/* 0 */
