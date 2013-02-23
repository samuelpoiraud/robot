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
	#define DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT					0x710
	
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
	
	//modif amaury pour les 3position de calibrage initiale
    #define ASSER_CASE_CALIBRAGE_1               0x112
    #define ASSER_CASE_CALIBRAGE_2               0x113
    #define ASSER_CASE_CALIBRAGE_3               0x114
		

/*****************************************************************
 *
 *		Messages echangés entre les cartes Actionneurs
 *		et la carte Stratégie
 *
 *****************************************************************/

	//////////////// Results ////////////////
	//Définir un SID du type ACT_nom_actionneur_RESULT avec un nombre de type (STRAT_FILTER | (ACT_FILTER >> 4) | valeur)
	//avec valeur différente pour chaque message de la carte actionneur à la carte strat
	#define ACT_RESULT (STRAT_FILTER | (ACT_FILTER >> 4) | 0)
	//Dans data[0]: le sid actionneur && 0xFF, dans data[1]: une de ces valeurs:
		#define ACT_RESULT_DONE		0
		#define ACT_RESULT_FAILED	1
    /////////////////////////////////////////
	
	//////////////// AX12 ///////////////////
    #define ACT_AX12 (ACT_FILTER | 0)

		#define ACT_BALL_GRABBER_GO_UP      0x02
		#define ACT_BALL_GRABBER_GO_DOWN    0x03
		#define ACT_BALL_GRABBER_GO_TIDY    0x04

		#define ACT_HAMMER_GO_UP            0x05
		#define ACT_HAMMER_GO_DOWN          0x06
		#define ACT_HAMMER_GO_TIDY          0x07
    /////////////////////////////////////////

	////////////// LONGHAMMER ///////////////
	#define ACT_LONGHAMMER (ACT_FILTER | 1)	//0x10 peut être changé mais pas le reste (sinon le message n'est plus reçu par la carte actionneur par filtrage)
		//Paramètres de LONGHAMMER (dans data[0])
		#define ACT_LONGHAMMER_GO_UP   0    // Lever le bras
		#define ACT_LONGHAMMER_GO_DOWN 1    // Appuyer sur les bougies et les éteindres en descendant le bras
		#define ACT_LONGHAMMER_GO_PARK 2    // Ranger le bras pour diminuer le diamètre du robot
		#define ACT_LONGHAMMER_GO_STOP 3    // Arreter l'asservissement, en cas de problème par exemple, ne devrai pas servir en match.
		                                    //Le bras n'est plus controllé après ça, si la gravité existe toujours, il tombera.
	/////////////////////////////////////////
	
	////////////// BALLLAUNCHER /////////////
	#define ACT_BALLLAUNCHER (ACT_FILTER | 2)
		//Paramètres de BALLLAUNCHER (dans data[0])
		#define ACT_BALLLAUNCHER_ACTIVATE 0	//Activer le lanceur de balles, vitesse en tr/min dans data[1] et data[2], data[1] est le poids faible (type: Uint16 inférieur à 32000)
		#define ACT_BALLLAUNCHER_STOP     1	//Stoper le launceur de balles
	/////////////////////////////////////////

	////////////////// PLATE ////////////////
	#define ACT_PLATE (ACT_FILTER | 3)
		//Paramètres de PLATE (dans data[0]) (0x1x: Pince, 0x2x: Rotation bras)
		#define ACT_PLATE_PLIER_CLOSE           0x10	//Fermer la pince et serrer l'assiette'
		#define ACT_PLATE_PLIER_OPEN            0x11	//Ouvrir la pince et lacher l'assiette
		#define ACT_PLATE_ROTATE_HORIZONTALLY   0x20	//Amener le bras en position horizontale (pour prendre ou lacher une assiette par ex)
		#define ACT_PLATE_ROTATE_VERTICALLY     0x21	//Amener le bras en position verticale (pour vider une assiette ou réduire le périmêtre du robot)
		#define ACT_PLATE_PLIER_STOP            0x12	//Stopper l'asservissement de la pince
		#define ACT_PLATE_ROTATE_STOP           0x22	//Stopper l'asservissement du bras
	/////////////////////////////////////////

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
