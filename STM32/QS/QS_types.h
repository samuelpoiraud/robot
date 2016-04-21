/*
 *  Club Robot ESEO 2006 - 2010
 *  Game Hoover, Phoboss, Archi-Tech', PACMAN
 *
 *  Fichier : QS_types.h
 *  Package : Qualité Soft
 *  Description : Définition des types pour tout code du robot
 *  Auteur : Axel Voitier (et un peu savon aussi)
 *	Revision 2008-2009 : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100415
 */

#ifndef QS_TYPES_H
	#define QS_TYPES_H

	/**********************************************************************************************************************
	 **********************************************************************************************************************
	 **													Types basiques													 **
	 **********************************************************************************************************************
	 *********************************************************************************************************************/

	/* Type de base pour le STM32 */
	typedef unsigned char Uint8;
	typedef signed char Sint8;
	typedef unsigned short Uint16;
	typedef signed short Sint16;
	typedef unsigned long int Uint32;
	typedef signed long int Sint32;
	typedef unsigned long long Uint64;
	typedef signed long long Sint64;

	/* Type pour les SID du protocole CAN */
	typedef Uint16	Uint11;

	// time_t à l'origine, mais modifié pour être compatible avec le simulateur EVE
	typedef Uint32 time32_t;

	#ifdef FALSE
		#undef FALSE
	#endif

	#ifdef TRUE
		#undef TRUE
	#endif

	typedef enum
	{
		FALSE=0,
		TRUE
	} bool_e;

	#define TOP_COLOR_NAME "GREEN"
	#define BOT_COLOR_NAME "MAGENTA"

	typedef enum
	{
		BOT_COLOR = 0, MAGENTA=0,
		TOP_COLOR = 1, GREEN=1
	} color_e;

	typedef enum
	{
		SLOT_PROP = 0,
		SLOT_ACT,
		SLOT_STRAT,
		SLOT_INCONNU
	} slot_id_e;


	typedef enum
	{
		CODE_PROP = 0,
		CODE_ACT,
		CODE_STRAT,
		CODE_BALISE
	} code_id_e;

	typedef enum
	{
		BEACON_ID_MOTHER = 0,
		BEACON_ID_CORNER = 1,
		BEACON_ID_MIDLE = 2,
		BEACON_ID_ROBOT_1 = 3,
		BEACON_ID_ROBOT_2 = 4,
		BEACONS_NUMBER
	} beacon_id_e;

	typedef enum {
		RF_SMALL,
		RF_BIG,
		RF_FOE1,
		RF_FOE2,
		RF_BROADCAST = 7
	} RF_module_e;


	/**********************************************************************************************************************
	 **********************************************************************************************************************
	 **											Types associés à la stratégie											 **
	 **********************************************************************************************************************
	 *********************************************************************************************************************/


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

	typedef enum
	{
		END_OK=0,
		IN_PROGRESS,
		END_WITH_TIMEOUT,
		NOT_HANDLED,
		FOE_IN_PATH
	}error_e;



	/**********************************************************************************************************************
	 **********************************************************************************************************************
	 **											Types associés à la propulsion											 **
	 **********************************************************************************************************************
	 *********************************************************************************************************************/


	/*sens de trajectoire - utilisé dans le code propulsion et partagé pour la stratégie... */
	typedef enum {
		ANY_WAY=0,
		BACKWARD,
		FORWARD
	} way_e;

	/*état de la carte propulsion - utilisé dans le code propulsion et partagé pour la stratégie... */
	typedef enum
	{
		NO_ERROR = 0,
		UNABLE_TO_GO_ERROR,
		IMMOBILITY_ERROR,
		ROUNDS_RETURNS_ERROR,
		UNKNOW_ERROR
	}SUPERVISOR_error_source_e;

	/*type de trajectoire - utilisé dans le code propulsion et partagé pour la stratégie... */
	typedef enum
	{
		TRAJECTORY_TRANSLATION = 0,
		TRAJECTORY_ROTATION,
		TRAJECTORY_STOP,
		TRAJECTORY_AUTOMATIC_CURVE,
		TRAJECTORY_NONE,
		WAIT_FOREVER
	} trajectory_e;

	/*type d'évitement - utilisé dans le code propulsion et partagé pour la stratégie... */
	typedef enum
	{
		AVOID_DISABLED = 0,
		AVOID_ENABLED,
		AVOID_ENABLED_AND_WAIT
	} avoidance_e;

	typedef enum
	{
		FAST = 0,
		SLOW,
		SLOW_TRANSLATION_AND_FAST_ROTATION,
		FAST_TRANSLATION_AND_SLOW_ROTATION,
		EXTREMELY_VERY_SLOW,
		CUSTOM	//Les valeurs suivantes sont également valables (jusqu'à 255... et indiquent un choix de vitesse personnalisé !)
	 } PROP_speed_e;

	typedef enum{
		PROP_NO_BORDER_MODE = 0,
		PROP_BORDER_MODE
	}prop_border_mode_e;

	typedef enum{
		PROP_NO_CURVE = 0,
		PROP_CURVE
	}prop_curve_e;

	typedef enum{
		PROP_NO_MULTIPOINT = 0,
		PROP_MULTIPOINT
	}prop_multipoint_e;

	typedef enum{
		PROP_ABSOLUTE = 0,
		PROP_RELATIVE
	}prop_referential_e;

	typedef enum{
		PROP_NOW = 0,
		PROP_END_OF_BUFFER
	}prop_buffer_mode_e;

	typedef enum{
		PROP_NO_ACKNOWLEDGE = 0,
		PROP_ACKNOWLEDGE
	}prop_acknowledge_e;

	typedef enum{
		WARNING_NO =				(0b00000000),
		WARNING_TIMER =				(0b00000010),
		WARNING_TRANSLATION =		(0b00000100),
		WARNING_ROTATION =			(0b00001000),
		WARNING_REACH_X =			(0b00010000),
		WARNING_REACH_Y =			(0b00100000),
		WARNING_REACH_TETA =		(0b01000000),
		WARNING_NEW_TRAJECTORY =	(0b10000000)
	}prop_warning_reason_e;

	typedef enum
	{
		ODOMETRY_COEF_TRANSLATION = 0,
		ODOMETRY_COEF_SYM,             //1
		ODOMETRY_COEF_ROTATION,        //2
		ODOMETRY_COEF_CENTRIFUGAL,     //3		//attention, la valeur de ODOMETRY_COEF_CENTRIFUGAL est utilisé comme borne dans le code de propulsion, il faut le laisser en dernier dans les coefs d'odométrie !
		CORRECTOR_COEF_KP_TRANSLATION, //4
		CORRECTOR_COEF_KD_TRANSLATION, //5
		CORRECTOR_COEF_KV_TRANSLATION, //6
		CORRECTOR_COEF_KA_TRANSLATION, //7
		CORRECTOR_COEF_KP_ROTATION,    //8
		CORRECTOR_COEF_KI_ROTATION,    //9
		CORRECTOR_COEF_KD_ROTATION,    //10
		CORRECTOR_COEF_KV_ROTATION,    //11
		CORRECTOR_COEF_KA_ROTATION,    //12
		GYRO_COEF_GAIN,                //13
		PROPULSION_NUMBER_COEFS
	}PROPULSION_coef_e;


	typedef enum{
		 ACCESS_NORTH_GRANTED = 1,
		 ACCESS_SOUTH_GRANTED = 2
	}access_scan_e;


	/**********************************************************************************************************************
	 **********************************************************************************************************************
	 **												Types associés à l'évitement										 **
	 **********************************************************************************************************************
	 *********************************************************************************************************************/


	/*Type d'evitement pour construction du message de debug*/
	typedef enum {
		FORCED_BY_USER = 0,
		FOE1,
		FOE2
	}foe_origin_e;

	typedef enum{
		ADVERSARY_DETECTION_FIABILITY_X			= 0b0001,
		ADVERSARY_DETECTION_FIABILITY_Y			= 0b0010,
		ADVERSARY_DETECTION_FIABILITY_TETA		= 0b0100,
		ADVERSARY_DETECTION_FIABILITY_DISTANCE	= 0b1000,
		ADVERSARY_DETECTION_FIABILITY_ALL		= 0b1111
	}adversary_detection_fiability_e;


	typedef enum{	// Plusieurs erreurs peuvent se cumuler
		AUCUNE_ERREUR				= 0b00000000,	//COMPORTEMENT : le résultat délivré semble bon, il peut être utilisé.

		AUCUN_SIGNAL				= 0b00000001,	//survenue de l'interruption timer 3 car strictement aucun signal reçu depuis au moins deux tours moteurs
													//cette erreur peut se produire si l'on est très loin
													//COMPORTEMENT : pas d'évittement par balise, prise en compte des télémètres !

		SIGNAL_INSUFFISANT			= 0b00000010,	//il peut y avoir un peu de signal, mais pas assez pour estimer une position fiable (se produit typiquement si l'on est trop loin)
													//cette erreur n'est pas grave, on peut considérer que le robot est LOIN !
													//COMPORTEMENT : pas d'évittement, pas de prise en compte des télémètres !

		TACHE_TROP_GRANDE			= 0b00000100,	//Ce cas se produit si trop de récepteurs ont vu du signal.
													// Ce seuil est STRICTEMENT supérieur au cas normal d'un robot très pret. Il y a donc probablement un autre émetteur quelque part, ou on est entouré de miroir.
													//COMPORTEMENT : La position obtenue n'est pas fiable, il faut se référer aux télémètres...

		TROP_DE_SIGNAL				= 0b00001000,	//Le récepteur ayant reçu le plus de signal en à trop recu
													//	cas 1, peu probable, le moteur est bloqué (cas de test facile pour vérifier cette fonctionnalité !)
													//	cas 2, probable, il y a un autre émetteur quelque part !!!
													// 	cas 3, on est dans une enceinte fermée et on capte trop
													//COMPORTEMENT : La position obtenue n'est pas fiable, il faut se référer aux télémètres...

		ERREUR_POSITION_INCOHERENTE = 0b00010000,	//La position obtenue en x/y est incohérente, le robot semble être franchement hors du terrain
													//COMPORTEMENT : si la position obtenue indique qu'il est loin, on ne fait pas d'évitement !
													//sinon, on fait confiance à nos télémètres (?)

		OBSOLESCENCE				= 0b10000000	//La position adverse connue est obsolète compte tenu d'une absence de résultat valide depuis un certain temps.
													//COMPORTEMENT : La position obtenue n'est pas fiable, il faut se référer aux télémètres...
	}beacon_ir_error_e;



	/**********************************************************************************************************************
	 **********************************************************************************************************************
	 **											Types associés à la communication XBEE									 **
	 **********************************************************************************************************************
	 *********************************************************************************************************************/

	typedef enum{
		XBEE_ZONE_TRY_LOCK = 0,		// La réponse de l'autre robot sera envoyé avec XBEE_ZONE_LOCK_RESULT
		XBEE_ZONE_LOCK_RESULT,		// La réponse dans data[2]: TRUE/FALSE suivant si le verouillage à été accepté ou non
		XBEE_ZONE_UNLOCK			// Libère une zone qui a été verouillée
	}xbee_zone_order_e;



	//Différentes zone ou les 2 robots passent
	/*typedef enum {
		MZ_MAMMOUTH_OUR = 0,
		MZ_FRUIT_TRAY,		//Zone du bac à fruit
		MZ_ZONE_LIBRE,
		ZONE_MUTEX_NUMBER
	} map_zone_e;*/

	typedef enum {
		ZONE_OUR_DOORS = 0,
		ZONE_OUR_DUNE,
		ZONE_ADV_DUNE,
		ZONE_ADV_DOORS,
		ZONE_OUR_START_ZONE,
		ZONE_OUR_START,
		ZONE_OUR_DEPOSE,
		ZONE_ADV_DEPOSE,
		ZONE_ADV_START,
		ZONE_ADV_START_ZONE,
		ZONE_OUR_ROCK,
		ZONE_OUR_FISH,
		ZONE_ADV_FISH,
		ZONE_ADV_ROCK,
		NB_ZONES
	} zone_id_e;

	typedef enum{
		EVENT_NO_EVENT	= 0b00000000,
		EVENT_GET_IN	= 0b00000001,
		EVENT_GET_OUT	= 0b00000010,
		EVENT_TIME_IN	= 0b00000100,
		EVENT_SPECIAL	= 0b00001000
	}zone_event_t;

	typedef enum{
		ZONE_NUMBER		//Nombre de zones...
	}zone_e;

	typedef enum{
		NO_AREA = 0,
		AREA_NORTH,
		AREA_SOUTH
	}protect_area_xbee_e;


	/**********************************************************************************************************************
	 **********************************************************************************************************************
	 **												Types associés à l'IHM												 **
	 **********************************************************************************************************************
	 *********************************************************************************************************************/


	typedef enum{
		BATTERY_OFF = 0,
		BATTERY_LOW,
		ARU_ENABLE,
		ARU_DISABLE,
		HOKUYO_POWER_FAIL
	} IHM_power_e;

	// Switch de la carte IHM, pour rajouter des switchs (voir IHM switch.c/h)
	typedef enum{
		BIROUTE_IHM = 0,
		SWITCH_COLOR_IHM,
		SWITCH_LCD_IHM,
		SWITCH0_IHM,
		SWITCH1_IHM,
		SWITCH2_IHM,
		SWITCH3_IHM,
		SWITCH4_IHM,
		SWITCH5_IHM,
		SWITCH6_IHM,
		SWITCH7_IHM,
		SWITCH8_IHM,
		SWITCH9_IHM,
		SWITCH10_IHM,
		SWITCH11_IHM,
		SWITCH12_IHM,
		SWITCH13_IHM,
		SWITCH14_IHM,
		SWITCH15_IHM,
		SWITCH16_IHM,
		SWITCH17_IHM,
		SWITCH18_IHM,
		SWITCHS_NUMBER_IHM
	}switch_ihm_e;

	// Button de la carte ihm
	typedef enum{
		BP_OK_IHM=0,
		BP_UP_IHM,
		BP_DOWN_IHM,
		BP_SET_IHM,
		BP_GO_TO_HOME_IHM,
		BP_CALIBRATION_IHM,
		BP_PRINTMATCH_IHM,
		BP_SUSPEND_RESUME_MATCH_IHM,
		BP_0_IHM,
		BP_1_IHM,
		BP_2_IHM,
		BP_3_IHM,
		BP_4_IHM,
		BP_5_IHM,
		BP_NUMBER_IHM
	}button_ihm_e;

	// Leds de la carte IHM
	typedef enum{
		LED_OK_IHM=0,
		LED_UP_IHM,
		LED_DOWN_IHM,
		LED_SET_IHM,
		LED_COLOR_IHM,
		LED_0_IHM,
		LED_1_IHM,
		LED_2_IHM,
		LED_3_IHM,
		LED_4_IHM,
		LED_5_IHM,
		LED_NUMBER_IHM
	}led_ihm_e;

	// Ne mettre que 8 états max
	// Si rajout état le faire aussi dans la fonction get_blink_state de led.c de la carte IHM
	typedef enum{
		OFF=0,
		ON,
		BLINK_1HZ,
		SPEED_BLINK_4HZ,
		FLASH_BLINK_10MS
	}blinkLED_e;

	typedef enum{
		LED_COLOR_BLACK =		0b000,
		LED_COLOR_BLUE =		0b001,
		LED_COLOR_GREEN =		0b010,
		LED_COLOR_CYAN =		0b011,
		LED_COLOR_RED =			0b100,
		LED_COLOR_MAGENTA =		0b101,
		LED_COLOR_YELLOW =		0b110,
		LED_COLOR_WHITE =		0b111
	}led_color_e;

	typedef struct{
		led_ihm_e id;
		blinkLED_e blink;
	}led_ihm_t;

	typedef enum{
		IHM_ERROR_HOKUYO	= 0b00000001,
		IHM_ERROR_ASSER		= 0b00000010
	}ihm_error_e;

	/**********************************************************************************************************************
	 **********************************************************************************************************************
	 **												Types associés au selftest											 **
	 **********************************************************************************************************************
	 *********************************************************************************************************************/

	typedef enum{
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
		SELFTEST_PROP_SWITCH_ASSER_DISABLE,
		SELFTEST_PROP_LASER_SENSOR_RIGHT,
		SELFTEST_PROP_LASER_SENSOR_LEFT,

		SELFTEST_STRAT_AVOIDANCE_SWITCH_DISABLE,
		SELFTEST_STRAT_XBEE_SWITCH_DISABLE,
		SELFTEST_STRAT_XBEE_DESTINATION_UNREACHABLE,
		SELFTEST_STRAT_RTC,
		SELFTEST_STRAT_BATTERY_NO_24V,
		SELFTEST_STRAT_BATTERY_LOW,
		SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME,
		SELFTEST_STRAT_BIROUTE_FORGOTTEN,
		SELFTEST_STRAT_SD_WRITE_FAIL,
		SELFTEST_STRAT_SWITCH_DISABLE_DUNE,
		SELFTEST_STRAT_SWITCH_DISABLE_SAND_BLOC,
		SELFTEST_STRAT_SWITCH_DISABLE_FISHS,
		SELFTEST_STRAT_SWITCH_DISABLE_DUNIX,

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

		//Black
		SELFTEST_ACT_RX24_FISH_MAGNETIC_ARM,
		SELFTEST_ACT_RX24_FISH_UNSTICK_ARM,
		SELFTEST_ACT_RX24_BLACK_SAND_CIRCLE,
		SELFTEST_ACT_RX24_BOTTOM_DUNE,
		SELFTEST_ACT_RX24_MIDDLE_DUNE,
		SELFTEST_ACT_RX24_CONE_DUNE,
		SELFTEST_ACT_RX24_DUNIX_LEFT,
		SELFTEST_ACT_RX24_DUNIX_RIGHT,
		SELFTEST_ACT_RX24_SAND_LOCKER_LEFT,
		SELFTEST_ACT_RX24_SAND_LOCKER_RIGHT,
		SELFTEST_ACT_RX24_SHIFT_CYLINDER,
		SELFTEST_ACT_RX24_PENDULUM,

		//Pearl
		SELFTEST_ACT_AX12_LEFT_ARM,
		SELFTEST_ACT_AX12_RIGHT_ARM,
		SELFTEST_ACT_AX12_SAND_CIRCLE,
		SELFTEST_ACT_AX12_PARASOL,
		SELFTEST_ACT_POMPE_FRONT_LEFT,
		SELFTEST_ACT_POMPE_FRONT_RIGHT,

		SELFTEST_ERROR_NB,
		SELFTEST_ERROR =0xFE,
		SELFTEST_NO_ERROR = 0xFF
		//... ajouter ici d'éventuels nouveaux code d'erreur.
	}SELFTEST_error_code_e;


