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

	#define BOT_COLOR_NAME "BLUE"
	#define TOP_COLOR_NAME "YELLOW"

	typedef enum
	{
		BOT_COLOR = 0, BLUE=0,
		TOP_COLOR = 1, YELLOW=1
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

	typedef enum
	{
		STATE_BLACK_FOR_COM_INIT,		//est utilisé pour l'appel sans évènement
		STATE_BLACK_FOR_COM_WAIT,
		STATE_BLACK_FOR_COM_RUSH,
		STATE_BLACK_FOR_COM_WAIT_ADV,
		STATE_BLACK_FOR_COM_TAKING_DUNE,
		STATE_BLACK_FOR_COM_COMING_BACK,
		STATE_BLACK_FOR_COM_DISPOSE,
		STATE_BLACK_FOR_COM_FISH,
		STATE_BLACK_FOR_COM_CENTER_SOUTH,
		STATE_BLACK_FOR_COM_OUR_BLOC,
		STATE_BLACK_FOR_COM_NB
	}state_black_for_com_e;


	typedef enum{
			//F_EXEMPLE
			FLAG_OUR_NORTH_CRATER_IS_TAKEN,
			FLAG_OUR_SOUTH_CRATER_IS_TAKEN,
			FLAG_ADV_NORTH_CRATER_IS_TAKEN,
			FLAG_ADV_SOUTH_CRATER_IS_TAKEN,

			FLAG_OUR_CORNER_CRATER_IS_TAKEN,
			FLAG_ADV_CORNER_CRATER_IS_TAKEN,
			FLAG_OUR_CORNER_MIDDLE_CRATER_IS_TAKEN,
			FLAG_ADV_CORNER_MIDDLE_CRATER_IS_TAKEN,
			FLAG_OUR_CORNER_ROCKET_CRATER_IS_TAKEN,
			FLAG_ADV_CORNER_ROCKET_CRATER_IS_TAKEN,
			FLAG_OUR_CORNER_CORNER_CRATER_IS_TAKEN,
			FLAG_ADV_CORNER_CORNER_CRATER_IS_TAKEN,


			FLAG_SHUTTLE_IS_FULL,

			FLAG_OUR_UNICOLOR_ROCKET_IS_FULL,
			FLAG_ADV_UNICOLOR_ROCKET_IS_FULL, //ça peut se voir au laser et ça peut servir sur de la strategie très fine mais j'en doute
			FLAG_OUR_MULTICOLOR_ROCKET_IS_FULL,
			FLAG_ADV_MULTICOLOR_ROCKET_IS_FULL,

			FLAG_OUR_MULTICOLOR_START_IS_TAKEN,
			FLAG_OUR_MULTICOLOR_SIDE_IS_TAKEN,
			FLAG_OUR_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN,
			FLAG_ADV_MULTICOLOR_START_IS_TAKEN,
			FLAG_ADV_MULTICOLOR_SIDE_IS_TAKEN,
			FLAG_ADV_MULTICOLOR_NEAR_DEPOSE_IS_TAKEN,
			FLAG_OUR_UNICOLOR_NORTH_IS_TAKEN,
			FLAG_OUR_UNICOLOR_SOUTH_IS_TAKEN,
			FLAG_ADV_UNICOLOR_NORTH_IS_TAKEN, //... laser
			FLAG_ADV_UNICOLOR_SOUTH_IS_TAKEN, //... laser

			//Flag de subaction pour éviter les collisions

			FLAG_SUB_HARRY_OUR_NORTH_CRATER,
			FLAG_SUB_HARRY_ADV_NORTH_CRATER,
			FLAG_SUB_HARRY_OUR_SOUTH_CRATER,
			FLAG_SUB_HARRY_ADV_SOUTH_CRATER,
			FLAG_SUB_HARRY_OUR_CORNER_CRATER,
			FLAG_SUB_HARRY_ADV_CORNER_CRATER,
			FLAG_SUB_HARRY_DEPOSE_CYLINDER_OUR_SIDE,
			FLAG_SUB_HARRY_DEPOSE_CYLINDER_ADV_SIDE,
			FLAG_SUB_HARRY_ORE_SHOOTING,
			FLAG_SUB_HARRY_DEPOSE_CYLINDER_OUR_DIAGONAL,
			FLAG_SUB_HARRY_DEPOSE_CYLINDER_CENTER,
			FLAG_SUB_HARRY_DEPOSE_CYLINDER_ADV_DIAGONAL,
			FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_UNI,
			FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_ROCKET_MULTI,
			FLAG_SUB_HARRY_TAKE_CYLINDER_ADV_ROCKET_MULTI,
			FLAG_SUB_HARRY_TAKE_CYLINDER_OUR_CENTER,
			FLAG_SUB_HARRY_TAKE_CYLINDER_ADV_CENTER,
			FLAG_SUB_HARRY_TAKE_CYLINDER_NORTH_UNI,
			FLAG_SUB_HARRY_TAKE_CYLINDER_SOUTH_UNI,


			//FLAG_SUB_ANNE_NORTH_CRATER,
			//FLAG_SUB_ANNE_SOUTH_CRATER,
			//FLAG_SUB_ANNE_CORNER_CRATER,
			FLAG_SUB_ANNE_DEPOSE_CYLINDER_OUR_SIDE,
			FLAG_SUB_ANNE_DEPOSE_CYLINDER_ADV_SIDE,
			//FLAG_SUB_ANNE_ORE_SHOOTING,
			FLAG_SUB_ANNE_DEPOSE_CYLINDER_OUR_DIAGONAL,
			FLAG_SUB_ANNE_DEPOSE_CYLINDER_CENTER,
			FLAG_SUB_ANNE_DEPOSE_CYLINDER_ADV_DIAGONAL,
			FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_UNI,
			FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_ROCKET_MULTI,
			FLAG_SUB_ANNE_TAKE_CYLINDER_ADV_ROCKET_MULTI,
			FLAG_SUB_ANNE_TAKE_CYLINDER_OUR_CENTER,
			FLAG_SUB_ANNE_TAKE_CYLINDER_ADV_CENTER,
			FLAG_SUB_ANNE_TAKE_CYLINDER_NORTH_UNI,
			FLAG_SUB_ANNE_TAKE_CYLINDER_SOUTH_UNI,


			F_ELEMENTS_FLAGS_END_SYNCH,	//Les flags au-dessus seront synchro entre les deux robots

			//Flags pour savoir si la comm passe entre les deux robots
			//Ce flag est envoyé à intervalle de temps régulier
			//Ne pas mettre ce flag dans la partie synchro, ce flag est synchro par une machine à état spécifique
			F_COMMUNICATION_AVAILABLE,

			// Début des hardflags
			F_ELEMENTS_HARDFLAGS_START,

			// Fin des hardflags
			F_ELEMENTS_HRDFLAGS_END,

			//Eléments pris (non synchro)
			FLAG_HARRY_STOMACH_IS_FULL, 		//le reservoir de baballe
			FLAG_HARRY_DISABLE_MODULE_RIGHT,	// Ce flag est levé lorsqu'une désactivation de l'actionneur à eu lieu (après une erreur)
			FLAG_HARRY_DISABLE_MODULE_LEFT,		// Ce flag est levé lorsqu'une désactivation de l'actionneur à eu lieu (après une erreur)
			FLAG_HARRY_MODULE_COLOR_RIGHT_SUCCESS,	// Flag levé lorsqu'un module a été retourné de la bonne couleur
			FLAG_HARRY_MODULE_COLOR_LEFT_SUCCESS,  	// Flag levé lorsqu'un module a été retourné de la bonne couleur
			FLAG_HARRY_MODULE_COLOR_RIGHT_FINISH,	// Flag levé lorsque la mae de préparation des modules pour la dépose est terminé
			FLAG_HARRY_MODULE_COLOR_LEFT_FINISH,  	// Flag levé lorsque la mae de préparation des modules pour la dépose est terminé
			FLAG_HARRY_MODULE_COLOR_RIGHT_FALLEN,	// Flag levé lorsque la mae de préparation des modules pour la dépose à descendu le module suivant
			FLAG_HARRY_MODULE_COLOR_LEFT_FALLEN,  	// Flag levé lorsque la mae de préparation des modules pour la dépose à descendu le module suivant
			FLAG_HARRY_STORAGE_LEFT_FINISH,			// Flag levé lorsque que la subaction de stockage à gauche est terminé
			FLAG_HARRY_STORAGE_RIGHT_FINISH,		// Flag levé lorsque que la subaction de stockage à droite est terminé


			FLAG_ANNE_STOMACH_IS_FULL, 				//le reservoir de baballe
			FLAG_ANNE_DISABLE_MODULE,				// Ce flag est levé lorsqu'une désactivation de l'actionneur à eu lieu (après une erreur)
			FLAG_ANNE_MODULE_COLOR_SUCCESS,			// Flag levé lorsqu'un module a été retourné de la bonne couleur
			FLAG_ANNE_MODULE_COLOR_FINISH,			// Flag levé lorsque la mae de préparation des modules pour la dépose est terminé
			FLAG_RETURN_CENTRAL_MOONBASIS,
			FLAG_RETURN_OUR_CENTRAL_MOONBASIS,
			FLAG_RETURN_ADV_CENTRAL_MOONBASIS,
			FLAG_RETURN_OUR_SIDE_MOONBASIS,
			FLAG_RETURN_ADV_SIDE_MOONBASIS,
			FLAG_SMALL_BALANCER_FINISH,

			F_ELEMENTS_FLAGS_NB

		}elements_flags_e;

	/**********************************************************************************************************************
	 **********************************************************************************************************************
	 **											Types associés à la propulsion											 **
	 **********************************************************************************************************************
	 *********************************************************************************************************************/


	/*sens de trajectoire - utilisé dans le code propulsion et partagé pour la stratégie... */
	typedef enum {
		ANY_WAY=0,

	// Translation
		BACKWARD,
		FORWARD,

	// Rotation
		CLOCKWISE = BACKWARD,
		TRIGOWISE = FORWARD
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
		CUSTOM,	//Les valeurs suivantes sont également valables (jusqu'à 255... et indiquent un choix de vitesse personnalisé !)
		MAX_SPEED = 0x03FF
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
		PROP_END_AT_POINT = 0,
		PROP_END_AT_BRAKE
	}propEndCondition_e;

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
		CORRECTOR_ENABLE = 0,
		CORRECTOR_ROTATION_ONLY,
		CORRECTOR_TRANSLATION_ONLY,
		CORRECTOR_DISABLE
	}corrector_e;


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
		XBEE_ZONE_UNLOCK,			// Libère une zone qui a été verouillée
		XBEE_ZONE_LOCK
	}xbee_zone_order_e;



	//Différentes zone ou les 2 robots passent
	/*typedef enum {
		MZ_MAMMOUTH_OUR = 0,
		MZ_FRUIT_TRAY,		//Zone du bac à fruit
		MZ_ZONE_LIBRE,
		ZONE_MUTEX_NUMBER
	} map_zone_e;*/

	typedef enum {
		ZONE_NO_ZONE = 0,
		ZONE_OUR_DOORS = 1,
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
		EVENT_TIME_IN	= 0b00000100
	} ZONE_event_t;

	typedef enum{
		ZONE_MIDDLE,
		ZONE_BOTTOM,
		ZONE_NUMBER		//Nombre de zones...
	} ZONE_zoneId_e;

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
		BATTERY_DISABLE 	= 0b000000001,
		BATTERY_ENABLE		= 0b000000010,
		BATTERY_LOW			= 0b000000100,
		ARU_DISABLE			= 0b000001000,
		ARU_ENABLE			= 0b000010000,
		HOKUYO_DISABLE		= 0b000100000,
		HOKUYO_ENABLE		= 0b001000000,
		POWER_AVAILABLE		= 0b010000000,
		POWER_NO_AVAILABLE	= 0b100000000
	}alim_state_e;

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
		BP_SELFTEST_IHM=0,
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
		SELFTEST_PROP_LASER_SENSOR_LEFT, //0xF

		SELFTEST_STRAT_XBEE_SWITCH_DISABLE,
		SELFTEST_STRAT_XBEE_DESTINATION_UNREACHABLE,
		SELFTEST_STRAT_RTC,
		SELFTEST_STRAT_BATTERY_NO_24V,
		SELFTEST_STRAT_BATTERY_LOW,
		SELFTEST_STRAT_WHO_AM_I_ARE_NOT_THE_SAME,
		SELFTEST_STRAT_BIROUTE_FORGOTTEN,
		SELFTEST_STRAT_SD_WRITE_FAIL,
		SELFTEST_STRAT_SWITCH_DISABLE_MODULE_LEFT,
		SELFTEST_STRAT_SWITCH_DISABLE_MODULE_RIGHT,
		SELFTEST_STRAT_SWITCH_DISABLE_ORE,

		SELFTEST_IHM_BATTERY_NO_24V,
		SELFTEST_IHM_BATTERY_LOW,
		SELFTEST_IHM_BIROUTE_FORGOTTEN, //0x1F
		SELFTEST_IHM_POWER_HOKUYO_FAILED,

		SELFTEST_ACT_UNREACHABLE,
		SELFTEST_PROP_UNREACHABLE,
		SELFTEST_BEACON_UNREACHABLE,
		SELFTEST_IHM_UNREACHABLE,

		// Self test de la carte actionneur (si actionneur indiqué, alors il n'a pas fonctionné comme prévu, pour plus d'info voir la sortie uart de la carte actionneur) :
		SELFTEST_ACT_MISSING_TEST,	//Test manquant après un timeout du selftest actionneur, certains actionneur n'ont pas le selftest d'implémenté ou n'ont pas terminé leur action (ou plus rarement, la pile était pleine et le selftest n'a pas pu se faire)
		SELFTEST_ACT_UNKNOWN_ACT,	//Un actionneur inconnu a fail son selftest. Pour avoir le nom, ajoutez un SELFTEST_ACT_xxx ici et gérez l'actionneur dans selftest.c de la carte actionneur
		SELFTEST_ACT_SIMU,			//Actionneur en mode simu (ROBOT_VIRTUEL_PARFAIT)

        //Harry
        SELFTEST_ACT_AX12_BEARING_BALL_WHEEL,
        SELFTEST_ACT_AX12_BIG_BALL_BACK_LEFT,
        SELFTEST_ACT_AX12_BIG_BALL_BACK_RIGHT,
        SELFTEST_ACT_AX12_BIG_BALL_FRONT_LEFT,
        SELFTEST_ACT_AX12_BIG_BALL_FRONT_RIGHT,

        SELFTEST_ACT_AX12_CYLINDER_BALANCER_LEFT,
        SELFTEST_ACT_AX12_CYLINDER_BALANCER_RIGHT,
        SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_LEFT,
        SELFTEST_ACT_RX24_CYLINDER_ELEVATOR_RIGHT,
        SELFTEST_ACT_AX12_CYLINDER_PUSHER_LEFT,
        SELFTEST_ACT_AX12_CYLINDER_PUSHER_RIGHT,
        SELFTEST_ACT_RX24_CYLINDER_SLIDER_LEFT,
        SELFTEST_ACT_RX24_CYLINDER_SLIDER_RIGHT,
        SELFTEST_ACT_AX12_CYLINDER_SLOPE_LEFT,
        SELFTEST_ACT_AX12_CYLINDER_SLOPE_RIGHT,
        SELFTEST_ACT_RX24_CYLINDER_ARM_LEFT,
        SELFTEST_ACT_RX24_CYLINDER_ARM_RIGHT,
		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_RIGHT,
		SELFTEST_ACT_AX12_CYLINDER_DISPOSE_LEFT,
		SELFTEST_ACT_RX24_CYLINDER_COLOR_RIGHT,
		SELFTEST_ACT_RX24_CYLINDER_COLOR_LEFT,
        SELFTEST_ACT_RX24_ORE_GUN,
        SELFTEST_ACT_RX24_ORE_ROLLER_ARM,
		SELFTEST_ACT_RX24_ORE_FOAM,
        SELFTEST_ACT_RX24_ORE_WALL,
		SELFTEST_ACT_RX24_ORE_TRIHOLE,
        SELFTEST_ACT_AX12_ROCKET,

		SELFTEST_ACT_TURBINE,
		SELFTEST_ACT_POMPE_SLIDER_RIGHT,
		SELFTEST_ACT_POMPE_SLIDER_LEFT,
		SELFTEST_ACT_POMPE_ELEVATOR_RIGHT,
		SELFTEST_ACT_POMPE_ELEVATOR_LEFT,
		SELFTEST_ACT_POMPE_DISPOSE_RIGHT,
		SELFTEST_ACT_POMPE_DISPOSE_LEFT,

		//Anne
		SELFTEST_ACT_AX12_SMALL_BALL_BACK,
		SELFTEST_ACT_AX12_SMALL_BALL_FRONT_LEFT,
		SELFTEST_ACT_AX12_SMALL_BALL_FRONT_RIGHT,

		SELFTEST_ACT_AX12_SMALL_CYLINDER_SLIDER,
		SELFTEST_ACT_AX12_SMALL_CYLINDER_ELEVATOR,
		SELFTEST_ACT_AX12_SMALL_CYLINDER_SLOPE,
		SELFTEST_ACT_AX12_SMALL_CYLINDER_BALANCER,
		SELFTEST_ACT_AX12_SMALL_CYLINDER_COLOR,
		SELFTEST_ACT_AX12_SMALL_CYLINDER_ARM,
		SELFTEST_ACT_AX12_SMALL_CYLINDER_DISPOSE,
		SELFTEST_ACT_AX12_SMALL_CYLINDER_MULTIFONCTION,
		SELFTEST_ACT_AX12_SMALL_POMPE_PRISE,
		SELFTEST_ACT_AX12_SMALL_POMPE_DISPOSE,

		SELFTEST_ACT_AX12_SMALL_MAGIC_ARM,
		SELFTEST_ACT_AX12_SMALL_MAGIC_COLOR,

		SELFTEST_ACT_AX12_SMALL_ORE,
		SELFTEST_ACT_AX12_SMALL_POUSSIX,



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
		WT100_CENTER_BOTTOM,
		NB_ACT_SENSOR
	}act_sensor_id_e;

