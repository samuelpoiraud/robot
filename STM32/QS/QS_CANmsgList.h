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

	/* Masque des cartes (des destinataires) */
	#define MASK_BITS					0x700
	#define BROADCAST_FILTER			0x000
	#define PROP_FILTER					0x100
	#define STRAT_FILTER				0x200
	#define ACT_FILTER					0x300
	#define BALISE_FILTER				0x400
	#define DEBUG_FILTER				0x700
	#define IHM_FILTER					0x600
	//Ces messages ne sont pas destinés à voyager sur les bus CAN des robot.
	#define XBEE_FILTER					0x500

	/***********************************************************
						Message pour tous
	***********************************************************/
	#define BROADCAST_START	 			0x001
	#define BROADCAST_STOP_ALL 			0x002
	#define BROADCAST_COULEUR			0x003			// couleur de notre robot : (Uint8):(BOT_COLOR=0, TOP_COLOR =1)

	#define BROADCAST_POSITION_ROBOT	0x004
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
		Raison : Uint8 (Raison de l'envoi du Broadcast... s'agit-il d'un avertisseur ? d'un envoi périodique ? sur simple demande ?...)
			Remarque : Il est possible de CUMULER plusieurs raisons !!!
	*/
		#define WARNING_NO					(0b00000000)
		#define WARNING_TIMER				(0b00000001)
		#define WARNING_TRANSLATION			(0b00000010)
		#define WARNING_ROTATION			(0b00000100)
		#define WARNING_REACH_X				(0b00001000)		//Nous venons d'atteindre une position en X pour laquelle on nous a demandé une surveillance.
		#define WARNING_REACH_Y				(0b00010000)		//Nous venons d'atteindre une position en Y pour laquelle on nous a demandé une surveillance.
		#define WARNING_REACH_TETA			(0b00100000)		//Nous venons d'atteindre une position en Teta pour laquelle on nous a demandé une surveillance.
		#define WARNING_NEW_TRAJECTORY		(0b01000000)		//Changement de trajectoire (attention, cela inclue les trajectoires préalables ajoutées en propulsion...)
	/*
		Status : Uint8 (état actuel de la carte propulsion...)
			error_byte = ((Uint8)(COPILOT_get_trajectory()) << 5 | (Uint8)(COPILOT_get_way())) << 3 | (Uint8)(error_source);
		 8 bits  : T R x W W E E E
			 T : TRUE si robot en translation
			 R : TRUE si robot en rotation
			 x : non utilisé
			 WW : Way, sens actuel
				ANY_WAY						= 0,
				BACKWARD					= 1,
				FORWARD						= 2,
			 EEE : Erreur
				NO_ERROR = 0,
				UNABLE_TO_GO_ERROR,
				IMMOBILITY_ERROR,
				ROUNDS_RETURNS_ERROR,
				UNKNOW_ERROR
	*/



	#define BROADCAST_RESET				0x0FF			// Envoyé par la stratégie et provoque la réinitialisation du fond de panier complet

	#define BROADCAST_ALIM				0x005		// Message envoyé lors d'un changement d'état de l'alimentation 24V
		#define ALIM_OFF					(0b00000000)
		#define ALIM_ON						(0b00000001)
	/*	argument (size = 3) :
	 *	octet 0	: Etat de l'alimentation | ALIM_OFF / ALIM_ON
	 *	octets 1 et 2	: Mesure de l'alimentation sur 16 bit en mV
	 */

	/*
	 * Procédure et messages échangés pour la localisation de l'adversaire.
	 * 1- la carte propulsion localise les adversaires avec l'hokuyo
	 * 2- la carte balise IR envoie ses infos à la carte propulsion
	 * 3- la carte propulsion analyse la correspondance pour comprendre qui est qui (2 adv + friend) + fusion de données et calcul de la fiabilité.
	 * 4- envoi des positions adverses à la stratégie (x, y, dist, teta) + fiabilité pour chaque donnée
	 *
	 * Possibilité de forcer en débog les positions adverses en envoyant le même message de position BROADCAST_ADVERSARIES_POSITION
	 */

	#define BROADCAST_BEACON_ADVERSARY_POSITION_IR	0x006	//Balise InfraRouge

	#define BROADCAST_ADVERSARIES_POSITION	0x099	//Position des adversaires
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

	#define STRAT_BUZZER_PLAY			0x2FF
	typedef enum{
		BUZZER_DEFAULT_NOTE = 0,	//DO : c'est la note qui fait le plus de bruit (le buzzer crache 90dB à 10cm, 4,2kHz, 3V)
		BUZZER_NOTE_DO0,
		BUZZER_NOTE_RE0,
		BUZZER_NOTE_MI0,
		BUZZER_NOTE_FA,
		BUZZER_NOTE_SOL,
		BUZZER_NOTE_LA,
		BUZZER_NOTE_SI,
		BUZZER_NOTE_DO,
		BUZZER_NOTE_RE,
		BUZZER_NOTE_MI
	} buzzer_play_note_e;
	/* 0/1		: durée du bip en ms
	 * 2		: enum de la note voulue
	 * 3		: nombre de bip
	 */

	/***********************************************************
	 *				Message IHM pour tous
	 ***********************************************************/

	#define IHM_BUTTON  0x060
	// Envois un message CAN quand un bouton est déplacé
	// data 0 : est l'identifiant de type button_ihm_e
	// data 1 : renvoie 1 si c'est un appuie long ou 0 sinon

	#define IHM_SWITCH  0x061
	// Envois un message CAN quand un switch est déplacer
	// Chaque data correspond à un switch avec son état
	// data : les 7 bits de points faible identifiant de type switch_ihm_e
	// data : le bit de points fort est son état
	// Pour savoir, combien de switch ont été envoyé regarder msg.size

	#define IHM_SWITCH_ALL 0x062
	// Envoie l'états de tous les switchs sur 4 data
	// Le bit de points faible (soit le bit de points faible de la data[3]) correspond à SW_COLOR de switch_ihm_e

	#define IHM_POWER 0x063
	// Envoie l'états de la batterie a toute les cartes
	// data 0 : de type IHM_power_e

	#define IHM_BIROUTE_IS_REMOVED	0x064	//La biroute vient d'être retirée, let's go, c'est parti pour la purée !


	/***********************************************************
						Message pour personne
	***********************************************************/
	#define DEBUG_CARTE_P				0x742
	#define DEBUG_FOE_POS				0x748
	/* arguments (size = 8) :
	 *	Uint8	octet d'erreur (toujours à 0)
	 *	Sint16	angle de vue de l'adversaire [rad/4096] (de -PI4096 à PI4096)
	 *	Uint8	distance de vue de l'adversaire [cm] (ATTENTION, unité = cm !!! donc maximum = 2,55m)
	 *	Sint16	position adverse x	[mm]	(ATTENTION, peut être négatif si on le voit prêt de la bordure avec une petite erreur...)
	 *	Sint16  position adverse y	[mm]	(idem)
	 */
	#define DEBUG_ELEMENT_UPDATED		0x749
	/* arguments (size = 6) :
	 *	Uint8	raison (ADD, UPDATE, DELETE)
	 *	Uint8 	type de pion (cf. module element)
	 *	Sint16	position element x	[mm]
	 *	Sint16	position element y	[mm]
	*/
	#define DEBUG_BEACON_US1_ERROR      0x750  //Envoyé par la strat
	#define DEBUG_BEACON_US2_ERROR      0x751  //Envoyé par la strat
	#define DEBUG_BEACON_US_POWER       0x752  //Enovyé par la balise receptrice US (mais non activé actuellement)
	#define IR_ERROR_RESULT				0x753	//Message de la super pour l'EEPROM -> compteur des erreurs de balise
	#define DEBUG_FOE_REASON			0x755	//Message de debug qui explique la raison d 'un evitement
	#define DEBUG_US_NOT_RELIABLE		0x756	//Message qui indique que on ne fait plus confiance aux ultrasons

	#define DEBUG_STRAT_STATE_CHANGED	0x760  //Envoyé par la strat quand un état change d'une machine à état
		//Paramètres: data[0]:data[1] = ID d'une machine à état (data[0] le poids fort), data[2] = old_state, data[3] = new_state, data suivant: paramètres divers

	#define DEBUG_PROPULSION_MAILBOX_IN_IS_FULL						0x70D
	#define DEBUG_PROPULSION_MAILBOX_OUT_IS_FULL					0x70E
	#define DEBUG_ENABLE_MODE_BEST_EFFORT							0x70F
	#define DEBUG_PROPULSION_ERREUR_RECOUVREMENT_IT					0x710

	#define DEBUG_DO_TRAJECTORY_FOR_TEST_COEFS						0x711
	#define DEBUG_TRAJECTORY_FOR_TEST_COEFS_DONE					0x712

	#define DEBUG_PROPULSION_SET_ACCELERATION						0x713		//Data sur 16 bits. (unité : mm/4096/5ms/5ms) Grandeur typique : 100
		//Uint16 : durée de la trajectoire en [ms]

	#define DEBUG_SET_ERROR_TRESHOLD_TRANSLATION					0x714
		//Sint32 sur data 0 à 3 : en mm.
	typedef enum
	{
		ODOMETRY_COEF_TRANSLATION = 0,
		ODOMETRY_COEF_SYM,
		ODOMETRY_COEF_ROTATION,
		ODOMETRY_COEF_CENTRIFUGAL,		//attention, la valeur de ODOMETRY_COEF_CENTRIFUGAL est utilisé comme borne dans le code de propulsion, il faut le laisser en dernier dans les coefs d'odométrie !
		CORRECTOR_COEF_KP_TRANSLATION,
		CORRECTOR_COEF_KD_TRANSLATION,
		CORRECTOR_COEF_KV_TRANSLATION,
		CORRECTOR_COEF_KA_TRANSLATION,
		CORRECTOR_COEF_KP_ROTATION,
		CORRECTOR_COEF_KD_ROTATION,
		CORRECTOR_COEF_KV_ROTATION,
		CORRECTOR_COEF_KA_ROTATION,
		GYRO_COEF_GAIN,
		PROPULSION_NUMBER_COEFS
	}PROPULSION_coef_e;

	#define DEBUG_PROPULSION_GET_COEFS								0x720
		//Demande à connaitre l'ensemble des coefs de la propulsion
	#define DEBUG_PROPULSION_COEF_IS								0x721
		//data[0] : id du coef
		//datas[1 à 4] : coef  !!! sur 32 bits !!!
	#define DEBUG_PROPULSION_SET_COEF								0x722
		//data[0] : id du coef
		//datas[1 à 4] : coef !!! sur 32 bits !!!

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


	#define DEBUG_PROP_MOVE_POSITION	0x783
	// Altère les coordonnées sur robot de manière relative
	/* data [0/1] = Sint16 offset en X (signé)
	 * data [2/3] = Sint16 offset en Y (signé)
	 * data [4/5] = Sint16 offset teta (signé)
	 */

	//message pour debug strategie

	#define DEBUG_DETECT_FOE			0x799	//Déclenchement manuel d'un évitement.

	//message pour le simulateur 2014

	#define DEBUG_AVOIDANCE_POLY		0x790
	// [0]		: new_polygonye ( TRUE / FALSE)
	// [1]		: numero du premier point du message
	// [2/3]	: x/16 (x>>4) / y/16 (y>>4) point n
	// [4/5]	: x/16 (x>>4) / y/16 (y>>4) point n+1
	// [6/7]	: x/16 (x>>4) / y/16 (y>>4) point n+2

	#define DEBUG_HOKUYO_ADD_POINT				0x791
	// [0/1]	: x/16 (x>>4) / y/16 (y>>4) point n
	// [2/3]	: x/16 (x>>4) / y/16 (y>>4) point n+1
	// [4/5]	: x/16 (x>>4) / y/16 (y>>4) point n+2
	// [6/7]	: x/16 (x>>4) / y/16 (y>>4) point n+3
	#define DEBUG_HOKUYO_RESET					0x792
	#define DEBUG_HOKUYO_INTENSITY_ADD_POINT	0x793
	#define DEBUG_HOKUYO_INTENSITY_RESET		0x794

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
	#define XBEE_REACH_POINT_GET_OUT_INIT	0x507	// Envoie un message quand il sort de la zone de départ

	#define XBEE_ZONE_COMMAND				0x5AA	//Effectue une demande lié au zones (un SID pour toute la gestion des zones comme ça)
	//Commande dans data[0]:
		#define XBEE_ZONE_TRY_LOCK       0	//Dans data[1]: la zone, type: map_zone_e. La réponse de l'autre robot sera envoyé avec XBEE_ZONE_LOCK_RESULT
		#define XBEE_ZONE_LOCK_RESULT    1	//Dans data[1]: la zone concernée, data[2]: TRUE/FALSE suivant si le verouillage à été accepté ou non
		#define XBEE_ZONE_UNLOCK         2  //Dans data[1]: la zone libérée. Libère une zone qui a été verouillée

	#define XBEE_HOLLY_ASK_PROTECT          0x5C0
	#define XBEE_WOOD_PROTECT_ZONE         	0x5C1
		/*	Le rectangle dans lequel HOLLY doit ignorer les adversaires est indiqué ainsi
		 * data[0-1] : Sint16 : x1
		 * data[2-3] : Sint16 : x2
		 * data[4-5] : Sint16 : y1
		 * data[6-7] : Sint16 : y2
		 *
		 * ATTENTION : Pour déclarer que l'on arrête la protection, on envoie toutes les datas à 0 !!!
		 */
	#define XBEE_WOOD_TAKE_CUP              0x5C2
		 /*Wood indique à Holly si il a pris les gobelets
		  *data[0]:
		  * bit4 cup south green
		  * bit3 cup north green
		  * bit2 cup middle
		  * bit1 cup south yellow
		  * bit0 cup north yellow
		  */
	#define XBEE_HOLLY_START_MATCH			0x5C3
	#define XBEE_WOOD_CAN_DO_CLAP			0x5C4
	#define XBEE_WOOD_CAN_GO                0x5C5

	#define XBEE_CAN_WOOD_ACCESS_SCAN		0x5B0
	#define XBEE_WOOD_ACCESS_SCAN			0x5B1	//Réponse...
		//data[0]... cf le code qui utilise ces messages.
	#define XBEE_MY_POSITION_IS				0x5FF
			/*
				X : Sint16 (mm)
				Y : Sint16 (mm)
				robot_id : robot_id_e (Uint8)
				+ 3 octets RFU
			*/