/**********************************************************************************************************************
 **********************************************************************************************************************
 **											Types associés à l'actionneur											 **
 **********************************************************************************************************************
 *********************************************************************************************************************/

	typedef enum{
		NB_ACT_SENSOR = 1
	}act_sensor_id_e;

// Mettre toujours l'ordre de STOP à la valeur 0 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	typedef enum {

// Ne pas toucher

		ACT_DEFAULT_STOP = 0,
		ACT_CONFIG = 0xFF,

//QS_mosfet
		ACT_MOSFET_STOP = 0,
		ACT_MOSFET_NORMAL,

// Black
		ACT_FISH_MAGNETIC_ARM_STOP = 0,
		ACT_FISH_MAGNETIC_ARM_IDLE,
		ACT_FISH_MAGNETIC_ARM_MIDDLE,
		ACT_FISH_MAGNETIC_ARM_OPEN,
		ACT_FISH_MAGNETIC_ARM_CLOSE,

		ACT_FISH_UNSTICK_ARM_STOP = 0,
		ACT_FISH_UNSTICK_ARM_IDLE,
		ACT_FISH_UNSTICK_ARM_OPEN,
		ACT_FISH_UNSTICK_ARM_CLOSE,

		ACT_BLACK_SAND_CIRCLE_STOP = 0,
		ACT_BLACK_SAND_CIRCLE_IDLE,
		ACT_BLACK_SAND_CIRCLE_LOCK,
		ACT_BLACK_SAND_CIRCLE_UNLOCK,

		ACT_BOTTOM_DUNE_STOP = 0,
		ACT_BOTTOM_DUNE_IDLE,
		ACT_BOTTOM_DUNE_LOCK,
		ACT_BOTTOM_DUNE_UNLOCK,
		ACT_BOTTOM_DUNE_MID,

		ACT_MIDDLE_DUNE_STOP = 0,
		ACT_MIDDLE_DUNE_IDLE,
		ACT_MIDDLE_DUNE_LOCK,
		ACT_MIDDLE_DUNE_UNLOCK,

		ACT_CONE_DUNE_STOP = 0,
		ACT_CONE_DUNE_IDLE,
		ACT_CONE_DUNE_LOCK,
		ACT_CONE_DUNE_UNLOCK,

		ACT_DUNIX_LEFT_STOP = 0,
		ACT_DUNIX_LEFT_IDLE,
		ACT_DUNIX_LEFT_OPEN,
		ACT_DUNIX_LEFT_CLOSE,

		ACT_DUNIX_RIGHT_STOP = 0,
		ACT_DUNIX_RIGHT_IDLE,
		ACT_DUNIX_RIGHT_OPEN,
		ACT_DUNIX_RIGHT_CLOSE,

		ACT_SAND_LOCKER_LEFT_STOP = 0,
		ACT_SAND_LOCKER_LEFT_IDLE,
		ACT_SAND_LOCKER_LEFT_LOCK,
		ACT_SAND_LOCKER_LEFT_MIDDLE,
		ACT_SAND_LOCKER_LEFT_LOCK_BLOCK,
		ACT_SAND_LOCKER_LEFT_LOCK_BLOCK_SERRAGE,
		ACT_SAND_LOCKER_LEFT_UNLOCK,

		ACT_SAND_LOCKER_RIGHT_STOP = 0,
		ACT_SAND_LOCKER_RIGHT_IDLE,
		ACT_SAND_LOCKER_RIGHT_LOCK,
		ACT_SAND_LOCKER_RIGHT_MIDDLE,
		ACT_SAND_LOCKER_RIGHT_LOCK_BLOCK,
		ACT_SAND_LOCKER_RIGHT_LOCK_BLOCK_SERRAGE,
		ACT_SAND_LOCKER_RIGHT_UNLOCK,

		ACT_SHIFT_CYLINDER_STOP = 0,
		ACT_SHIFT_CYLINDER_IDLE,
		ACT_SHIFT_CYLINDER_OPEN,
		ACT_SHIFT_CYLINDER_CLOSE,

		ACT_PENDULUM_STOP = 0,
		ACT_PENDULUM_IDLE,
		ACT_PENDULUM_OPEN,
		ACT_PENDULUM_CLOSE,

