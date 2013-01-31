/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi-Tech'
 *
 *  Fichier : QS_CANmsgList.h
 *  Package : Qualité Soft
 *  Description : alias des sid des messages CAN
 *  Auteur : Jacen
 *  Version 20090426
 */

#ifndef QS_CANMSGLIST_H
	#define QS_CANMSGLIST_H
	#include "../QS/QS_CANmsgDoc.h"

	/* Masque des cartes (des destinataires) */
	#define ESEO_MASK					0x700
	#define BROADCAST_FILTER			0x000
	#define ASSER_FILTER				0x100
	#define P_FILTER					0x200
	#define ACT1_FILTER					0x300
	#define ACT2_FILTER					0x400
	#define SUPER_FILTER				0x500
	#define SENSOR_FILTER				0x600
	#define DEBUG_FILTER				0x700
	
	/* Message pour tous */
	#define BROADCAST_START	 			0x001
	#define BROADCAST_STOP_ALL 			0x002
	#define BROADCAST_COULEUR			0x003
	#define BROADCAST_POSITION_ROBOT	0x004

	/* Message pour personne */
	#define DEBUG_CAN_MESSAGE			0x742
	#define DEBUG_ASSER_POINT_FICTIF	0x701

/*****************************************************************
 *
 *		Messages echangés entre la carte Supervision
 *		et la carte P
 *
 *****************************************************************/

	/* Carte super vers carte P */
	#define SUPER_ASK_COLOR				0x200
	#define SUPER_ASK_CONFIG			0x201
	
	/* Carte P vers Super */
	#define SUPER_CONFIG_IS				0x500


/*****************************************************************
 *		Petits mots doux echangés entre la carte Asser 
 *		et la carte P
 *
 *****************************************************************/

	/* Carte asser vers carte P */
	#define CARTE_P_TRAJ_FINIE			0x210
	#define CARTE_P_ASSER_ERREUR		0x211
	#define CARTE_P_POSITION_ROBOT		BROADCAST_POSITION_ROBOT
	#define CARTE_P_ROBOT_FREINE		0x213

	/* Carte P vers carte asser */

	#define ASSER_GO_POSITION			0x100
	#define ASSER_STOP					0x101
	#define ASSER_GO_ANGLE				0x102
	#define ASSER_MULTI					0x103
	#define ASSER_ANGLE_MULTI			0x104
	#define ASSER_TELL_POSITION			0x105
 	#define CARTE_ASSER_FIN_ERREUR		0x106
	#define	ASSER_SET_POSITION			0x107
	#define ASSER_TYPE_ASSERVISSEMENT	0x108
	#define ASSER_RUSH_IN_THE_WALL		0x109
	#define ASSER_GO_RELATIVE_ANGLE		0x10A
	#define ASSER_CALIBRATION			0x10B
	#define ASSER_WARN_ANGLE			0x10C	
	#define ASSER_WARN_X				0x10D
	#define ASSER_WARN_Y				0x10E
	#define ASSER_GO_RELATIVE_POSITION	0x10F
	#define ASSER_GO_COURBE				0x110
	
/*****************************************************************
 *
 *		Messages echangés entre les cartes Actionneurs
 *		et la carte P
 *
 *****************************************************************/


	/*Carte Actionneur 1 vers carteP */
	#define ACT1_DCM_STATE				0x220
	#define ACT1_CLAMP_STATE			0x221
	#define ACT1_GOT					0x222
	#define ACT1_DROPPED				0x223
	#define ACT1_ASK_ROTATION			0x224
	
	/* Carte P vers Carte Actionneur 1*/
	#define	ACT1_CLAMP_SET_STATE		0x300
	#define ACT1_FISH_SET_POS			0x301
	#define ACT1_LIFT_SET_LEVEL			0x302
	#define ACT1_GET					0x303
	#define ACT1_DROP					0x342
	#define ACT1_MADE_ROTATION			0x333

	/*Carte Actionneur 2 vers carteP */
	#define ACT2_LIFT_STATE				0x230
	#define ACT2_BAR_STATE				0x231
	#define ACT2_LINTEL_DROPED			0x232
	#define ACT2_GOT_LINTEL				0x233

	/* Carte P vers Carte Actionneur 2*/
	#define ACT2_LIFT_SET_LEVEL			0x400
	#define ACT2_BAR_SET_STATE			0x401
	#define ACT2_DROP_LINTEL			0x402
	#define ACT2_GET_LINTEL				0x403
	#define ACT2_PREPARE_DROP			0x405


/*****************************************************************
 *
 *		Messages echangés entre la carte capteurs
 *		et la carte P
 *
 *****************************************************************/

	/* Carte capteurs vers carte P */
	#define SENSOR_SEE						0x240	// HAUTEUR | Mode 0 : en continu
	#define SENSOR_TOPO						0x241	// HAUTEUR_POS_0, ..., HAUTEUR_POS_7 | Mode 1 : échantillonné
	#define SENSOR_VALUE					0x242	// DISTANCE_SENSOR_1, ..., DISTANCE_SENSOR_8 (cm)
	#define SENSOR_SPOT						0x243	// DISTANCE_AVANT, DISTANCE_ARRIERE (cm)
	#define SENSOR_MAP_IS					0x244
	#define SENSOR_MIDDLE_SCAN_RESULT		0x245

	/* Carte P vers carte capteurs */
	#define SENSOR_RUN_SCAN					0x600	// MODE
	#define SENSOR_GET_VALUE				0x601
	#define SENSOR_REARM					0x602
	#define SENSOR_SET_TRESHOLD				0x603	// SEUIL AVANT, SEUIL ARRIERE
	#define SENSOR_GET_SIDE_VALUE			0x604	// MODE RETOUR (0: retour SENSOR_SEE, 1: retour SENSOR_VALUE)
	#define SENSOR_STOP						0x605



/*****************************************************************
 *
 *		Messages echangés entre la carte balise
 *		et la carte P
 *
 *****************************************************************/

	/* Carte P vers carte balises */
	#define BEACON_TELL_DISPENSER_POSITION	0x510
	/* Carte balises vers carte P */
	#define BEACON_POS						0x250
	#define BEACON_DISPENSER_POSITION_IS	0x251

#endif	/* ndef QS_CANMSGLIST_H */
