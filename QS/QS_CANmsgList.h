/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi-Tech'
 *
 *  Fichier : QS_CANmsgList.h
 *  Package : Qualité Soft
 *  Description : alias des sid des messages CAN
 *  Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20101216
 */

#ifndef QS_CANMSGLIST_H
	#define QS_CANMSGLIST_H
	#include "../QS/QS_CANmsgDoc.h"

	/* Masque des cartes (des destinataires) */
	#define MASK_BITS					0x700
	#define BROADCAST_FILTER			0x000
	#define ASSER_FILTER				0x100
	#define STRAT_FILTER				0x200
	#define ACT_FILTER					0x300
	#define BALISE_FILTER				0x400
	#define SUPER_FILTER				0x500
	#define DEBUG_FILTER				0x700
	
	/* Message pour tous */
	#define BROADCAST_START	 			0x001
	#define BROADCAST_STOP_ALL 			0x002
	#define BROADCAST_COULEUR			0x003
	#define BROADCAST_POSITION_ROBOT	0x004

	/* Message pour personne */
	#define DEBUG_CARTE_P				0x742
	#define DEBUG_FOE_POS				0x748
	#define DEBUG_ELEMENT_UPDATED		0x749
	
	#define DEBUG_ASSER_POINT_FICTIF								0x701
	#define DEBUG_PROPULSION_REGLAGE_COEF_ODOMETRIE_SYM 			0x702
	#define DEBUG_PROPULSION_REGLAGE_COEF_ODOMETRIE_ROTATION 		0x703
	#define DEBUG_PROPULSION_REGLAGE_COEF_ODOMETRIE_TRANSLATION 	0x704
	#define DEBUG_PROPULSION_REGLAGE_COEF_ODOMETRIE_CENTRIFUGE 		0x705
	#define DEBUG_PROPULSION_REGLAGE_COEF_KP_ROTATION 				0x706	
	#define DEBUG_PROPULSION_REGLAGE_COEF_KD_ROTATION 				0x707
	#define DEBUG_PROPULSION_REGLAGE_COEF_KP_TRANSLATION			0x708
	#define DEBUG_PROPULSION_REGLAGE_COEF_KD_TRANSLATION			0x709
	#define DEBUG_PROPULSION_REGLAGE_COEF_KV_TRANSLATION			0x70A
	#define DEBUG_PROPULSION_REGLAGE_COEF_KV_ROTATION				0x70B
	
	/* Message de l'utilisateur vers Super */
	#define SUPER_EEPROM_RESET			0x770
	#define SUPER_EEPROM_PRINT_MATCH	0x777

	#define SUPER_RTC_GET				(0x780)	
	#define SUPER_RTC_SET				(0x781)
	#define	SUPER_RTC_TIME				(0x782)
	

 /*****************************************************************
 *
 *		Messages echangés entre la carte Supervision
 *		et les cartes stratégie, actionneur et propulsion
 *
 *****************************************************************/

	/* Carte super vers carte stratégie */
	#define SUPER_ASK_STRAT_SELFTEST	0x200
	#define SUPER_ASK_CONFIG			0x201
	
	/* Carte super vers carte actionneur */
	#define SUPER_ASK_ACT_SELFTEST		0x300
	
	/* Carte super vers carte propulsion */
	#define SUPER_ASK_ASSER_SELFTEST	0x100
	
	/* Carte super vers carte balise */
	#define SUPER_ASK_BEACON_SELFTEST	0x400
	
	/* Carte stratégie vers Super */
	#define STRAT_SELFTEST				0x500
	#define SUPER_CONFIG_IS				0x501
	
	/* Carte actionneur vers Super */
	#define ACT_SELFTEST 				0x502
	
	/* Carte propulsion vers Super */
	#define ASSER_SELFTEST 				0x503
	
	/* Carte balise vers Super */
	#define BEACON_IR_SELFTEST 			0x504
	#define BEACON_US_SELFTEST 			0x505
	
	

/*****************************************************************
 *		Petits mots doux echangés entre la carte propulsion
 *		et la carte stratégie
 *
 *****************************************************************/

	/* carte propulsion vers carte stratégie */
	#define CARTE_P_TRAJ_FINIE			0x210
	#define CARTE_P_ASSER_ERREUR		0x211
	#define CARTE_P_POSITION_ROBOT		BROADCAST_POSITION_ROBOT
	#define CARTE_P_ROBOT_FREINE		0x213
	#define CARTE_P_ROBOT_CALIBRE		0x214

	/* carte stratégie vers carte propulsion */
	#define ASSER_GO_POSITION					0x155
	#define ASSER_GO_ANGLE						0x177
	#define ASSER_SEND_PERIODICALLY_POSITION	0x188
	#define ASSER_STOP							0x101
	#define ASSER_TELL_POSITION					0x105
 	#define CARTE_ASSER_FIN_ERREUR				0x106
	#define	ASSER_SET_POSITION					0x107
	#define ASSER_TYPE_ASSERVISSEMENT			0x108
	#define ASSER_RUSH_IN_THE_WALL				0x109
	#define ASSER_CALIBRATION					0x10B
	#define ASSER_WARN_ANGLE					0x10C
	#define ASSER_WARN_X						0x10D
	#define ASSER_WARN_Y						0x10E
	#define ASSER_JOYSTICK 						0x111

/*****************************************************************
 *
 *		Messages echangés entre les cartes Actionneurs
 *		et la carte Stratégie
 *
 *****************************************************************/

	
    #define ACT_AX12 0x300

	//Paramètres de ACT_AX12 dans data[0]:
    #define ACT_BALL_GRABBER_GO_UP      1
    #define ACT_BALL_GRABBER_GO_DOWN    2
    #define ACT_BALL_GRABBER_GO_TIDY    3

    #define ACT_HAMMER_GO_UP            4
    #define ACT_HAMMER_GO_DOWN          5
    #define ACT_HAMMER_GO_TIDY          6
    //////////////////////////////////////

/*****************************************************************
 *
 *		Messages echangés entre la carte balise
 *		et la carte stratégie
 *
 *****************************************************************/

	/* carte stratégie vers carte balises */
	#define BEACON_ENABLE_PERIODIC_SENDING	0x410
	#define BEACON_DISABLE_PERIODIC_SENDING	0x411
	
	/* Carte balises vers carte stratégie */
	#define BEACON_ADVERSARY_POSITION_IR					0x250	//Balise InfraRouge
	#define BEACON_ADVERSARY_POSITION_US					0x251	//Balise UltraSon
	#define BEACON_ADVERSARY_POSITION_IR_ARROUND_AREA		0x252	//Balises terrain avec réception InfraRouge
	#define BEACON_ADVERSARY_POSITION_US_ARROUND_AREA		0x253	//Balises terrain avec réception UltraSon

/*****************************************************************
 *
 *		Messages echangés par liaison XBee
 *		avec la carte supervision ou balise mère...
 *
 *****************************************************************/
 	#define XBEE_PING	0x516	//PING = pInG = p1n6
 	#define XBEE_PONG	0x506	//PONG = pOnG = p0n6
 	
#endif	/* ndef QS_CANMSGLIST_H */
