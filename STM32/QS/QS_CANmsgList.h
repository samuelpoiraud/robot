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
	#include "QS_CANmsgDoc.h"

	/* Masque des cartes (des destinataires) */
	#define MASK_BITS					0x700
	#define BROADCAST_FILTER			0x000
	#define ASSER_FILTER				0x100
	#define STRAT_FILTER				0x200
	#define ACT_FILTER					0x300
	#define BALISE_FILTER				0x400
	#define DEBUG_FILTER				0x700

	//Ces messages ne sont pas destinés à voyager sur les bus CAN des robot.
	#define XBEE_FILTER					0x500

	/* Message pour tous */
	#define BROADCAST_START	 			0x001
	#define BROADCAST_STOP_ALL 			0x002
	#define BROADCAST_COULEUR			0x003
	#define BROADCAST_POSITION_ROBOT	0x004
		#define WARNING_NO					(0b00000000)
		#define WARNING_TIMER				(0b00000001)
		#define WARNING_TRANSLATION			(0b00000010)
		#define WARNING_ROTATION			(0b00000100)
		#define WARNING_REACH_X				(0b00001000)		//Nous venons d'atteindre une position en X pour laquelle on nous a demandé une surveillance.
		#define WARNING_REACH_Y				(0b00010000)		//Nous venons d'atteindre une position en Y pour laquelle on nous a demandé une surveillance.
		#define WARNING_REACH_TETA			(0b00100000)		//Nous venons d'atteindre une position en Teta pour laquelle on nous a demandé une surveillance.
		#define WARNING_NEW_TRAJECTORY		(0b01000000)		//Changement de trajectoire (attention, cela inclue les trajectoires préalables ajoutées en propulsion...)
	#define BROADCAST_ALIM				0x005		// Message envoyé lors d'un changement d'état de l'alimentation 12/24V
		#define ALIM_OFF					(0b00000000)
		#define ALIM_ON						(0b00000001)
	/* Message pour personne */
	#define DEBUG_CARTE_P				0x742
	#define DEBUG_FOE_POS				0x748
	#define DEBUG_ELEMENT_UPDATED		0x749
	#define DEBUG_BEACON_US1_ERROR      0x750  //Envoyé par la strat
	#define DEBUG_BEACON_US2_ERROR      0x751  //Envoyé par la strat
	#define DEBUG_BEACON_US_POWER       0x752  //Enovyé par la balise receptrice US (mais non activé actuellement)
	#define IR_ERROR_RESULT				0x753	//Message de la super pour l'EEPROM -> compteur des erreurs de balise
	#define US_ERROR_RESULT				0x754	//Message de la super pour l'EEPROM -> compteur des erreurs de balise
	#define DEBUG_FOE_REASON			0x755	//Message de debug qui explique la raison d 'un evitement
	#define DEBUG_US_NOT_RELIABLE		0x756	//Message qui indique que on ne fait plus confiance aux ultrasons

	#define DEBUG_STRAT_STATE_CHANGED	0x760  //Envoyé par la strat quand un état change d'une machine à état
		//Paramètres: data[0]:data[1] = ID d'une machine à état (data[0] le poids fort), data[2] = old_state, data[3] = new_state, data suivant: paramètres divers


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
	#define SUPER_EEPROM_RESET				0x770
	#define SUPER_EEPROM_TEST_ALL_MEMORY	0x771
	#define SUPER_EEPROM_PRINT_ALL_MEMORY	0x772
	#define SUPER_EEPROM_PRINT_MATCH		0x777

	#define DEBUG_RTC_GET				(0x780)
	#define DEBUG_RTC_SET				(0x781)
					/*	GET et SET :
						Uint8 secondes
						Uint8 minutes
						Uint8 hours
						Uint8 day
						Uint8 date
						Uint8 months
						Uint8 year	(11 pour 2011)
					*/
	#define	DEBUG_RTC_TIME				(0x782)

	//message pour debug strategie

	#define DEBUG_DETECT_FOE			0x799	//Déclenchement manuel d'un évitement.
/*****************************************************************
 *
 *		Messages echangés entre les cartes stratégies des
 *		deux robots via le Xbee
 *
 *****************************************************************/
