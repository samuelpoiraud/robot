/*
 *  Club Robot ESEO 2008 - 2009
 *  Archi-Tech'
 *
 *  Fichier : QS_CANmsgList.h
 *  Package : Qualit� Soft
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

	#define XBEE_FILTER					0x5A0	//Seuls les messages commencant par 5A seront transmis par XBee !
	//PS : les messages PING et PONG ne sont pas soumis � cette r�glementation car envoy�s par le fichier QS_CAN_OVER_XBEE
	#define STRAT_XBEE_FILTER			0x2A0	//Un message re�u 5Ax par XBEE sera propag� 2Ax vers la strat du robot local.
/*****************************************************************
 *
 *		Messages echang�s entre les cartes strat�gies des
 *		deux robots via le Xbee: TESTS
 *			(les messages vont par pair)
 *****************************************************************/
/*	Envoi d'update des elements de l'environnement	*/
	//Envois
	#define XBEE_ENV_UPDATE					0x5A1
	#define XBEE_START_MATCH				0x5A4	//Ce message est envoy� pour lancer le match de l'autre robot
	#define XBEE_PING_OTHER_STRAT			0x5A6	//Ce message est envoy� pour pinger l'autre carte strat�gie
	#define XBEE_PONG_OTHER_STRAT			0x5A7	//Ce message est envoy� en r�ponse d'un ping vers l'autre carte strat�gie

	//Reception: La super se charge de transformer le message recu par xbee et qui commence par le sid precedent
	#define STRAT_ELTS_UPDATE				0x2A1
	#define STRAT_START_MATCH				0x2A4	//Ce message est envoy� pour demander � une carte S de lancer le match
	#define XBEE_PING_FROM_OTHER_STRAT		0x2A6	//Ce message est re�u par la strat, en provenance de l'autre strat
	#define XBEE_PONG_FROM_OTHER_STRAT		0x2A7	//Ce message est re�u en r�ponse d'un ping en provenance de l'autre carte strat�gie

 /*****************************************************************
 *
 *		Messages echang�s entre la carte Supervision
 *		et les cartes strat�gie, actionneur et propulsion
 *
 *****************************************************************/

	/* Carte super vers carte strat�gie */
	#define SUPER_ASK_STRAT_SELFTEST	0x200
	#define SUPER_ASK_CONFIG			0x201
	
	/* Carte super vers carte actionneur */
	#define SUPER_ASK_ACT_SELFTEST		0x300
	
	/* Carte super vers carte propulsion */
	#define SUPER_ASK_ASSER_SELFTEST	0x100
	
	/* Carte super vers carte balise */
	#define SUPER_ASK_BEACON_SELFTEST	0x400
	
	/* Carte strat�gie vers Super */
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
 *		Petits mots doux echang�s entre la carte propulsion
 *		et la carte strat�gie
 *
 *****************************************************************/

	/* carte propulsion vers carte strat�gie */
	#define CARTE_P_TRAJ_FINIE			0x210
	#define CARTE_P_ASSER_ERREUR		0x211
	#define CARTE_P_POSITION_ROBOT		BROADCAST_POSITION_ROBOT
	#define CARTE_P_ROBOT_FREINE		0x213
	#define CARTE_P_ROBOT_CALIBRE		0x214

	/* carte strat�gie vers carte propulsion */
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
		typedef enum
		{
			ASSER_CALIBRATION_SQUARE_0	= 0,	//Near gifts
			ASSER_CALIBRATION_SQUARE_1,
			ASSER_CALIBRATION_SQUARE_2,
			ASSER_CALIBRATION_SQUARE_3,
			ASSER_CALIBRATION_SQUARE_4			//Near cake
		}calibration_square_e;
		
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
 *		Messages echang�s entre les cartes Actionneurs
 *		et la carte Strat�gie
 *
 *****************************************************************/

	//////////////// Results ////////////////
	//D�finir un SID du type ACT_nom_actionneur_RESULT avec un nombre de type (STRAT_FILTER | (ACT_FILTER >> 4) | valeur)
	//avec valeur diff�rente pour chaque message de la carte actionneur � la carte strat
	#define ACT_RESULT (STRAT_FILTER | (ACT_FILTER >> 4) | 0)
	//Dans data[0]: le sid actionneur & 0xFF (on ne garde que l'octet de poid faible, l'octet de poids fort contient le filtre et on en a pas besoin)
	//Dans data[1]: la commande dont en envoi le r�sultat. Par ex: ACT_BALLLAUNCHER_ACTIVATE
	//Dans data[2]: �tat de la commande, une de ces valeurs:
		#define ACT_RESULT_DONE        0	//Tout s'est bien pass�
		#define ACT_RESULT_FAILED      1	//La commande s'est mal pass� et on ne sait pas dans quel �tat est l'actionneur (par: les capteurs ne fonctionnent plus)
		#define ACT_RESULT_NOT_HANDLED 2	//La commande ne s'est pas effectu� correctement et on sait que l'actionneur n'a pas boug� (ou quand la commande a �t� ignor�e)
	//Dans data[3]: la raison du probl�me (si pas DONE), une de ces valeurs:
		#define ACT_RESULT_ERROR_OK           0	//Quand pas d'erreur
		#define ACT_RESULT_ERROR_TIMEOUT      1	//Il y a eu timeout, par ex l'asservissement prend trop de temps
		#define ACT_RESULT_ERROR_OTHER        2	//La commande �tait li� � une autre qui ne s'est pas effectu� correctement, utilis� avec ACT_RESULT_NOT_HANDLED
		#define ACT_RESULT_ERROR_NOT_HERE     3	//L'actionneur ou le capteur ne r�pond plus (on le sait par un autre moyen que le timeout, par exemple l'AX12 ne renvoie plus d'info apr�s l'envoi d'une commande.)
		#define ACT_RESULT_ERROR_LOGIC        4	//Erreur de logique interne � la carte actionneur, probablement une erreur de codage (par exemple un �tat qui n'aurait jamais d� arriv�)
		#define ACT_RESULT_ERROR_NO_RESOURCES 5 //La carte n'a pas assez de resource pour g�rer la commande. Commande � renvoyer plus tard.
			//Ajoutez-en si n�cessaire
		#define ACT_RESULT_ERROR_UNKNOWN      255	//Erreur inconnue ou qui ne correspond pas aux pr�c�dentes.
    /////////////////////////////////////////