// Pearl
		ACT_LEFT_ARM_STOP = 0,
		ACT_LEFT_ARM_IDLE,
		ACT_LEFT_ARM_UNLOCK,
		ACT_LEFT_ARM_LOCK,
		ACT_LEFT_ARM_LOCK_SERRAGE,

		ACT_RIGHT_ARM_STOP = 0,
		ACT_RIGHT_ARM_IDLE,
		ACT_RIGHT_ARM_UNLOCK,
		ACT_RIGHT_ARM_LOCK,
		ACT_RIGHT_ARM_LOCK_SERRAGE,

		ACT_CIRCLE_STOP = 0,
		ACT_CIRCLE_IDLE,
		ACT_CIRCLE_OPEN,
		ACT_CIRCLE_CLOSE,

		ACT_PEARL_SAND_CIRCLE_STOP = 0,
		ACT_PEARL_SAND_CIRCLE_IDLE,
		ACT_PEARL_SAND_CIRCLE_LOCK,
		ACT_PEARL_SAND_CIRCLE_UNLOCK,

		ACT_PARASOL_STOP = 0,
		ACT_PARASOL_IDLE,
		ACT_PARASOL_OPEN,
		ACT_PARASOL_CLOSE,

//Pompes

		ACT_POMPE_STOP = 0,
		ACT_POMPE_NORMAL,
		ACT_POMPE_REVERSE,

		ACT_POMPE_ALL_STOP = 0,
		ACT_POMPE_ALL_NORMAL_5 = 0b01111100,
		ACT_POMPE_ALL_NORMAL_7 = 0b11111111

	} ACT_order_e;

	typedef enum{
		ACT_RESULT_DONE = 0,			//Tout s'est bien passé
		ACT_RESULT_FAILED,				//La commande s'est mal passé et on ne sait pas dans quel état est l'actionneur (par: les capteurs ne fonctionnent plus)
		ACT_RESULT_NOT_HANDLED			//La commande ne s'est pas effectué correctement et on sait que l'actionneur n'a pas bougé (ou quand la commande a été ignorée)
	}act_result_state_e;

	typedef enum{
		ACT_RESULT_ERROR_OK = 0,		//Quand pas d'erreur
		ACT_RESULT_ERROR_TIMEOUT,		//Il y a eu timeout, par ex l'asservissement prend trop de temps
		ACT_RESULT_ERROR_OTHER,			//La commande était lié à une autre qui ne s'est pas effectué correctement, utilisé avec ACT_RESULT_NOT_HANDLED
		ACT_RESULT_ERROR_NOT_HERE,		//L'actionneur ou le capteur ne répond plus (on le sait par un autre moyen que le timeout, par exemple l'AX12 ne renvoie plus d'info après l'envoi d'une commande.)
		ACT_RESULT_ERROR_LOGIC,			//Erreur de logique interne à la carte actionneur, probablement une erreur de codage (par exemple un état qui n'aurait jamais dû arrivé)
		ACT_RESULT_ERROR_NO_RESOURCES,	//La carte n'a pas assez de resource pour gérer la commande. Commande à renvoyer plus tard.
		ACT_RESULT_ERROR_INVALID_ARG,	//La commande ne peut pas être effectuée, l'argument n'est pas valide ou est en dehors des valeurs acceptés
		ACT_RESULT_ERROR_CANCELED,		//L'action a été annulé

		ACT_RESULT_ERROR_UNKNOWN = 255	//Erreur inconnue ou qui ne correspond pas aux précédentes.

	}act_result_error_code_e;

	typedef enum{
		SPEED_CONFIG = 0,
		TORQUE_CONFIG
	}act_config_e;

	typedef enum{
		DEFAULT_MONO_ACT = 0
		/*EXEMPLE
		SUB_ACT0_0,
		SUB_ACT0_1,
		SUB_ACT0_2,
		SUB_ACT0_3,

		SUB_ACT1_0,
		SUB_ACT1_1,
		SUB_ACT1_2,
		SUB_ACT1_3,

		SUB_ACT2_0,
		SUB_ACT2_1,
		SUB_ACT2_2,
		SUB_ACT2_3*/

	}act_sub_act_id_e;


#endif /* ndef QS_TYPES_H */