/*	Envoi d'update des elements de l'environnement	*/
	//Envois
	#define XBEE_START_MATCH				0x5A4	//Ce message est envoyé pour lancer le match de l'autre robot
	#define XBEE_PING						0x516	//PING = pInG = p1n6, Ce message est envoyé pour pinger l'autre carte stratégie
	#define XBEE_PONG						0x506	//PONG = pOnG = p0n6, Ce message est envoyé en réponse d'un ping vers l'autre carte stratégie
	#define XBEE_ZONE_COMMAND				0x5AA	//Effectue une demande lié au zones (un SID pour toute la gestion des zones comme ça)
	//Commande dans data[0]:
		#define XBEE_ZONE_TRY_LOCK       0	//Dans data[1]: la zone, type: map_zone_e. La réponse de l'autre robot sera envoyé avec XBEE_ZONE_LOCK_RESULT
		#define XBEE_ZONE_LOCK_RESULT    1	//Dans data[1]: la zone concernée, data[2]: TRUE/FALSE suivant si le verouillage à été accepté ou non
		#define XBEE_ZONE_UNLOCK         2  //Dans data[1]: la zone libérée. Libère une zone qui a été verouillée


 /*****************************************************************
 *
 *		Messages echangés entre la carte Supervision
 *		et les cartes stratégie, actionneur et propulsion
 *
 *****************************************************************/


	/* Carte super vers carte actionneur */
	#define ACT_DO_SELFTEST		0x300
	#define ACT_PING			0x316	//16 = IG = PING

	/* Carte super vers carte propulsion */

	#define PROP_DO_SELFTEST	0x100
	#define PROP_PING			0x116	//16 = IG = PING

	/* Carte super vers carte balise */
	#define BEACON_DO_SELFTEST	0x400
	#define BEACON_PING			0x416	//16 = IG = PING

	/* Carte actionneur vers Strat */
	#define STRAT_ACT_SELFTEST_DONE 				0x2E3
	#define STRAT_ACT_PONG							0x226
		//Data[0] contient l'ID du robot (WHO_AM_I)

	/* Carte propulsion vers Strat */
	#define STRAT_PROP_SELFTEST_DONE 				0x2E1
	#define STRAT_PROP_PONG							0x216
		//Data[0] contient l'ID du robot (WHO_AM_I)

	/* Carte balise vers Strat */
	#define STRAT_BEACON_IR_SELFTEST_DONE			0x2E4
	#define STRAT_BEACON_US_SELFTEST_DONE			0x2E5
	#define STRAT_BEACON_PONG						0x246

	#define DEBUG_SELFTEST_LAUNCH					0x700
	//Jusqu'à 8 codes d'erreurs peuvent être transmis dans la réponse de chaque carte. (la size contient le nombre de code d'erreurs envoyés.)
	//En cas de test réussi, size vaut 0...
	typedef enum
	{
		SELFTEST_NOT_DONE = 0,
		SELFTEST_FAIL_UNKNOW_REASON,
		SELFTEST_NO_POWER,
		SELFTEST_TIMEOUT,
		SELFTEST_PROP_FAILED,
		SELFTEST_PROP_HOKUYO_FAILED,
		SELFTEST_PROP_DT10_1_FAILED,
		SELFTEST_PROP_DT10_2_FAILED,
		SELFTEST_PROP_DT10_3_FAILED,
		SELFTEST_STRAT_BIROUTE_NOT_IN_PLACE,
		SELFTEST_STRAT_RTC,
		SELFTEST_STRAT_BATTERY_NO_24V,
		SELFTEST_STRAT_BATTERY_LOW,
		SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME,
		SELFTEST_STRAT_BIROUTE_FORGOTTEN,
		SELFTEST_STRAT_SD_WRITE_FAIL,
		SELFTEST_ACT_UNREACHABLE,
		SELFTEST_PROP_UNREACHABLE,
		SELFTEST_BEACON_UNREACHABLE,

		// Self test de la carte actionneur (si actionneur indiqué, alors il n'a pas fonctionné comme prévu, pour plus d'info voir la sortie uart de la carte actionneur) :
		SELFTEST_ACT_MISSING_TEST,	//Test manquant après un timeout du selftest actionneur, certains actionneur n'ont pas le selftest d'implémenté ou n'ont pas terminé leur action (ou plus rarement, la pile était pleine et le selftest n'a pas pu se faire)
		SELFTEST_ACT_UNKNOWN_ACT,	//Un actionneur inconnu a fail son selftest. Pour avoir le nom, ajoutez un SELFTEST_ACT_xxx ici et gérez l'actionneur dans selftest.c de la carte actionneur
		SELFTEST_ACT_FRUIT_MOUTH,
		SELFTEST_ACT_LANCELAUNCHER,
		SELFTEST_ACT_ARM,
		SELFTEST_ACT_SMALL_ARM,
		SELFTEST_ACT_FILET,
		SELFTEST_POMPE,

		SELFTEST_ERROR_NB,
		SELFTEST_NO_ERROR = 0xFF
		//... ajouter ici d'éventuels nouveaux code d'erreur.
	}SELFTEST_error_code_e;