// Code des SID des messages: 0x30x = message pour Tiny, 0x31x = message pour Krusty.
// Le SID 0x300 est reserv� pour le self_test
/* Messages pour Tiny */
	//////////////// HAMMER /////////////////
	#define ACT_HAMMER (ACT_FILTER | 0x01)    //0x01 peut �tre chang� mais pas le reste (sinon le message n'est plus re�u par la carte actionneur par filtrage)
		//Param�tres de HAMMER (dans data[0])
		#define ACT_HAMMER_MOVE_TO     0   // Changer la position du bras, angle en degr� dans data[1] (poids faible) et data[2]. 90� = bras vertical, 0� = bras rentr�
		#define ACT_HAMMER_STOP        1   // Arreter l'asservissement, en cas de probl�me par exemple, ne devrai pas servir en match.
		                                   //Le bras n'est plus controll� apr�s �a, si la gravit� existe toujours, il tombera.
	/////////////////////////////////////////

	////////////// LONGHAMMER ///////////////
	#define ACT_LONGHAMMER (ACT_FILTER | 0x02)
		//Param�tres de LONGHAMMER (dans data[0])
		#define ACT_LONGHAMMER_GO_UP   0    // Lever le bras
		#define ACT_LONGHAMMER_GO_DOWN 1    // Appuyer sur les bougies et les �teindres en descendant le bras
		#define ACT_LONGHAMMER_GO_PARK 2    // Ranger le bras pour diminuer le diam�tre du robot
		#define ACT_LONGHAMMER_GO_STOP 3    // Arreter l'asservissement, en cas de probl�me par exemple, ne devrai pas servir en match.
		                                    //Le bras n'est plus controll� apr�s �a, si la gravit� existe toujours, il tombera.
	/////////////////////////////////////////

	///////////// BALLINFLATER //////////////
	#define ACT_BALLINFLATER (ACT_FILTER | 0x03)
		//Param�tres de LONGHAMMER (dans data[0])
		#define ACT_BALLINFLATER_START 0    // Gonfler le ballon pendant data[1] secondes (Uint8). Le message de retour n'attend pas la fin du gonflage.
		#define ACT_BALLINFLATER_STOP  1    // Stopper le gonflage
	/////////////////////////////////////////


	///////////// CANDLECOLOR ///////////////
	#define ACT_CANDLECOLOR (ACT_FILTER | 0x04)
		//Param�tres de CANDLECOLOR (dans data[0])
		#define ACT_CANDLECOLOR_GET_LOW    0  //Bouge le capteur et r�cup�re la couleur d'une bougie � l'�tage bas
		#define ACT_CANDLECOLOR_GET_HIGH   1  //Bouge le capteur et r�cup�re la couleur d'une bougie � l'�tage haut

	#define ACT_CANDLECOLOR_RESULT (STRAT_FILTER | (ACT_FILTER >> 4) | 0x1)
		//Couleur detect�e dans data[0]:
		#define ACT_CANDLECOLOR_COLOR_BLUE   0  //Couleur bleue detect�e
		#define ACT_CANDLECOLOR_COLOR_RED    1  //Couleur rouge detect�e
		#define ACT_CANDLECOLOR_COLOR_YELLOW 2  //Couleur jaune detect�e
		#define ACT_CANDLECOLOR_COLOR_WHITE  3  //Couleur blanche detect�e
		#define ACT_CANDLECOLOR_COLOR_OTHER  4  //Pas de couleur parmi les pr�c�dentes detect�e
	/////////////////////////////////////////