/******************************************************************
 *
 * 		Messages échangés entre les cartes stratégies et la balise fixe
 *
 ******************************************************************/
											//B comme Beacon (ou Balise !)
	#define ENABLE_WATCHING_ZONE			0x5BE	//E comme Enable
	#define DISABLE_WATCHING_ZONES			0x5BD	//D comme Disable
	#define GET_ZONE_INFOS					0x5B1	//1 comme 1nfos...

	#define STRAT_ZONE_INFOS				0x551	//5tratégie 1nfos

	typedef enum
	{
		ZONE_NUMBER		//Nombre de zones...
	}zone_e;

	typedef Uint8 zone_event_t;
	#define EVENT_NO_EVENT	0b00000000
	#define EVENT_GET_IN	0b00000001
	#define EVENT_GET_OUT	0b00000010
	#define EVENT_TIME_IN	0b00000100
	#define EVENT_SPECIAL	0b00001000


 /*****************************************************************
 *
 *		Messages echangés entre la carte Supervision
 *		et les cartes stratégie, actionneur et propulsion
 *
 *****************************************************************/

	/* Carte strat vers carte actionneur */
	#define ACT_DO_SELFTEST		0x300
	#define ACT_PING			0x316	//16 = IG = PING

	/* Carte strat vers carte propulsion */
	#define PROP_DO_SELFTEST	0x100
	#define PROP_PING			0x116	//16 = IG = PING

	/* Carte strat vers carte balise */
	#define BEACON_DO_SELFTEST	0x400
	#define BEACON_PING			0x416	//16 = IG = PING

	/* Carte strat vers carte IHM */
	#define IHM_DO_SELFTEST		0x600
	#define IHM_PING			0x616	//16 = IG = PING

	/* Carte actionneur vers Strat */
	#define STRAT_ACT_SELFTEST_DONE 				0x2E3
	#define STRAT_ACT_PONG							0x226
		//Data[0] contient l'ID du robot (WHO_AM_I)

	/* Carte propulsion vers Strat */
	#define STRAT_PROP_SELFTEST_DONE 				0x2E1
	#define STRAT_PROP_PONG							0x216
	#define STRAT_CUP_POSITION                      0x219

	/* Carte IHM vers Strat */
	#define STRAT_IHM_SELFTEST_DONE 				0x2E6
	#define STRAT_IHM_PONG							0x266
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
		SELFTEST_TIMEOUT,

		SELFTEST_BEACON_ADV1_NOT_SEEN,				//Ne rien mettre avant ceci sans synchroniser le QS_CANmsgList dsPIC pour la balise !!!
		SELFTEST_BEACON_ADV2_NOT_SEEN,
		SELFTEST_BEACON_SYNCHRO_NOT_RECEIVED,
		SELFTEST_BEACON_ADV1_BATTERY_LOW,
		SELFTEST_BEACON_ADV2_BATTERY_LOW,
		SELFTEST_BEACON_ADV1_RF_UNREACHABLE,
		SELFTEST_BEACON_ADV2_RF_UNREACHABLE,

		SELFTEST_PROP_FAILED,
		SELFTEST_PROP_HOKUYO_FAILED,
		SELFTEST_PROP_IN_SIMULATION_MODE,
		SELFTEST_PROP_IN_LCD_TOUCH_MODE,
		SELFTEST_PROP_SWITCH_ASSER_DISABLE,
		SELFTEST_PROP_SENSOR_CUP_LEFT,
		SELFTEST_PROP_SENSOR_CUP_RIGHT,

		SELFTEST_STRAT_AVOIDANCE_SWITCH_DISABLE,
		SELFTEST_STRAT_XBEE_SWITCH_DISABLE,
		SELFTEST_STRAT_XBEE_DESTINATION_UNREACHABLE,
		SELFTEST_STRAT_RTC,
		SELFTEST_STRAT_BATTERY_NO_24V,
		SELFTEST_STRAT_BATTERY_LOW,
		SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME,
		SELFTEST_STRAT_BIROUTE_FORGOTTEN,
		SELFTEST_STRAT_SD_WRITE_FAIL,
		SELFTEST_STRAT_ESTRADE_SENSOR_RIGHT,
		SELFTEST_STRAT_ESTRADE_SENSOR_LEFT,
		SELFTEST_STRAT_SWITCH_POPCORN_DISABLED,
		SELFTEST_STRAT_SWITCH_LEFT_PUMP_DISABLED,
		SELFTEST_STRAT_SWITCH_RIGHT_PUMP_DISABLED,
		SELFTEST_STRAT_SWITCH_CLAPS_DISABLED,
		SELFTEST_STRAT_SWITCH_CARPETS_DISABLED,
		SELFTEST_STRAT_SWITCH_LIFT_DISABLED,

		SELFTEST_IHM_BATTERY_NO_24V,
		SELFTEST_IHM_BATTERY_LOW,
		SELFTEST_IHM_BIROUTE_FORGOTTEN,
		SELFTEST_IHM_POWER_HOKUYO_FAILED,

		SELFTEST_ACT_UNREACHABLE,
		SELFTEST_PROP_UNREACHABLE,
		SELFTEST_BEACON_UNREACHABLE,
		SELFTEST_IHM_UNREACHABLE,

		// Self test de la carte actionneur (si actionneur indiqué, alors il n'a pas fonctionné comme prévu, pour plus d'info voir la sortie uart de la carte actionneur) :
		SELFTEST_ACT_MISSING_TEST,	//Test manquant après un timeout du selftest actionneur, certains actionneur n'ont pas le selftest d'implémenté ou n'ont pas terminé leur action (ou plus rarement, la pile était pleine et le selftest n'a pas pu se faire)
		SELFTEST_ACT_UNKNOWN_ACT,	//Un actionneur inconnu a fail son selftest. Pour avoir le nom, ajoutez un SELFTEST_ACT_xxx ici et gérez l'actionneur dans selftest.c de la carte actionneur
		SELFTEST_ACT_POP_COLLECT_LEFT,
		SELFTEST_ACT_POP_COLLECT_RIGHT,
		SELFTEST_ACT_POP_DROP_LEFT,
		SELFTEST_ACT_POP_DROP_RIGHT,
		SELFTEST_ACT_BACK_SPOT_LEFT,
		SELFTEST_ACT_BACK_SPOT_RIGHT,
		SELFTEST_ACT_SPOT_POMPE_LEFT,
		SELFTEST_ACT_SPOT_POMPE_RIGHT,
		SELFTEST_ACT_CARPET_LAUNCHER_LEFT,
		SELFTEST_ACT_CARPET_LAUNCHER_RIGHT,
		SELFTEST_ACT_PINCEMI_RIGHT,
		SELFTEST_ACT_PINCEMI_LEFT,
		SELFTEST_ACT_ELEVATOR,
		SELFTEST_ACT_STOCK_RIGHT,
		SELFTEST_ACT_STOCK_LEFT,
		SELFTEST_ACT_CUP_NIPPER,
		SELFTEST_ACT_CUP_NIPPER_ELEVATOR,
		SELFTEST_ACT_CLAP,
		SELFTEST_ACT_PINCE_DROITE,
		SELFTEST_ACT_PINCE_GAUCHE,
		SELFTEST_ACT_POP_DROP_LEFT_WOOD,
		SELFTEST_ACT_POP_DROP_RIGHT_WOOD,
		SELFTEST_ACT_CLAP_HOLLY,

		SELFTEST_ERROR_NB,
		SELFTEST_NO_ERROR = 0xFF
		//... ajouter ici d'éventuels nouveaux code d'erreur.
	}SELFTEST_error_code_e;