// Mettre toujours l'ordre de STOP à la valeur 0 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	typedef enum {

// Ne pas toucher

		ACT_DEFAULT_STOP = 0,
		ACT_WARNER = 0xFD,
		ACT_GET_CONFIG = 0xFE,
		ACT_SET_CONFIG = 0xFF,

//QS_mosfet
		ACT_MOSFET_STOP = 0,
		ACT_MOSFET_NORMAL,

// Harry



        ACT_BIG_BALL_FRONT_LEFT_STOP = 0,
        ACT_BIG_BALL_FRONT_LEFT_IDLE,
        ACT_BIG_BALL_FRONT_LEFT_DOWN,
        ACT_BIG_BALL_FRONT_LEFT_UP,

        ACT_BIG_BALL_FRONT_RIGHT_STOP = 0,
        ACT_BIG_BALL_FRONT_RIGHT_IDLE,
        ACT_BIG_BALL_FRONT_RIGHT_DOWN,
        ACT_BIG_BALL_FRONT_RIGHT_UP,

        ACT_BIG_BALL_BACK_LEFT_STOP = 0,
        ACT_BIG_BALL_BACK_LEFT_IDLE,
        ACT_BIG_BALL_BACK_LEFT_DOWN,
        ACT_BIG_BALL_BACK_LEFT_UP,

        ACT_BIG_BALL_BACK_RIGHT_STOP = 0,
        ACT_BIG_BALL_BACK_RIGHT_IDLE,
        ACT_BIG_BALL_BACK_RIGHT_DOWN,
        ACT_BIG_BALL_BACK_RIGHT_UP,

		ACT_CYLINDER_SLOPE_RIGHT_STOP = 0,
		ACT_CYLINDER_SLOPE_RIGHT_IDLE,
		ACT_CYLINDER_SLOPE_RIGHT_DOWN,
		ACT_CYLINDER_SLOPE_RIGHT_UP,
		ACT_CYLINDER_SLOPE_RIGHT_VERY_UP,

		ACT_CYLINDER_SLOPE_LEFT_STOP = 0,
		ACT_CYLINDER_SLOPE_LEFT_IDLE,
		ACT_CYLINDER_SLOPE_LEFT_DOWN,
		ACT_CYLINDER_SLOPE_LEFT_UP,
		ACT_CYLINDER_SLOPE_LEFT_VERY_UP,

		ACT_CYLINDER_BALANCER_RIGHT_STOP = 0,
		ACT_CYLINDER_BALANCER_RIGHT_IDLE,
		ACT_CYLINDER_BALANCER_RIGHT_IN,
		ACT_CYLINDER_BALANCER_RIGHT_OUT,
		ACT_CYLINDER_BALANCER_RIGHT_VERY_OUT,

		ACT_CYLINDER_BALANCER_LEFT_STOP = 0,
		ACT_CYLINDER_BALANCER_LEFT_IDLE,
		ACT_CYLINDER_BALANCER_LEFT_IN,
		ACT_CYLINDER_BALANCER_LEFT_OUT,
		ACT_CYLINDER_BALANCER_LEFT_VERY_OUT,

        ACT_BEARING_BALL_WHEEL_STOP = 0,
        ACT_BEARING_BALL_WHEEL_IDLE,
        ACT_BEARING_BALL_WHEEL_DOWN,
        ACT_BEARING_BALL_WHEEL_UP,

        ACT_CYLINDER_PUSHER_RIGHT_STOP = 0,
        ACT_CYLINDER_PUSHER_RIGHT_IDLE,
        ACT_CYLINDER_PUSHER_RIGHT_IN,
        ACT_CYLINDER_PUSHER_RIGHT_OUT,
		//ACT_CYLINDER_PUSHER_RIGHT_DEPOSE,
		ACT_CYLINDER_PUSHER_RIGHT_HIT,
		ACT_CYLINDER_PUSHER_RIGHT_PREVENT_DEPOSE,

        ACT_CYLINDER_PUSHER_LEFT_STOP = 0,
        ACT_CYLINDER_PUSHER_LEFT_IDLE,
        ACT_CYLINDER_PUSHER_LEFT_IN,
        ACT_CYLINDER_PUSHER_LEFT_OUT,
		//ACT_CYLINDER_PUSHER_LEFT_DEPOSE,
		ACT_CYLINDER_PUSHER_LEFT_HIT,
		ACT_CYLINDER_PUSHER_LEFT_PREVENT_DEPOSE,

        ACT_CYLINDER_ELEVATOR_RIGHT_STOP = 0,
        ACT_CYLINDER_ELEVATOR_RIGHT_IDLE,
        ACT_CYLINDER_ELEVATOR_RIGHT_TOP,
		ACT_CYLINDER_ELEVATOR_RIGHT_LOCK_WITH_CYLINDER,
        ACT_CYLINDER_ELEVATOR_RIGHT_BOTTOM,
		ACT_CYLINDER_ELEVATOR_RIGHT_WARNER,

        ACT_CYLINDER_ELEVATOR_LEFT_STOP = 0,
        ACT_CYLINDER_ELEVATOR_LEFT_IDLE,
        ACT_CYLINDER_ELEVATOR_LEFT_TOP,
		ACT_CYLINDER_ELEVATOR_LEFT_LOCK_WITH_CYLINDER,
        ACT_CYLINDER_ELEVATOR_LEFT_BOTTOM,
		ACT_CYLINDER_ELEVATOR_LEFT_WARNER,

        ACT_CYLINDER_SLIDER_RIGHT_STOP = 0,
        ACT_CYLINDER_SLIDER_RIGHT_IDLE,
        ACT_CYLINDER_SLIDER_RIGHT_IN,
        ACT_CYLINDER_SLIDER_RIGHT_OUT,
        ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT,
		ACT_CYLINDER_SLIDER_RIGHT_ALMOST_OUT_WITH_CYLINDER,
		ACT_CYLINDER_SLIDER_LEFT_HARVEST,
		ACT_CYLINDER_SLIDER_RIGHT_WARNER,

        ACT_CYLINDER_SLIDER_LEFT_STOP = 0,
        ACT_CYLINDER_SLIDER_LEFT_IDLE,
        ACT_CYLINDER_SLIDER_LEFT_IN,
        ACT_CYLINDER_SLIDER_LEFT_OUT,
        ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT,
		ACT_CYLINDER_SLIDER_LEFT_ALMOST_OUT_WITH_CYLINDER,
		ACT_CYLINDER_SLIDER_RIGHT_HARVEST,
		ACT_CYLINDER_SLIDER_LEFT_WARNER,

        ACT_CYLINDER_ARM_RIGHT_STOP = 0,
        ACT_CYLINDER_ARM_RIGHT_IDLE,
        ACT_CYLINDER_ARM_RIGHT_IN,				// Position intiale
		ACT_CYLINDER_ARM_RIGHT_PREPARE_TO_TAKE,	// Position prêt à prendre
		ACT_CYLINDER_ARM_RIGHT_TAKE,			// Position de prise
		ACT_CYLINDER_ARM_RIGHT_DISPOSE,			// Position de dépose
        ACT_CYLINDER_ARM_RIGHT_OUT,				// Position de sortie max (lorsqu'on sort le cylindre du robot)

        ACT_CYLINDER_ARM_LEFT_STOP = 0,
        ACT_CYLINDER_ARM_LEFT_IDLE,
        ACT_CYLINDER_ARM_LEFT_IN,				// Position intiale
		ACT_CYLINDER_ARM_LEFT_PREPARE_TO_TAKE,  // Position prêt à prendre
		ACT_CYLINDER_ARM_LEFT_TAKE,				// Position de prise
		ACT_CYLINDER_ARM_LEFT_DISPOSE,			// Position de dépose
        ACT_CYLINDER_ARM_LEFT_OUT,				// Position de sortie max (lorsqu'on sort le cylindre du robot)

		ACT_CYLINDER_COLOR_RIGHT_STOP = 0,		// A ne surtout pas utiliser pour mettre la vitesse du servo à 0%
		ACT_CYLINDER_COLOR_RIGHT_IDLE,
		ACT_CYLINDER_COLOR_RIGHT_NORMAL_SPEED,
		ACT_CYLINDER_COLOR_RIGHT_ZERO_SPEED,

		ACT_CYLINDER_COLOR_LEFT_STOP = 0,		// A ne surtout pas utiliser pour mettre la vitesse du servo à 0%
		ACT_CYLINDER_COLOR_LEFT_IDLE,
		ACT_CYLINDER_COLOR_LEFT_NORMAL_SPEED,
		ACT_CYLINDER_COLOR_LEFT_ZERO_SPEED,

		ACT_CYLINDER_DISPOSE_RIGHT_STOP = 0,
		ACT_CYLINDER_DISPOSE_RIGHT_IDLE,
		ACT_CYLINDER_DISPOSE_RIGHT_TAKE,	// Position de prise
		ACT_CYLINDER_DISPOSE_RIGHT_RAISE,	// Position où on lève légèrement le cylindre avant de la sortir du robot
		ACT_CYLINDER_DISPOSE_RIGHT_DISPOSE, // Position de dépose

		ACT_CYLINDER_DISPOSE_LEFT_STOP = 0,
		ACT_CYLINDER_DISPOSE_LEFT_IDLE,
		ACT_CYLINDER_DISPOSE_LEFT_TAKE,		// Position de prise
		ACT_CYLINDER_DISPOSE_LEFT_RAISE,	// Position où on lève légèrement le cylindre avant de la sortir du robot
		ACT_CYLINDER_DISPOSE_LEFT_DISPOSE,	// Position de dépose

        ACT_ORE_WALL_STOP = 0,
        ACT_ORE_WALL_IDLE,
        ACT_ORE_WALL_OUT,
		ACT_ORE_WALL_IN,

        ACT_ORE_ROLLER_ARM_STOP = 0,
        ACT_ORE_ROLLER_ARM_IDLE,
        ACT_ORE_ROLLER_ARM_OUT,
        ACT_ORE_ROLLER_ARM_IN,
        ACT_ORE_ROLLER_ARM_DEPOSE,


		ACT_ORE_ROLLER_FOAM_STOP = 0,
		ACT_ORE_ROLLER_FOAM_IDLE,
		ACT_ORE_ROLLER_FOAM_RUN,

		ACT_ORE_TRIHOLE_STOP = 0,
		ACT_ORE_TRIHOLE_IDLE,
		ACT_ORE_TRIHOLE_RUN,

		ACT_ORE_GUN_STOP = 0,
		ACT_ORE_GUN_IDLE,
        ACT_ORE_GUN_DOWN,
        ACT_ORE_GUN_UP,

        ACT_ROCKET_STOP = 0,
        ACT_ROCKET_IDLE,
        ACT_ROCKET_LAUNCH,
		ACT_ROCKET_PREPARE_LAUNCH,