/* Messages pour Krusty */
	////////////// BALLLAUNCHER /////////////
	#define ACT_BALLLAUNCHER (ACT_FILTER | 0x11)
		//Param�tres de BALLLAUNCHER (dans data[0])
		#define ACT_BALLLAUNCHER_ACTIVATE 0	//Activer le lanceur de balles, vitesse en tr/min dans data[1] et data[2], data[1] est le poids faible (type: Uint16 inf�rieur � 32000).
		#define ACT_BALLLAUNCHER_STOP     1	//Stoper le launceur de balles
	/////////////////////////////////////////

	////////////////// PLATE ////////////////
	#define ACT_PLATE (ACT_FILTER | 0x12)
		//Param�tres de PLATE (dans data[0]) (0x1x: Pince, 0x2x: Rotation bras)
		#define ACT_PLATE_PLIER_CLOSE           0x10	//Fermer la pince et serrer l'assiette
		#define ACT_PLATE_PLIER_OPEN            0x11	//Ouvrir la pince et lacher l'assiette
		#define ACT_PLATE_PLIER_STOP            0x1F	//Stopper l'asservissement de la pince. Peut servir � diminer la conso courant de l'AX12, la pince en elle m�me ne bougera pas (sauf si il y a une assiette dans la pince ...)

		#define ACT_PLATE_ROTATE_HORIZONTALLY   0x20	//Amener le bras en position horizontale (pour prendre ou lacher une assiette par ex)
		#define ACT_PLATE_ROTATE_PREPARE        0x21	//Amener le bras en position interm�diaire (45�) pour pr�parer un mouvement vers l'horizontale ou verticale
		#define ACT_PLATE_ROTATE_VERTICALLY     0x22	//Amener le bras en position verticale. Ferme la pince si elle ne l'est pas avant d'effectuer le mouvement (meca oblige) (pour vider une assiette ou r�duire le p�rim�tre du robot)
		#define ACT_PLATE_ROTATE_STOP           0x2F	//Stopper l'asservissement du bras. A �viter, dans les virages il ne faudrait pas que l'actionneur tombe (m�me si gros reducteur ...)
	/////////////////////////////////////////

	/////////////// BALLSORTER //////////////
	#define ACT_BALLSORTER (ACT_FILTER | 0x13)
		//Param�tres de BALLSORTER (dans data[0])
		#define ACT_BALLSORTER_TAKE_NEXT_CHERRY 0    //Ejecter la cerise et en prendre une autre pour la detecter

	#define ACT_BALLSORTER_RESULT (STRAT_FILTER | (ACT_FILTER >> 4) | 0xA)
		//R�sultat de la detection de la cerise dans data[0]:
		#define ACT_BALLSORTER_WHITE_CHERRY     0
		#define ACT_BALLSORTER_NO_CHERRY        1
	/////////////////////////////////////////

	////////////////// LIFT /////////////////
	#define ACT_LIFT_RIGHT (ACT_FILTER | 0x14)
	#define ACT_LIFT_LEFT (ACT_FILTER | 0x15)
		//Param�tres de LIFT (dans data[0]) (0x1x: Pince, 0x2x: mouvement ascenseur)
		#define ACT_LIFT_PLIER_OPEN            0x10	//Ouvrir la pince et lacher le verre
		#define ACT_LIFT_PLIER_CLOSE           0x11	//Fermer la pince et serrer le verre
		#define ACT_LIFT_PLIER_STOP            0x1F	//Stopper l'asservissement de la pince. Peut servir � diminer la conso courant de l'AX12, la pince en elle m�me ne bougera pas (sauf si il y a une assiette dans la pince ...)

		#define ACT_LIFT_GO_UP                 0x20	//Amener l'ascenseur en position haute.
		#define ACT_LIFT_GO_MID                0x21	//Amener l'ascenseur en position interm�diaire pour pr�parer un mouvement vers haut ou bas.
		#define ACT_LIFT_GO_DOWN               0x22	//Amener l'ascenseur en position basse.
		#define ACT_LIFT_STOP                  0x2F	//Stopper l'asservissement de l'ascenseur.
	/////////////////////////////////////////

/*****************************************************************
 *
 *		Messages echang�s entre la carte balise
 *		et la carte strat�gie
 *
 *****************************************************************/

	/* carte strat�gie vers carte balises */
	#define BEACON_ENABLE_PERIODIC_SENDING	0x410
	#define BEACON_DISABLE_PERIODIC_SENDING	0x411
	
	/* Carte balises vers carte strat�gie */
	#define BEACON_ADVERSARY_POSITION_IR					0x250	//Balise InfraRouge
	#define BEACON_ADVERSARY_POSITION_US					0x251	//Balise UltraSon
	#define BEACON_ADVERSARY_POSITION_IR_ARROUND_AREA		0x252	//Balises terrain avec r�ception InfraRouge
	#define BEACON_ADVERSARY_POSITION_US_ARROUND_AREA		0x253	//Balises terrain avec r�ception UltraSon

/*****************************************************************
 *
 *		Messages echang�s par liaison XBee
 *		avec la carte supervision ou balise m�re...
 *
 *****************************************************************/
 	#define XBEE_PING	0x516	//PING = pInG = p1n6
 	#define XBEE_PONG	0x506	//PONG = pOnG = p0n6
 	
#endif	/* ndef QS_CANMSGLIST_H */