/*****************************************************************
 *		Petits mots doux echangés entre la carte propulsion
 *		et la carte stratégie
 *
 *****************************************************************/

	/* carte propulsion vers carte stratégie */
//TODO renommer ces messages pour respecter le nom es cartes (STRAT et PROP)
	#define STRAT_TRAJ_FINIE			0x210
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
	*/
	#define STRAT_PROP_ERREUR			0x211
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
		0x00  : Uint8 RFU (Reserved for Future Use)
		Error : Octet caractérisant l'erreur rencontrée :
				0bTRxWWEEE
					 T = bool_e
					 R = bool_e
					 x = non utilisé
					 WW  = way_e
					 EEE = SUPERVISOR_error_source_e
				avec :
					T = 1 si robot en translation
					R = 1 si robot en rotation
					La combinaison des 2 bit T et R est possible lors de courbe ou correction d'angle lors d'un mouvement

					typedef enum
					{
						ANY_WAY=0,
						BACKWARD,
						FORWARD
					} way_e;

					typedef enum
					{
						NO_ERROR = 0,					//Ne doit pas arriver, s'il y a eu erreur, c'est qu'il y a une raison
						UNABLE_TO_GO_ERROR,				//La raison la plus fréquente : un obstacle empêche la propulsion d'atteindre son objectif. (se produit lors d'une absence d'alim de puissance)
						IMMOBILITY_ERROR,				//Le robot est proche de son objectif, mais est immobilisé (par un élément où un obstacle...)
						ROUNDS_RETURNS_ERROR,			//Une erreur d'algo de propulsion ou de mécanique produit une oscillation autour de notre objectif et nous le rend difficile à atteindre. On peut considérer qu'on est arrivé à notre objectif (il est proche !)
						UNKNOW_ERROR					//N'existe pas à l'heure où j'écris ces lignes... RFU (Reserved for Futur Use)
					}SUPERVISOR_error_source_e;


	*/

	#define STRAT_ROBOT_FREINE		0x213
	/*
		X : Sint16 (mm)
		Y : Sint16 (mm)
		Angle : Sint16 (RAD4096)
	*/
	#define PROP_ROBOT_CALIBRE		0x214

	#define STRAT_PROP_FOE_DETECTED		0x215
	/* Message CAN envoyé par la propulsion vers la stratégie lors de la détection du adversaire dans le déplacement demandé
	 *	0/1 : x
	 *  2/3 : y
	 *  4 : timeout (si l'évitement a eu lieu avec un timeout ou directement)
	 */

	#define STRAT_SEND_REPORT		0x217
	/* Envoie à la fin du match de la prop pour la stratégie du total de rotation et de translation faite
	 * 0/1 : rotation en PI4096/8 (>>3)
	 * 2/3 : max rotation en PI4096/8 (>>3)
	 * 4/5 : translation en mm/2  (>>1)
	 */

	#define STRAT_INFORM_CAPTEUR		0x218
		// Dans data[0] choix du capteur selon la liste à côté du message CAN ACT_ASK_SENSOR
		// Dans data[1]
		#define STRAT_INFORM_CAPTEUR_ABSENT		(0b000000000)
		#define STRAT_INFORM_CAPTEUR_PRESENT	(0b000000001)




	/* carte stratégie vers carte propulsion */
	#define PROP_GO_POSITION					0x155
	/* arguments :
		CONFIG	: Uint8	=>
							|.0.. .... - aqcuittement demandé normalement (par défaut)
							|.1.. .... - demande de NON acquittement de l'ordre
							|..0. .... - ordre non multipoint
							|..1. .... - ou multipoint
							|
							|...0 .... - ordre a executer maintenant
							|...1 .... - ou a la fin du buffer
							|
							|.... ...0 - ordre dans referentiel pas relatif (donc absolu)
							|.... ...1 - ou relatif
							|
							|.... ..0. - sans mode bordure
							|.... ..1. - avec mode bordure
		XHIGH : Uint8	bits les plus significatifs de X, (mm)
		XLOW : Uint8	bits les moins significatifs de X
		YHIGH : Uint8	bits les plus significatifs de Y, (mm)
		YLOW  : Uint8	bits les moins significatifs de Y
		VITESSE : Uint8	=>	0x00 : rapide
							0x01 : lent
							0x02 : très lent
							....
							0x08 à 0xFF : vitesse "analogique"

		MARCHE : Uint8	=>	|...0 ...0 |_ marche avt ou arrière
							|...1 ...1 |
							|
							|...0 ...1 	- marche avant obligé
							|...1 ...0 	- marche arrière obligée

		RAYONCRB : Uint8 =>	|.... ...0 | > courbe automatique désactivé (choix en propulsion)
							|.... ...1 | > courbe automatique désactivé (choix en propulsion)
							|xxxx .... | > type d'évitement sur la trajectoire
	*/
	#define PROP_GO_ANGLE						0x177
	/* argument :
			CONFIG : Uint8	=>
								|.0.. .... - aqcuittement demandé normalement (par défaut)
								|.1.. .... - demande de NON acquittement de l'ordre
								|..0. .... - ordre non multipoint
								|..1. .... - ou multipoint
								|
								|...0 .... - ordre a executer maintenant
								|...1 .... - ou a la fin du buffer
								|
								|.... ...0 - ordre dans referentiel pas relatif (donc absolu)
								|.... ...1 - ou relatif
			TETAHIGH : Uint8	bits les plus significatifs de TETA,(rad/4096)
			TETALOW : Uint8		bits les moins significatifs de TETA
			0
			0
			VITESSE : Uint8	=>	0x00 : rapide
								0x01 : lent
								0x02 : très lent
								....
								0x08 à 0xFF : vitesse "analogique"

			MARCHE : Uint8	=>	|...0 ...0 |_ marche avt ou arrière
								|...1 ...1 |
								|
								|...0 ...1 	- marche avant obligé
								|...1 ...0 	- marche arrière obligée

			RAYONCRB : Uint8
		*/
	#define PROP_SEND_PERIODICALLY_POSITION		0x188
		/*
			PERIODE : 		Uint16 		unité : [ms] 	Période à laquelle on veut recevoir des messages de BROADCAST_POSITION
			TRANSLATION : 	Sint16 		unité : mm		Déplacement du robot au delà duquel on veut recevoir un BROADCAST_POSITION
			ROTATION : 		Sint16		unité : rad4096	Déplacement du robot au delà duquel on veut recevoir un BROADCAST_POSITION

			Si l'un des paramètres vaut 0, l'avertisseur correspondant est désactivé.

			Remarque :
			l'it tourne à 5ms => plus petite période d'envoi
			donc la PERIODE effective sera la période demandée arrondie au 5ms supérieur !

		*/
	#define PROP_STOP							0x101
	#define PROP_TELL_POSITION					0x105
	#define	PROP_SET_POSITION					0x107
	/*
		XHIGH : Uint8		bits les plus significatifs de X, (mm)
		XLOW : Uint8		bits les moins significatifs de X
		YHIGH : Uint8		bits les plus significatifs de Y, (mm)
		YLOW  : Uint8		bits les moins significatifs de Y
		TETAHIGH : Uint8	bits les plus significatifs de TETA,(rad/4096)
		TETALOW : Uint8		bits les moins significatifs de TETA
	*/
	#define PROP_RUSH_IN_THE_WALL				0x109
	/*
		SENS : way_e Uint8
		asservissement en rotation on (1)/off(0) : Uint8
	*/
	#define PROP_CALIBRATION					0x10B
	/* argument :
		SENS : way_e (Uint8)
		0 pour demander un désarmement !!!
	*/

	#define PROP_WARN_ANGLE						0x10C
	/* argument :
		Angle : Sint16 (RAD4096)
			0 pour demander un désarmement !!!
			ATTENTION, pas d'armement possible en 0, demandez 1[rad/4096], c'est pas si loin.
	*/
	#define PROP_WARN_X							0x10D
	/* argument :
		x : Sint16 (mm)
			0 pour demander un désarmement !!!
	*/
	#define PROP_WARN_Y							0x10E
	/* argument :
		y : Sint16 (mm)
			0 pour demander un désarmement !!!
	*/
	#define PROP_SET_CORRECTORS					0x10F
		//data 0 : bool_e  correcteur en rotation
		//data 1 : bool_e  correcteur en translation
	#define PROP_JOYSTICK 						0x111


	#define PROP_OFFSET_AVOID					0x112
	// data 0/1 : Uint16 Xleft
	// data 2/3 : Uint16 Xright
	// data 4/5 : Uint16 Yfront
	// data 6/7 : Uint16 Yback

	#define PROP_SCAN_CUP						0x113
	// A compléter !

	#define PROP_DEBUG_FORCED_FOE				0x118
	// Message CAN pour forcer la détection d'un adversaire

	#define PROP_START_ROTATION					0x114

	#define PROP_WOOD_PROTECT_ZONE				0x120
		/*	Le rectangle dans lequel HOLLY doit ignorer les adversaires est indiqué ainsi
		 * data[0-1] : Sint16 : x1
		 * data[2-3] : Sint16 : x2
		 * data[4-5] : Sint16 : y1
		 * data[6-7] : Sint16 : y2
		 *
		 * ATTENTION : Pour déclarer que l'on arrête la protection, on envoie toutes les datas à 0 !!!
		 */

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
	/////////////////////////////////////////////

	///////////////COMMON ARG////////////////////
	// Dans data[1]
	#define ACT_CONFIG						0xFF	// Argument ajouté aprés un SID d'actionneur pour le configure
		// Dans data[2]
		#define AX12_SPEED_CONFIG				0x00
		   // wheel mode (0 à 100)
		   // position mode (0 à 500)
		#define AX12_TORQUE_CONFIG				0x01
		   // (0 à 100)
	/////////////////////////////////////////////

	#define ACT_ASK_SENSOR						0x3FE
		// Dans data[0]
		#define PINCE_GOBELET_DROITE		(0b000000000)
		#define PINCE_GOBELET_GAUCHE		(0b000000001)
		#define GOBELET_DEVANT_WOOD			(0b000000010)
		#define GOBELET_HOLLY				(0b000000011)