// Anne
        ACT_SMALL_BALL_FRONT_LEFT_STOP = 0,
        ACT_SMALL_BALL_FRONT_LEFT_IDLE,
        ACT_SMALL_BALL_FRONT_LEFT_DOWN,
        ACT_SMALL_BALL_FRONT_LEFT_UP,

        ACT_SMALL_BALL_FRONT_RIGHT_STOP = 0,
        ACT_SMALL_BALL_FRONT_RIGHT_IDLE,
        ACT_SMALL_BALL_FRONT_RIGHT_DOWN,
        ACT_SMALL_BALL_FRONT_RIGHT_UP,

        ACT_SMALL_BALL_BACK_STOP = 0,
        ACT_SMALL_BALL_BACK_IDLE,
        ACT_SMALL_BALL_BACK_DOWN,
        ACT_SMALL_BALL_BACK_UP,

        ACT_SMALL_CYLINDER_ARM_STOP = 0,
        ACT_SMALL_CYLINDER_ARM_IDLE,
        ACT_SMALL_CYLINDER_ARM_IN,
        ACT_SMALL_CYLINDER_ARM_PREPARE_TO_TAKE,
        ACT_SMALL_CYLINDER_ARM_TAKE,
        ACT_SMALL_CYLINDER_ARM_DISPOSE,
        ACT_SMALL_CYLINDER_ARM_OUT,
		ACT_SMALL_CYLINDER_ARM_PROTECT_FALL,

        ACT_SMALL_CYLINDER_BALANCER_STOP = 0,
        ACT_SMALL_CYLINDER_BALANCER_IDLE,
        ACT_SMALL_CYLINDER_BALANCER_IN,
        ACT_SMALL_CYLINDER_BALANCER_OUT,
		ACT_SMALL_CYLINDER_BALANCER_PROTECT_FALL,

        ACT_SMALL_CYLINDER_COLOR_STOP = 0,
        ACT_SMALL_CYLINDER_COLOR_IDLE,
        ACT_SMALL_CYLINDER_COLOR_NORMAL_SPEED,
        ACT_SMALL_CYLINDER_COLOR_ZERO_SPEED,

        ACT_SMALL_CYLINDER_DISPOSE_STOP = 0,
        ACT_SMALL_CYLINDER_DISPOSE_IDLE,
        ACT_SMALL_CYLINDER_DISPOSE_TAKE,
        ACT_SMALL_CYLINDER_DISPOSE_RAISE,
        ACT_SMALL_CYLINDER_DISPOSE_DISPOSE,

        ACT_SMALL_CYLINDER_ELEVATOR_STOP = 0,
        ACT_SMALL_CYLINDER_ELEVATOR_IDLE,
        ACT_SMALL_CYLINDER_ELEVATOR_TOP,
        ACT_SMALL_CYLINDER_ELEVATOR_WAIT_FOR_SLOPE,
        ACT_SMALL_CYLINDER_ELEVATOR_BOTTOM,
		ACT_SMALL_CYLINDER_ELEVATOR_LOCK_WITH_CYLINDER,
		ACT_SMALL_CYLINDER_ELEVATOR_WARNER,

        ACT_SMALL_CYLINDER_SLIDER_STOP = 0,
        ACT_SMALL_CYLINDER_SLIDER_IDLE,
        ACT_SMALL_CYLINDER_SLIDER_IN,
        ACT_SMALL_CYLINDER_SLIDER_OUT,
        ACT_SMALL_CYLINDER_SLIDER_ALMOST_OUT,
        ACT_SMALL_CYLINDER_SLIDER_ALMOST_OUT_WITH_CYLINDER,

        ACT_SMALL_CYLINDER_SLOPE_STOP = 0,
        ACT_SMALL_CYLINDER_SLOPE_IDLE,
        ACT_SMALL_CYLINDER_SLOPE_DOWN,
		ACT_SMALL_CYLINDER_SLOPE_MID,
		ACT_SMALL_CYLINDER_SLOPE_ALMOST_UP,//Utilisé pour la prise, quand l'elevator est en wait_for_slope
        ACT_SMALL_CYLINDER_SLOPE_UP,
        ACT_SMALL_CYLINDER_SLOPE_VERY_UP,
		ACT_SMALL_CYLINDER_SLOPE_VERTICAL,

        ACT_SMALL_CYLINDER_MULTIFONCTION_STOP = 0,
        ACT_SMALL_CYLINDER_MULTIFONCTION_IDLE,
        ACT_SMALL_CYLINDER_MULTIFONCTION_LOCK,
        ACT_SMALL_CYLINDER_MULTIFONCTION_PUSH,
		ACT_SMALL_CYLINDER_MULTIFONCTION_OUT,
		ACT_SMALL_CYLINDER_MULTIFONCTION_IN,

        ACT_SMALL_MAGIC_ARM_STOP = 0,
        ACT_SMALL_MAGIC_ARM_IDLE,
        ACT_SMALL_MAGIC_ARM_IN,
        ACT_SMALL_MAGIC_ARM_OUT,
		ACT_SMALL_MAGIC_ARM_CURRENT_POS,

        ACT_SMALL_MAGIC_COLOR_STOP = 0,
        ACT_SMALL_MAGIC_COLOR_IDLE,
        ACT_SMALL_MAGIC_COLOR_NORMAL_SPEED,
        ACT_SMALL_MAGIC_COLOR_ZERO_SPEED,

        ACT_SMALL_ORE_STOP = 0,
        ACT_SMALL_ORE_IDLE,
        ACT_SMALL_ORE_UP,
        ACT_SMALL_ORE_DOWN,

		ACT_SMALL_CYLINDER_POUSSIX_STOP = 0,
		ACT_SMALL_CYLINDER_POUSSIX_IDLE,
		ACT_SMALL_CYLINDER_POUSSIX_UP,
		ACT_SMALL_CYLINDER_POUSSIX_MID,
		ACT_SMALL_CYLINDER_POUSSIX_DOWN,