/*****************************************************************
 *		Petits mots doux echangés entre la carte propulsion
 *		et la carte stratégie
 *
 *****************************************************************/
/*
 * Procédure et messages échangés pour la localisation de l'adversaire.
 * 1- la carte propulsion localise les adversaires avec l'hokuyo
 * 2- la carte balise IR envoie ses infos à la carte propulsion
 * 3- la carte propulsion analyse la correspondance pour comprendre qui est qui (2 adv + friend) + fusion de données et calcul de la fiabilité.
 * 4- envoi des positions adverses à la stratégie (x, y, dist, teta) + fiabilité pour chaque donnée
 *
 * Possibilité de forcer en débog les positions adverses en envoyant le même message de position STRAT_ADVERSARIES_POSITION
 */


	/* carte propulsion vers carte stratégie */
//TODO renommer ces messages pour respecter le nom es cartes (STRAT et PROP)
	#define CARTE_P_TRAJ_FINIE			0x210
	#define CARTE_P_ASSER_ERREUR		0x211
	#define CARTE_P_POSITION_ROBOT		BROADCAST_POSITION_ROBOT
	#define CARTE_P_ROBOT_FREINE		0x213
	#define CARTE_P_ROBOT_CALIBRE		0x214
	#define STRAT_TRIANGLE_POSITON		0x215
		#define IT_IS_THE_LAST_TRIANGLE		0x80	// Bit à 1 si le triangle est le dernier
		/*		0:7		: Indiquant si c'est le dernier triangle
		 *		0:6-5	: Hauteur du capteur entre 0 et 2
		 *		0:5-0	: Numéro du triangle
		 * 		1-2		: x
		 * 		3-4		: y
		 * 		5-6		: teta
		 */
	#define STRAT_TRIANGLE_WARNER		0x217
	#define STRAT_ADVERSARIES_POSITION	0x299	//Position des adversaires
		#define IT_IS_THE_LAST_ADVERSARY	0x80	//Bit levé si l'adversaire envoyé est le dernier...
		/*		0 : ADVERSARY_NUMBER | IT_IS_THE_LAST_ADVERSARY	//de 0 à n, il peut y avoir plus de deux adversaires si l'on inclut notre ami...
		 * 		1 :  x [2cm]
		 * 		2 :  y [2cm]
		 * 		3-4 : teta
		 * 		5 : distance [2cm]
		 * 		6 : fiability	:    "0 0 0 0 d t y x" (distance, teta, y, x) : 1 si fiable, 0 sinon.
		 */
		#define ADVERSARY_DETECTION_FIABILITY_X			0b00000001
		#define ADVERSARY_DETECTION_FIABILITY_Y			0b00000010
		#define ADVERSARY_DETECTION_FIABILITY_TETA		0b00000100
		#define ADVERSARY_DETECTION_FIABILITY_DISTANCE	0b00001000
		#define ADVERSARY_DETECTION_FIABILITY_ALL		0b00001111


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
	#define ASSER_CASE_CALIBRAGE_1              0x112
	#define ASSER_CASE_CALIBRAGE_2              0x113
	#define ASSER_CASE_CALIBRAGE_3              0x114

	//Modif Arnaud pour la détection de triangle
	#define ASSER_LAUNCH_SCAN_TRIANGLE			0x115
	/*		0:7		: type		0 -> ROTATE / 1 -> LINEAR
	 *		0:6		: speed 	0 -> SLOW / 1 -> FAST
	 *		0:5		: way		0 -> FORWARD / 1 -> BACKWARD
	 *-----ROTATE
	 * 		1-2		: startTeta
	 * 		3-4		: endTeta
	 *-----LINEAR
	 * 		1		: startX
	 *		2		: endX
	 * 		3		: startY
	 * 		4		: endY
	 */
	 #define ASSER_LAUNCH_WARNER_TRIANGLE		0x117
	 //		0		: number_triangle


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
	//Dans data[0]: le sid actionneur & 0xFF (on ne garde que l'octet de poid faible, l'octet de poids fort contient le filtre et on en a pas besoin)
	//Dans data[1]: la commande dont en envoi le résultat. Par ex: ACT_BALLLAUNCHER_ACTIVATE
	//Dans data[2]: état de la commande, une de ces valeurs:
		#define ACT_RESULT_DONE        0	//Tout s'est bien passé
		#define ACT_RESULT_FAILED      1	//La commande s'est mal passé et on ne sait pas dans quel état est l'actionneur (par: les capteurs ne fonctionnent plus)
		#define ACT_RESULT_NOT_HANDLED 2	//La commande ne s'est pas effectué correctement et on sait que l'actionneur n'a pas bougé (ou quand la commande a été ignorée)
	//Dans data[3]: la raison du problème (si pas DONE), une de ces valeurs:
		#define ACT_RESULT_ERROR_OK           0	//Quand pas d'erreur
		#define ACT_RESULT_ERROR_TIMEOUT      1	//Il y a eu timeout, par ex l'asservissement prend trop de temps
		#define ACT_RESULT_ERROR_OTHER        2	//La commande était lié à une autre qui ne s'est pas effectué correctement, utilisé avec ACT_RESULT_NOT_HANDLED
		#define ACT_RESULT_ERROR_NOT_HERE     3	//L'actionneur ou le capteur ne répond plus (on le sait par un autre moyen que le timeout, par exemple l'AX12 ne renvoie plus d'info après l'envoi d'une commande.)
		#define ACT_RESULT_ERROR_LOGIC        4	//Erreur de logique interne à la carte actionneur, probablement une erreur de codage (par exemple un état qui n'aurait jamais dû arrivé)
		#define ACT_RESULT_ERROR_NO_RESOURCES 5 //La carte n'a pas assez de resource pour gérer la commande. Commande à renvoyer plus tard.
		#define ACT_RESULT_ERROR_INVALID_ARG  6 //La commande ne peut pas être effectuée, l'argument n'est pas valide ou est en dehors des valeurs acceptés
		#define ACT_RESULT_ERROR_CANCELED     7 //L'action a été annulé

		//Ajoutez-en si nécessaire
		#define ACT_RESULT_ERROR_UNKNOWN      255	//Erreur inconnue ou qui ne correspond pas aux précédentes.
	/////////////////////////////////////////

	/////////////////// FILET ///////////////////
	// Message de l'actionneur vers la stratégie pour informer de l'état du filet
	#define STRAT_INFORM_FILET (STRAT_FILTER | (ACT_FILTER >> 4) | 1)
		// Dans data[0]
		#define STRAT_INFORM_FILET_ABSENT	(0b000000000)
		#define STRAT_INFORM_FILET_PRESENT	(0b000000001)
	//////////////////////////////////////////////