// Code des SID des messages: 0x30x = message pour Holly, 0x33x = message pour Wood.
// Le SID 0x300 est reservé pour le self_test
// Ceci est un enum de SID d'actionneur avec les paramètres de chaque actions définie par des defines. L'enum est utilisé pour vérifier que tous les messages de retour d'actionneurs sont géré en strat

typedef enum { //SEUL les SID des actionneurs doivent être mis comme enum, le reste en #DEFINE

#if 0
	//////////////////EXEMPLE///////////////////
	ACT_EXEMPLE = (ACT_FILTER | 0x01),
		//Paramètres de EXEMPLE (dans data[0])
		#define ACT_EXEMPLE_POS_1			0x11
		#define ACT_EXEMPLE_POS_2			0x12
		#define ACT_SMALL_ARM_STOP			0x13
#endif

	//////////////////////////////////////////////////////////////////
	//----------------------------HOLLY-----------------------------//
	//////////////////////////////////////////////////////////////////

	/////////////////TEST SERVO///////////////////
	ACT_TEST_SERVO = (ACT_FILTER | 0x01),
		//Paramètres de TEST_SERVO (dans data[0])
		#define ACT_TEST_SERVO_IDLE					0x11
		#define ACT_TEST_SERVO_STATE_1				0x12
		#define ACT_TEST_SERVO_STATE_2				0x13
		#define ACT_TEST_SERVO_STOP					0x14

	///////////COLLECT POPCORN LEFT////////////////
	ACT_POP_COLLECT_LEFT = (ACT_FILTER | 0x02),
		#define ACT_POP_COLLECT_LEFT_OPEN			0x11
		#define ACT_POP_COLLECT_LEFT_MID			0x12
		#define ACT_POP_COLLECT_LEFT_CLOSED			0x13
		#define ACT_POP_COLLECT_LEFT_STOP			0x14

	///////////COLLECT POPCORN RIGHT///////////////
	ACT_POP_COLLECT_RIGHT = (ACT_FILTER | 0x03),
		#define ACT_POP_COLLECT_RIGHT_OPEN			0x11
		#define ACT_POP_COLLECT_RIGHT_MID			0x12
		#define ACT_POP_COLLECT_RIGHT_CLOSED		0x13
		#define ACT_POP_COLLECT_RIGHT_STOP			0x14

	//////////////DROP POPCORN LEFT////////////////
	ACT_POP_DROP_LEFT = (ACT_FILTER | 0x04),
		#define ACT_POP_DROP_LEFT_IDLE				0x11
		#define ACT_POP_DROP_LEFT_OPEN				0x12
		#define ACT_POP_DROP_LEFT_CLOSED			0x13
		#define ACT_POP_DROP_LEFT_STOP				0x14

	//////////////DROP POPCORN RIGHT///////////////
	ACT_POP_DROP_RIGHT = (ACT_FILTER | 0x05),
		#define ACT_POP_DROP_RIGHT_IDLE				0x11
		#define ACT_POP_DROP_RIGHT_OPEN				0x12
		#define ACT_POP_DROP_RIGHT_CLOSED			0x13
		#define ACT_POP_DROP_RIGHT_STOP				0x14

	///////////////////ELEVATOR////////////////////
	ACT_ELEVATOR = (ACT_FILTER | 0x06),
		#define ACT_ELEVATOR_BOT					0x11
		#define ACT_ELEVATOR_MID					0x12
		#define ACT_ELEVATOR_PRE_TOP				0x13
		#define ACT_ELEVATOR_TOP					0x14
		#define ACT_ELEVATOR_STOP					0x15

	/////////////////PINCEMI RIGHT//////////////////
	ACT_PINCEMI_RIGHT = (ACT_FILTER | 0x07),
		#define ACT_PINCEMI_RIGHT_CLOSE				0x11
		#define ACT_PINCEMI_RIGHT_CLOSE_INNER		0x12
		#define ACT_PINCEMI_RIGHT_LOCK				0x13
		#define ACT_PINCEMI_RIGHT_LOCK_BALL			0x14
		#define ACT_PINCEMI_RIGHT_UNLOCK			0x15
		#define ACT_PINCEMI_RIGHT_OPEN				0x16
		#define ACT_PINCEMI_RIGHT_OPEN_GREAT		0x17
		#define ACT_PINCEMI_RIGHT_STOP				0x18

	/////////////////PINCEMI LEFT//////////////////
	ACT_PINCEMI_LEFT = (ACT_FILTER | 0x08),
		#define ACT_PINCEMI_LEFT_CLOSE				0x21
		#define ACT_PINCEMI_LEFT_CLOSE_INNER		0x22
		#define ACT_PINCEMI_LEFT_LOCK				0x23
		#define ACT_PINCEMI_LEFT_LOCK_BALL			0x24
		#define ACT_PINCEMI_LEFT_UNLOCK				0x25
		#define ACT_PINCEMI_LEFT_OPEN				0x26
		#define ACT_PINCEMI_LEFT_OPEN_GREAT			0x27
		#define ACT_PINCEMI_LEFT_STOP				0x28

	/////////////////STOCK RIGHT///////////////////
	ACT_STOCK_RIGHT = (ACT_FILTER | 0x09),
		#define ACT_STOCK_RIGHT_CLOSE				0x11
		#define ACT_STOCK_RIGHT_LOCK				0x12
		#define ACT_STOCK_RIGHT_UNLOCK				0x13
		#define ACT_STOCK_RIGHT_OPEN				0x14
		#define ACT_STOCK_RIGHT_STOP				0x15

	//////////////////STOCK LEFT///////////////////
	ACT_STOCK_LEFT = (ACT_FILTER | 0x0A),
		#define ACT_STOCK_LEFT_CLOSE				0x21
		#define ACT_STOCK_LEFT_LOCK					0x22
		#define ACT_STOCK_LEFT_UNLOCK				0x23
		#define ACT_STOCK_LEFT_OPEN					0x24
		#define ACT_STOCK_LEFT_STOP					0x25

	//////////////////CUP NIPPER///////////////////
	ACT_CUP_NIPPER = (ACT_FILTER | 0x0B),
		#define ACT_CUP_NIPPER_OPEN					0x11
		#define ACT_CUP_NIPPER_CLOSE				0x12
		#define ACT_CUP_NIPPER_LOCK					0x13
		#define ACT_CUP_NIPPER_STOP					0x14

	///////////////CUP NIPPER ELEVATOR////////////////
	ACT_CUP_NIPPER_ELEVATOR = (ACT_FILTER | 0x0C),
		#define ACT_CUP_NIPPER_ELEVATOR_IDLE		0x11
		#define ACT_CUP_NIPPER_ELEVATOR_UP			0x12
		#define ACT_CUP_NIPPER_ELEVATOR_STOP		0x13

	///////////////BACK SPOT RIGHT/////////////////
	ACT_BACK_SPOT_RIGHT = (ACT_FILTER | 0x0D),
		#define ACT_BACK_SPOT_RIGHT_OPEN			0x11
		#define ACT_BACK_SPOT_RIGHT_CLOSED			0x12
		#define ACT_BACK_SPOT_RIGHT_STOP			0x13

	///////////////BACK SPOT LEFT/////////////////
	ACT_BACK_SPOT_LEFT = (ACT_FILTER | 0x0E),
		#define ACT_BACK_SPOT_LEFT_OPEN				0x11
		#define ACT_BACK_SPOT_LEFT_CLOSED			0x12
		#define ACT_BACK_SPOT_LEFT_STOP				0x13

	///////////////SPOT POMPE LEFT/////////////////
	ACT_SPOT_POMPE_LEFT = (ACT_FILTER | 0x0F),
		#define ACT_SPOT_POMPE_LEFT_NORMAL			0x11
		#define ACT_SPOT_POMPE_LEFT_REVERSE			0x12
		#define ACT_SPOT_POMPE_LEFT_STOP			0x13

	///////////////BACK SPOT RIGHT/////////////////
	ACT_SPOT_POMPE_RIGHT = (ACT_FILTER | 0x10),
		#define ACT_SPOT_POMPE_RIGHT_NORMAL			0x11
		#define ACT_SPOT_POMPE_RIGHT_REVERSE		0x12
		#define ACT_SPOT_POMPE_RIGHT_STOP			0x13

	//////////////CARPET LAUNCHER RIGHT/////////////
	ACT_CARPET_LAUNCHER_RIGHT = (ACT_FILTER | 0x11),
		#define ACT_CARPET_LAUNCHER_RIGHT_IDLE		0x11
		#define ACT_CARPET_LAUNCHER_RIGHT_LAUNCH	0x12
		#define ACT_CARPET_LAUNCHER_RIGHT_LOADING	0x13
		#define ACT_CARPET_LAUNCHER_RIGHT_STOP		0x14

	////////////CARPET LAUNCHER LEFT////////////////
	ACT_CARPET_LAUNCHER_LEFT = (ACT_FILTER | 0x12),
		#define ACT_CARPET_LAUNCHER_LEFT_IDLE		0x11
		#define ACT_CARPET_LAUNCHER_LEFT_LAUNCH		0x12
		#define ACT_CARPET_LAUNCHER_LEFT_LOADING	0x13
		#define ACT_CARPET_LAUNCHER_LEFT_STOP		0x14

	////////////CLAP HOLLY////////////////
	ACT_CLAP_HOLLY = (ACT_FILTER | 0x13),
		#define ACT_CLAP_HOLLY_IDLE					0x11
		#define ACT_CLAP_HOLLY_LEFT					0x12
		#define ACT_CLAP_HOLLY_RIGHT				0x13
		#define ACT_CLAP_HOLLY_STOP					0x14



	//////////////////////////////////////////////////////////////////
	//----------------------------WOOD------------------------------//
	//////////////////////////////////////////////////////////////////

	/////////////////PINCE GAUCHE//////////////////
	ACT_PINCE_GAUCHE = (ACT_FILTER | 0x30),
		#define ACT_PINCE_GAUCHE_IDLE_POS			0x11
		#define ACT_PINCE_GAUCHE_CLOSED				0x12
		#define ACT_PINCE_GAUCHE_MID_POS			0x13
		#define ACT_PINCE_GAUCHE_OPEN				0x14
		#define ACT_PINCE_GAUCHE_STOP				0x15

	/////////////////PINCE DROITE//////////////////
	ACT_PINCE_DROITE = (ACT_FILTER | 0x31),
		#define ACT_PINCE_DROITE_IDLE_POS			0x11
		#define ACT_PINCE_DROITE_OPEN				0x12
		#define ACT_PINCE_DROITE_MID_POS			0x13
		#define ACT_PINCE_DROITE_CLOSED				0x14
		#define ACT_PINCE_DROITE_STOP				0x15

	//////////////////CLAP/////////////////////////
	ACT_CLAP = (ACT_FILTER | 0x33),
		#define ACT_CLAP_OPEN						0x11
		#define ACT_CLAP_CLOSED						0x12
		#define ACT_CLAP_STOP						0x13

	////////////POP DROP LEFT WOOD////////////////
	ACT_POP_DROP_LEFT_WOOD = (ACT_FILTER | 0x40),
		#define ACT_POP_DROP_LEFT_WOOD_OPEN			0x11
		#define ACT_POP_DROP_LEFT_WOOD_CLOSED		0x12
		#define ACT_POP_DROP_LEFT_WOOD_STOP 		0x13

	////////////POP DROP RIGHT WOOD////////////////
	ACT_POP_DROP_RIGHT_WOOD = (ACT_FILTER | 0x41)
		#define ACT_POP_DROP_RIGHT_WOOD_OPEN		0x11
		#define ACT_POP_DROP_RIGHT_WOOD_CLOSED		0x12
		#define ACT_POP_DROP_RIGHT_WOOD_STOP 		0x13



} ACT_sid_e; //FIN de l'enum des SID d'actionneurs