//Pompes

		ACT_POMPE_STOP = 0,
		ACT_POMPE_NORMAL,
		ACT_POMPE_REVERSE,
		//Parametré pour pompe slider a droite
		ACT_POMPE_SMALL_ELEVATOR_NORMAL  = ACT_POMPE_NORMAL,
		ACT_POMPE_SMALL_ELEVATOR_REVERSE = ACT_POMPE_REVERSE,
		ACT_POMPE_SMALL_DEPOSE_NORMAL    = ACT_POMPE_REVERSE,
		ACT_POMPE_SMALL_DEPOSE_REVERSE   = ACT_POMPE_NORMAL,
		ACT_POMPE_SMALL_SLIDER_NORMAL    = ACT_POMPE_NORMAL,
		ACT_POMPE_SMALL_SLIDER_REVERSE   = ACT_POMPE_REVERSE,



		ACT_TURBINE_STOP = 0,
		ACT_TURBINE_NORMAL = ACT_MOSFET_NORMAL

	} ACT_order_e;

	typedef enum {
		VACUOSTAT_ELEVATOR_RIGHT = 1,
		VACUOSTAT_ELEVATOR_LEFT,
		VACUOSTAT_DISPOSE_RIGHT,
		VACUOSTAT_DISPOSE_LEFT,
		VACUOSTAT_SLIDER_RIGHT,
		VACUOSTAT_SLIDER_LEFT
	}act_vacuostat_id;

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
		ACT_ERROR_OVERHEATING = 0,
		ACT_ERROR_TORQUE_TOO_HIGH=1
	}act_error;

	typedef enum{
		SPEED_CONFIG = 0,
		TORQUE_CONFIG,
		POSITION_CONFIG,
		TEMPERATURE_CONFIG,
		LOAD_CONFIG
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

	typedef struct{
		Uint16 warner_pos;
		Uint16 last_pos;
		bool_e activated;
	}act_warner_s;

	typedef enum{
		COLOR_SENSOR_I2C_NONE = 0,
		COLOR_SENSOR_I2C_BLUE,
		COLOR_SENSOR_I2C_WHITE,
		COLOR_SENSOR_I2C_YELLOW
	}COLOR_SENSOR_I2C_color_e;

	typedef enum{
		SCAN_I2C_LEFT = 0,
		SCAN_I2C_RIGHT
	}SCAN_I2C_side_e;

	typedef enum{
		SCAN_SENSOR_ID_SMALL_ELEVATOR = 0,
		SCAN_SENSOR_ID_NB
	}SCAN_SENSOR_id_e;

	/**********************************************************************************************************************
	 **********************************************************************************************************************
	 **												Types associés à la carte mosfet									 **
	 **********************************************************************************************************************
	 *********************************************************************************************************************/

	typedef enum{
		MOSFET_BOARD_CURRENT_MEASURE_STATE_NO_PUMPING,
		MOSFET_BOARD_CURRENT_MEASURE_STATE_PUMPING_NOTHING,
		MOSFET_BOARD_CURRENT_MEASURE_STATE_PUMPING_OBJECT
	}MOSFET_BOARD_CURRENT_MEASURE_state_e;


#endif /* ndef QS_TYPES_H */