// Code des SID des messages: 0x30x = message pour Tiny, 0x31x = message pour Krusty.
// Le SID 0x300 est reservé pour le self_test
// Ceci est un enum de SID d'actionneur avec les paramètres de chaque actions définie par des defines. L'enum est utilisé pour vérifier que tous les messages de retour d'actionneurs sont géré en strat

typedef enum { //SEUL les SID des actionneurs doivent être mis comme enum, le reste en #DEFINE
/* Messages pour Tiny */
	//////////////// HAMMER /////////////////
	ACT_HAMMER = (ACT_FILTER | 0x01),    //0x01 peut être changé mais pas le reste (sinon le message n'est plus reçu par la carte actionneur par filtrage)
		//Paramètres de HAMMER (dans data[0])
		#define ACT_HAMMER_MOVE_TO     0   // Changer la position du bras, angle en degré dans data[1] (poids faible) et data[2]. 90° = bras vertical, 0° = bras rentré
		#define ACT_HAMMER_STOP        1   // Arreter l'asservissement, en cas de problème par exemple, ne devrai pas servir en match.
										   //  Le bras n'est plus controllé après ça, si la gravité existe toujours, il tombera.
		#define ACT_HAMMER_BLOW_CANDLE 2   // Souffler une bougie. La couleur est automatiquement gérée et si on ne doit pas la souffler, rien ne se passera.
										   //  Dans data[1]: couleur du robot, RED (0) pour rouge, BLUE (1) pour bleu (comme l'enum color_e dans QS_types.h)
	/////////////////////////////////////////

	////////////// LONGHAMMER ///////////////
	ACT_LONGHAMMER =(ACT_FILTER | 0x02),
		//Paramètres de LONGHAMMER (dans data[0])
		#define ACT_LONGHAMMER_GO_UP   0    // Lever le bras
		#define ACT_LONGHAMMER_GO_DOWN 1    // Appuyer sur les bougies et les éteindres en descendant le bras
		#define ACT_LONGHAMMER_GO_PARK 2    // Ranger le bras pour diminuer le diamètre du robot
		#define ACT_LONGHAMMER_GO_STOP 3    // Arreter l'asservissement, en cas de problème par exemple, ne devrai pas servir en match.
											//Le bras n'est plus controllé après ça, si la gravité existe toujours, il tombera.
	/////////////////////////////////////////

	///////////// BALLINFLATER //////////////
	ACT_BALLINFLATER = (ACT_FILTER | 0x03),
		//Paramètres de LONGHAMMER (dans data[0])
		#define ACT_BALLINFLATER_START 0    // Gonfler le ballon pendant data[1] secondes (Uint8). Le message de retour n'attend pas la fin du gonflage.
		#define ACT_BALLINFLATER_STOP  1    // Stopper le gonflage
	/////////////////////////////////////////


	///////////// CANDLECOLOR ///////////////
	ACT_CANDLECOLOR = (ACT_FILTER | 0x04),
		//Paramètres de CANDLECOLOR (dans data[0])
		#define ACT_CANDLECOLOR_GET_LOW    0  //Bouge le capteur et récupère la couleur d'une bougie à l'étage bas
		#define ACT_CANDLECOLOR_GET_HIGH   1  //Bouge le capteur et récupère la couleur d'une bougie à l'étage haut

	#define ACT_CANDLECOLOR_RESULT (STRAT_FILTER | (ACT_FILTER >> 4) | 0x1)
		//Couleur detectée dans data[0]:
		#define ACT_CANDLECOLOR_COLOR_BLUE   0  //Couleur bleue detectée
		#define ACT_CANDLECOLOR_COLOR_RED    1  //Couleur rouge detectée
		#define ACT_CANDLECOLOR_COLOR_YELLOW 2  //Couleur jaune detectée
		#define ACT_CANDLECOLOR_COLOR_WHITE  3  //Couleur blanche detectée
		#define ACT_CANDLECOLOR_COLOR_OTHER  4  //Pas de couleur parmi les précédentes detectée
	/////////////////////////////////////////




	//////////// PLIER //////////////////////
	ACT_PLIER = (ACT_FILTER | 0x05),
		//PAramètre de PLIER (dans data[0])
		#define ACT_PLIER_OPEN         0    //Ouvre les pinces à verres
		#define ACT_PLIER_CLOSE        1    //Ferme les pinces à verres
	/////////////////////////////////////////

/* Messages pour Krusty */
	////////////// BALLLAUNCHER /////////////
	ACT_BALLLAUNCHER = (ACT_FILTER | 0x11),
		//Paramètres de BALLLAUNCHER (dans data[0])
		#define ACT_BALLLAUNCHER_ACTIVATE 0	//Activer le lanceur de balles, vitesse en tr/min dans data[1] et data[2], data[1] est le poids faible (type: Uint16 inférieur à 32000).
		#define ACT_BALLLAUNCHER_STOP     1	//Stoper le launceur de balles
	/////////////////////////////////////////

	////////////////// PLATE ////////////////
	ACT_PLATE = (ACT_FILTER | 0x12),
		//Paramètres de PLATE (dans data[0]) (0x1x: Pince, 0x2x: Rotation bras)
		#define ACT_PLATE_PLIER_CLOSE           0x10	//Fermer la pince et serrer l'assiette
		#define ACT_PLATE_PLIER_OPEN            0x11	//Ouvrir la pince et lacher l'assiette
		#define ACT_PLATE_PLIER_STOP            0x1F	//Stopper l'asservissement de la pince. Peut servir à diminer la conso courant de l'AX12, la pince en elle même ne bougera pas (sauf si il y a une assiette dans la pince ...)

		#define ACT_PLATE_ROTATE_HORIZONTALLY   0x20	//Amener le bras en position horizontale (pour prendre ou lacher une assiette par ex)
		#define ACT_PLATE_ROTATE_PREPARE        0x21	//Amener le bras en position intermédiaire (45°) pour préparer un mouvement vers l'horizontale ou verticale
		#define ACT_PLATE_ROTATE_VERTICALLY     0x22	//Amener le bras en position verticale. Ferme la pince si elle ne l'est pas avant d'effectuer le mouvement (meca oblige) (pour vider une assiette ou réduire le périmêtre du robot)
		#define ACT_PLATE_ROTATE_STOP           0x2F	//Stopper l'asservissement du bras. A éviter, dans les virages il ne faudrait pas que l'actionneur tombe (même si gros reducteur ...)
		#define ACT_PLATE_ROTATE_ANGLE          0x2A    //Changer l'angle manuellement
	/////////////////////////////////////////

	/////////////// BALLSORTER //////////////
	ACT_BALLSORTER = (ACT_FILTER | 0x13),
		//Paramètres de BALLSORTER (dans data[0])
		#define ACT_BALLSORTER_TAKE_NEXT_CHERRY 0    //Ejecter la cerise et en prendre une autre pour la detecter. Après detection, le lanceur de balle change de vitesse automatiquement.
													 //Vitesse en tr/min dans data[1] et data[2], data[1] est le poids faible (type: Uint16 inférieur à 32000).
													 //data[3] bool_e: TRUE si on doit garder une cerise blanche dans le lanceur, si la cerise n'est pas blanche, on la lance. FALSE pour toujours lancer toutes les cerises.

	#define ACT_BALLSORTER_RESULT (STRAT_FILTER | (ACT_FILTER >> 4) | 0xA)
		//Résultat de la detection de la cerise dans data[0]:
		#define ACT_BALLSORTER_WHITE_CHERRY     0
		#define ACT_BALLSORTER_NO_CHERRY        1
		#define ACT_BALLSORTER_BAD_CHERRY       2
	/////////////////////////////////////////

	////////////////// LIFT /////////////////
	ACT_LIFT_RIGHT = (ACT_FILTER | 0x14),
	ACT_LIFT_LEFT = (ACT_FILTER | 0x15),
		//Paramètres de LIFT (dans data[0]) (0x1x: Pince, 0x2x: mouvement ascenseur)
		#define ACT_LIFT_PLIER_OPEN            0x10	//Ouvrir la pince et lacher le verre
		#define ACT_LIFT_PLIER_CLOSE           0x11	//Fermer la pince et serrer le verre
		#define ACT_LIFT_PLIER_STOP            0x1F	//Stopper l'asservissement de la pince. Peut servir à diminer la conso courant de l'AX12, la pince en elle même ne bougera pas (sauf si il y a une assiette dans la pince ...)

		#define ACT_LIFT_GO_UP                 0x20	//Amener l'ascenseur en position haute.
		#define ACT_LIFT_GO_MID                0x21	//Amener l'ascenseur en position intermédiaire pour préparer un mouvement vers haut ou bas.
		#define ACT_LIFT_GO_DOWN               0x22	//Amener l'ascenseur en position basse.
		#define ACT_LIFT_STOP                  0x2F	//Stopper l'asservissement de l'ascenseur.
	/////////////////////////////////////////


	// 2014

	////////////////// FRUIT_MOUTH ///////////
	ACT_FRUIT_MOUTH = (ACT_FILTER | 0x18),   //0x16: collision avec ACT_PING
		//Paramètres de la pompe
		#define ACT_FRUIT_MOUTH_CLOSE           0x10
		#define ACT_FRUIT_MOUTH_OPEN            0x11
		#define ACT_FRUIT_MOUTH_CANCELED        0x12
		#define ACT_FRUIT_MOUTH_STOP	        0x1F

		//Fruit Labium (trappe)
		#define ACT_FRUIT_LABIUM_CLOSE          0x13
		#define ACT_FRUIT_LABIUM_OPEN           0x14
		#define ACT_FRUIT_LABIUM_STOP           0x1E

	/////////////////////////////////////////

	/////////////////LANCELAUNCHER////////////////////
	ACT_LANCELAUNCHER = (ACT_FILTER | 0x19),
		//Paramètres de LANCELAUNCHER (dans data[0])
		#define ACT_LANCELAUNCHER_RUN_1_BALL	0x11
		#define ACT_LANCELAUNCHER_RUN_5_BALL	0x12
		#define ACT_LANCELAUNCHER_RUN_ALL		0x13
		#define ACT_LANCELAUNCHER_STOP			0x1F
	////////////////////////////////////////////////

	////////////////// ARM  /////////////////
	ACT_ARM = (ACT_FILTER | 0x20),
		//Paramètres de ARM (dans data[0])
		#define ACT_ARM_GOTO 0   // Va à la position demandée dans data[1] (une des valeurs ci-dessous)
			// Voir position du bras ci-dessous ( A la fin de l'enum)

		#define ACT_ARM_STOP 1  // Stoppe l'asservissement des moteurs
	/////////////////////////////////////////

	/////////////////////FILET///////////////////
	ACT_FILET = (ACT_FILTER | 0x21),
		//Paramètres de FILET (dans data[0])
		#define ACT_FILET_IDLE				0x11
		#define ACT_FILET_LAUNCHED			0x12
		#define ACT_FILET_STOP				0x13
	/////////////////////////////////////////////

	//////////////////PETIT BRAS/////////////////
	ACT_SMALL_ARM = (ACT_FILTER | 0x22),
		//Paramètres de SMALL_ARM (dans data[0])
		#define ACT_SMALL_ARM_IDLE			0x11
		#define ACT_SMALL_ARM_MID			0x12
		#define ACT_SMALL_ARM_DEPLOYED		0x13
		#define ACT_SMALL_ARM_STOP			0x14
	/////////////////////////////////////////////

	/////////////////////POMPE///////////////////
	ACT_POMPE = (ACT_FILTER | 0x23)
		//Paramètres de SMALL_ARM (dans data[0])
		#define ACT_POMPE_NORMAL			0x11
		#define ACT_POMPE_REVERSE			0x12
		// Pour les deux actions ci-dessus dans data[1]
		   // Le rapport cyclique voulue de la pompe entre 0 et 100

		#define ACT_POMPE_STOP				0x13

	/////////////////////////////////////////////

} ACT_sid_e; //FIN de l'enum des SID d'actionneurs


// Position Prise par le bras
typedef enum {
	ACT_ARM_POS_PARKED,
	ACT_ARM_POS_OPEN,
	ACT_ARM_POS_MID,
	ARM_ST_NUMBER
} ARM_state_e;



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
	#define STRAT_FRIEND_FORCE_POSITION						0x258	//Forcer la position du robot ami



		/* Liste des messages de definition d'erreur --- Pour plus de doc, consulter QS_CANmsgDoc.h */
		#define AUCUNE_ERREUR						(0b00000000)
		#define AUCUN_SIGNAL						(0b00000001)
		#define SIGNAL_INSUFFISANT					(0b00000010)
		#define TACHE_TROP_GRANDE					(0b00000100)
		#define TROP_DE_SIGNAL						(0b00001000)
		#define ERREUR_POSITION_INCOHERENTE 		(0b00010000)
		#define OBSOLESCENCE						(0b10000000)


#endif	/* ndef QS_CANMSGLIST_H */