/*****************************************************************
 *
 *		Messages echangés entre la carte balise
 *		et la carte stratégie
 *
 *****************************************************************/

	/* carte stratégie vers carte balises */
	#define BEACON_ENABLE_PERIODIC_SENDING	0x410
	#define BEACON_DISABLE_PERIODIC_SENDING	0x411
	#define STRAT_FRIEND_FORCE_POSITION						0x258	//Forcer la position du robot ami




		/* DEFINITION DES ERREURS RENVOYEES PAR LA BALISE :
		###ATTENTION : ce texte est une copie extraite du fichier "balise_config.h" du projet balise.
		--> Plusieurs erreurs peuvent se cumuler... donc 1 bit chacune...
		*/
		#define AUCUNE_ERREUR						0b00000000
						//COMPORTEMENT : le résultat délivré semble bon, il peut être utilisé.

		#define AUCUN_SIGNAL						0b00000001
						//survenue de l'interruption timer 3 car strictement aucun signal reçu depuis au moins deux tours moteurs
						//cette erreur peut se produire si l'on est très loin
						//COMPORTEMENT : pas d'évittement par balise, prise en compte des télémètres !

		#define SIGNAL_INSUFFISANT					0b00000010
						//il peut y avoir un peu de signal, mais pas assez pour estimer une position fiable (se produit typiquement si l'on est trop loin)
						//cette erreur n'est pas grave, on peut considérer que le robot est LOIN !
						//COMPORTEMENT : pas d'évittement, pas de prise en compte des télémètres !

		#define TACHE_TROP_GRANDE					0b00000100
						//Ce cas se produit si trop de récepteurs ont vu du signal.
						// Ce seuil est STRICTEMENT supérieur au cas normal d'un robot très pret. Il y a donc probablement un autre émetteur quelque part, ou on est entouré de miroir.
						//COMPORTEMENT : La position obtenue n'est pas fiable, il faut se référer aux télémètres...

		#define TROP_DE_SIGNAL						0b00001000
						//Le récepteur ayant reçu le plus de signal en à trop recu
						//	cas 1, peu probable, le moteur est bloqué (cas de test facile pour vérifier cette fonctionnalité !)
						//	cas 2, probable, il y a un autre émetteur quelque part !!!
						// 	cas 3, on est dans une enceinte fermée et on capte trop
						//COMPORTEMENT : La position obtenue n'est pas fiable, il faut se référer aux télémètres...

		#define ERREUR_POSITION_INCOHERENTE 		0b00010000
						//La position obtenue en x/y est incohérente, le robot semble être franchement hors du terrain
						//COMPORTEMENT : si la position obtenue indique qu'il est loin, on ne fait pas d'évitement !
						//sinon, on fait confiance à nos télémètres (?)

		#define OBSOLESCENCE						0b10000000
						//La position adverse connue est obsolète compte tenu d'une absence de résultat valide depuis un certain temps.
						//COMPORTEMENT : La position obtenue n'est pas fiable, il faut se référer aux télémètres...


/*****************************************************************
 *
 *		Messages echangés entre les autres cartes
 *		et l'IHM
 *
 *****************************************************************/


#define IHM_GET_SWITCH				0x601
// Demande à la carte, IHM l'état de des switchs
// Dans chaque data mettre l'id du switch (type : switch_ihm_e) que vous souhaitez
// ASTUCE : si vous souhaitez recevoir tous les switchs, mettre 0 en size (msg.size = 0)

#define IHM_SET_LED					0x602
// Envoie les messages pour configurer les leds
// 5 bits de poids faible id : 0b...XXXXX
// 3 bits de poids fort blink (clignotement) ou couleur pour LED_COLOR_IHM : 0bXXX.....

#define IHM_LCD_BIT_RESET			0x603
// Set ou reset le bits LCD_RESET de la carte IHM
// data 0 == TRUE si set sinon reset

#define IHM_SET_ERROR				0x604
// data 0 -> ihm_error_e
// data 1 ->	TRUE	-> set
//				FALSE	-> reset


typedef enum{
	IHM_ERROR_HOKUYO	= 0b00000001,
	IHM_ERROR_ASSER		= 0b00000010
}ihm_error_e;

#endif	/* ndef QS_CANMSGLIST_H */
