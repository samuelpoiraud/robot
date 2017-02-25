/*
 * hokuyo.c
 *
 *  Created on: 16 févr. 2017
 *      Author: guill
 */

#ifndef QS_HOKUYO_HOKUYO_C_
#define QS_HOKUYO_HOKUYO_C_

#include "../../config/config_use.h"

#if defined(USE_HOKUYO)

#include "hokuyo.h"
#include "../QS_ports.h"
#include "../QS_uart.h"
#include "../QS_buttons.h"
#include "../QS_who_am_i.h"
#include "../QS_outputlog.h"
#include "../QS_maths.h"
#include "../QS_macro.h"
#include <string.h>
#include "../../config/config_use.h"

#ifdef STM32F40XX
	#include "../QS/QS_sys.h"
#endif

#if defined(I_AM_CARTE_BEACON_EYE)
	#include "../IHM/terminal.h"
	#include "../environment.h"
	#define terminal_debug(...)	 TERMINAL_printf(__VA_ARGS__)
#elif defined(I_AM_CARTE_PROP)
	#include "../../detection.h"
	#include "../../secretary.h"
	#define terminal_debug(...)	 (void)0
#else
	#define terminal_debug(...)	 (void)0
#endif

//------------------- USB Library -------------------

#include "STM32_USB_HOST_Library/Core/usbh_core.h"
#include "usbh_usr.h"
#include "STM32_USB_OTG_Driver/usb_hcd_int.h"
#include "STM32_USB_OTG_Driver/usb_bsp.h"
#include "STM32_USB_HOST_Library/Class/CDC/usbh_cdc_core.h"
#include "STM32_USB_HOST_Library/Class/MSC/usbh_msc_core.h"
#include "STM32_USB_HOST_Library/Class/usbh_class_switch.h"
#include "usb_vcp_cdc.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE      USB_OTG_Core 	__ALIGN_END;
__ALIGN_BEGIN USBH_HOST                USB_Host 		__ALIGN_END;

//---------------------------------------------------


//---------------- Privates defines -----------------

/* Hokuyo frame */
#define LINE_FEED							0x0A	// Caractère de retour à la ligne
#define DATA_BLOCK_SIZE						64
#ifdef USE_COMMAND_ME
	#define NB_BYTES_FROM_HOKUYO			6750
#else
	#define NB_BYTES_FROM_HOKUYO			2500
#endif

/* Hokuyo specification */
#define FIRST_MEASUREMENT_POINT				0
#define SENSOR_FRONT_STEP					540
#define LAST_MEASUREMENT_POINT				1080
#define SLIT_DIVISION						1440	// Nombre de pas de mesures pour 360°
#define FIRST_STEP_DEG						4500	// [°*100]
#define FRONT_STEP_DEG						18000   // [°*100]
#define LAST_POINT_DEG						31525   // [°*100]
#define NB_STEP_MAX							(LAST_MEASUREMENT_POINT - FIRST_MEASUREMENT_POINT + 1)
#define ANGLE_RESOLUTION					(360 * 100 / SLIT_DIVISION) // [°*100]

/* Utility */
#define DEG100_TO_PI4096(x)					((((Sint32)(x))*183)>>8) 																// [°*100] en [PI4096]
#define TWO_CHARACTER_DECODING(a, b)		((((Uint16)a)-0x30)<<6)+((((Uint16)b)-0x30)&0x3F)										// Decode octets hokuyo, algo pour 2 caractères
#define THREE_CHARACTER_DECODING(a, b, c)	((((Uint16)a)-0x30)<<12)+(((((Uint16)b)&0x3F)-0x30)<<6)+((((Uint16)c)-0x30)&0x3f)		// Decode octets hokuyo, algo pour 3 caractères

/* Command specification */
#define STARTING_STEP_SIZE					4		// Taille du champs startingStep
#define END_STEP_SIZE						4		// Taille du champs endStep
#define CLUSTER_COUNT_SIZE					2		// Taille du champs clusterCount
#define CLUSTER_COUNT_MIN					1		// Valeur minimum (incluse) pour le champs clusterCount
#define CLUSTER_COUNT_MAX					99		// Valeur maximale (incluse) pour le champs clusterCount
#define SCAN_INTERVAL_SIZE					1		// Taille du champs scanInterval
#define SCAN_INTERVAL_MIN					0		// Valeur minimum (incluse) pour le champs scanInterval
#define SCAN_INTERVAL_MAX					9		// Valeur maximale (incluse) pour le champs scanInterval
#define NUMBER_OF_SCANS_SIZE				2		// Taille du champs numberOfScans
#define NUMBER_OF_SCANS_MIN					0		// Valeur minimum (incluse) pour le champs numberOfScans
#define NUMBER_OF_SCANS_MAX					99		// Valeur maximale (incluse) pour le champs numberOfScans
#define COMMAND_SYMBOL_SIZE					2		// Taille du symbol d'une commande
#define STRING_CHARACTERS_SIZE  			16		// Taille maximale de la chaine de caractères de la commande
#define COMMAND_SIZE_MAX					35		// Taille maximale d'une commande

/* Command */
#define CMD_STEP_BLIND						(BLIND_ANGLE * 100 / ANGLE_RESOLUTION)
#define CMD_STARTING_STEP					(FIRST_MEASUREMENT_POINT + CMD_STEP_BLIND)
#define CMD_END_STEP						(LAST_MEASUREMENT_POINT - CMD_STEP_BLIND)
#define BLOCK_DATA_SIZE						64
#define CHECKSUM_SIZE						1
#ifdef USE_COMMAND_ME
	#define COMMAND_SYMBOL					CMD_ME
	#define CMD_ECHO_SIZE					47

#else
	#define COMMAND_SYMBOL					CMD_MS
	#define CMD_ECHO_SIZE					26
#endif

//---------------------------------------------------


//----------------- Privates types ------------------

typedef enum{
	CMD_MS		= 0x4D53,
	CMD_ME		= 0x4D45
} commandSymbol_e;

//---------------------------------------------------


//--------------- Privates variables ----------------

/* Stockage des données brutes issues du capteur */
#ifdef USE_CCMRAM
	static Uint8 HOKUYO_datas[NB_BYTES_FROM_HOKUYO] __attribute__((section(".ccm")));
#else
	static Uint8 HOKUYO_datas[NB_BYTES_FROM_HOKUYO];
#endif
static Uint32 datas_index = 0;

/* Points considés comme valides après traitement */
static HOKUYO_point_position detected_valid_points[NB_STEP_MAX];
static Uint16 nb_valid_points = 0;

/* Adversaires détectés après traitement */
static HOKUYO_adversary_position hokuyo_adversaries[HOKUYO_MAX_FOES];
static Uint8 adversaries_number = 0;

/* Sécurité contre la double initialisation */
static bool_e hokuyo_initialized = FALSE;

/* Flag indiquant que l'hokuyo est connecté ou déconnecté */
volatile bool_e flag_device_disconnected = FALSE;
volatile bool_e flag_device_connected = FALSE;

/* Temps en [ms] depuis le dernier envoi des adversaires */
volatile Uint16 time_since_last_sent_adversaries_datas = 0;

/* Nombre de déconnexion de l'hokuyo depuis le lancement de la carte */
static Uint16 nb_hokuyo_disconnection = 0;

/* Temps en [ms] de la dernière mesure */
static time32_t duration_last_measure = 0;

/* Commande que l'on envoi périodiquement à l'hokuyo pour demander un scan */
Uint8 cmd[COMMAND_SIZE_MAX];

/* Taille de la commande reçue */
Uint16 received_frame_size = 0;

#ifdef I_AM_CARTE_PROP
	static position_t robot_position_during_measurement;
#endif

//---------------------------------------------------


//--------------- Privates functions ----------------

static void HOKUYO_putsCommand(Uint8 cmd[]);
static bool_e HOKUYO_readBuffer(void);
static void HOKUYO_parseDataFrame(void);
static void HOKUYO_detectRobots(void);
static Uint16 HOKUYO_calculateFrameSize(commandSymbol_e commandSymbol);
static bool_e HOKUYO_isValidPoint(Sint32 x, Sint32 y);
static void HOKUYO_generateCommand( Uint8 *cmd, commandSymbol_e commandSymbol, Uint16 startingStep,
									Uint16 endStep, Uint8 clusterCount, Uint8 scanInterval,
									Uint8 numberOfScans, char *stringCharacters);

#ifdef I_AM_CARTE_PROP
	static void HOKUYO_computeDistanceTeta(void);
	static void HOKUYO_refreshAdversaries(void);
	static void HOKUYO_sendAdversariesDatas(void);
#endif

//---------------------------------------------------


void HOKUYO_init(void) {

	if(!hokuyo_initialized) {

		debug_printf("Init usb for Hokuyo\n");
		time32_t startTime = global.absolute_time;

		VCP_init(19200);

		USBH_Init(&USB_OTG_Core,
			  #ifdef USE_USB_OTG_FS
				  USB_OTG_FS_CORE_ID,
			  #else
				  USB_OTG_HS_CORE_ID,
			  #endif
				  &USB_Host,
				  &USBH_CDC_cb,
				  &USR_cb);

		debug_printf("Fin Init usb for Hokuyo (%ld ms)\n", (global.absolute_time - startTime));
	}

	hokuyo_initialized = TRUE;
}

void HOKUYO_processIt(Uint8 ms) {
	time_since_last_sent_adversaries_datas += ms;
}


void HOKUYO_processMain(void) {

	typedef enum {
		INIT = 0,
		HOKUYO_WAIT,
		ASK_NEW_MEASUREMENT,
		BUFFER_READ,
		TREATMENT_DATA,
		DETECTION_ADVERSARIES,
		SEND_ADVERSARIES_DATAS,
		ERROR,
		RESET_HOKUYO,
		RESET_ACKNOWLEDGE,
		TURN_ON_LASER,
		TURN_OFF_LASER,
		DONE
	}state_e;

	static state_e state = INIT, last_state = INIT;
	bool_e entrance;
	static time32_t buffer_read_time_begin = 0;
	static time32_t ask_measure_time_begin = 0;
	static bool_e isFirst = TRUE;
	static bool_e isFirstReceiveFrame = TRUE;

	if((state == INIT && last_state == INIT) || state != last_state) {
		entrance = TRUE;
	} else {
		entrance = FALSE;
	}

	last_state = state;

	/* Tache de fond de la librairie USB */
	USBH_Process(&USB_OTG_Core, &USB_Host);

	if(flag_device_disconnected) {
		flag_device_disconnected = FALSE;
		hokuyo_initialized = FALSE;
		HOKUYO_init();
		isFirstReceiveFrame = FALSE;
		state = INIT;
	}

	switch(state) {

		case INIT:
			if(hokuyo_initialized) {
				state = HOKUYO_WAIT;
			}
		break;

		case HOKUYO_WAIT:
			if(USBH_CDC_is_ready_to_run()) {

				/* Génération de la commande à envoyer */
				HOKUYO_generateCommand(cmd, COMMAND_SYMBOL, CMD_STARTING_STEP, CMD_END_STEP, 1, 0, 1, "");
				/* Calcul de la longueur de la trame */
				received_frame_size = HOKUYO_calculateFrameSize(COMMAND_SYMBOL);

				state = ASK_NEW_MEASUREMENT;
			}
		break;

		case ASK_NEW_MEASUREMENT:
			if(entrance) {
				if(!isFirst) {
					duration_last_measure = global.absolute_time - ask_measure_time_begin;
				} else {
					isFirst = FALSE;
				}

				ask_measure_time_begin = global.absolute_time;

			}
			if(VCP_isIdle()){
				HOKUYO_putsCommand(cmd);
			}

			datas_index = 0;
			state = BUFFER_READ;

#ifdef I_AM_CARTE_PROP
			/* On enregistre la position du robot avant de lancer le scan */
			robot_position_during_measurement = global.position;
#endif

		break;

		case BUFFER_READ:
			if(entrance) {
				buffer_read_time_begin = global.absolute_time;
			}

			if(HOKUYO_readBuffer()) {

#ifdef USE_COMMAND_ME
				if(HOKUYO_datas[datas_index - 2] == LINE_FEED && HOKUYO_datas[datas_index - 1] == LINE_FEED && datas_index >= received_frame_size) {
					state = TREATMENT_DATA;
				} else if (datas_index > received_frame_size){
					state=ASK_NEW_MEASUREMENT;
				}

#else
				if(HOKUYO_datas[datas_index - 2] == LINE_FEED && HOKUYO_datas[datas_index - 1] == LINE_FEED && datas_index >= received_frame_size) {
					state = TREATMENT_DATA;
				} else {
					state=ASK_NEW_MEASUREMENT;
				}
#endif

			}

			if(global.absolute_time - buffer_read_time_begin > HOKUYO_BUFFER_READ_TIMEOUT) {
				state = ASK_NEW_MEASUREMENT;
			}

		break;

		case TREATMENT_DATA:
			if(isFirstReceiveFrame) {
				terminal_debug("hokuyo is alive");
				isFirstReceiveFrame = FALSE;
			}
			HOKUYO_parseDataFrame();
			state=DETECTION_ADVERSARIES;
		break;

		case DETECTION_ADVERSARIES:
			HOKUYO_detectRobots();

#ifdef I_AM_CARTE_PROP
			HOKUYO_computeDistanceTeta();
#endif

			state=SEND_ADVERSARIES_DATAS;
		break;

		case SEND_ADVERSARIES_DATAS:

#ifdef I_AM_CARTE_PROP
			if(time_since_last_sent_adversaries_datas > PERIOD_SEND_ADVERSARIES_DATAS) {
				time_since_last_sent_adversaries_datas = 0;
				HOKUYO_sendAdversariesDatas();
			}
			HOKUYO_refreshAdversaries();
#endif

			state=ASK_NEW_MEASUREMENT;
			break;

		case ERROR:
			debug_printf("ERROR SEQUENCE INITIALIZING");
			state=RESET_HOKUYO;
		break;

		case RESET_HOKUYO:
			HOKUYO_putsCommand((Uint8*)"RS");
			datas_index = 0;
			state=RESET_ACKNOWLEDGE;
		break;

		case RESET_ACKNOWLEDGE:
			HOKUYO_readBuffer();
			if(HOKUYO_datas[datas_index - 2] == LINE_FEED && HOKUYO_datas[datas_index - 1] == LINE_FEED) {
				state=TURN_ON_LASER;
			} else {
				state=TURN_OFF_LASER;
			}
		break;

		case TURN_ON_LASER:
			HOKUYO_putsCommand((Uint8*)"BM");
			state=DONE;
		break;

		case TURN_OFF_LASER:
			HOKUYO_putsCommand((Uint8*)"QT");
			state=DONE;
		break;

		case DONE:
		break;

		default:
		break;
	}
}

void HOKUYO_displayAdversariesPosition(void) {
	Uint8 i;
	for(i = 0; i < adversaries_number; i++){
		debug_printf("Adv n°%d X=[%ld]", i, hokuyo_adversaries[i].coordX);
		debug_printf(" and Y=[%ld]\n", hokuyo_adversaries[i].coordY);
	}
}

void HOKUYO_dispalyValidPosition(void) {
	Uint8 i;
	for(i = 0; i < nb_valid_points; i++){
		debug_printf("\nAdv n%d X=[%ld]", i, detected_valid_points[i].coordX);
		debug_printf(" and Y=[%ld]\n", detected_valid_points[i].coordY);
	}
}

Uint8 HOKUYO_getAdversariesNumber(void) {
	return adversaries_number;
}

HOKUYO_point_position* HOKUYO_getValidPoints(void) {
	return detected_valid_points;
}

Uint16 HOKUYO_getNbValidPoints(void) {
	return nb_valid_points;
}

HOKUYO_adversary_position* HOKUYO_getAdversaryPosition(Uint8 i) {
	//assert(i < adversaries_number);
	return &hokuyo_adversaries[i];
}

Uint16 HOKUYO_getNbHokuyoDisconnection(void) {
	return nb_hokuyo_disconnection;
}

time32_t HOKUYO_getLastMeasureDuration(void) {
	return duration_last_measure;
}

void HOKUYO_deviceDisconnected(void) {
	flag_device_disconnected = TRUE;
	nb_hokuyo_disconnection++;
}

void HOKUYO_deviceConnected(void) {
	flag_device_connected = TRUE;
}

#ifdef I_AM_CARTE_PROP

bool_e HOKUYO_isWorkingWell(void) {
	if(time_since_last_sent_adversaries_datas < 2*PERIOD_SEND_ADVERSARIES_DATAS) {
		return TRUE;
	} else {
		return FALSE;
	}
}

static void HOKUYO_computeDistanceTeta(void) {
	Uint8 i;
	Sint16 teta;

	for(i = 0; i < adversaries_number; i++) {
		hokuyo_adversaries[i].dist = GEOMETRY_distance( (GEOMETRY_point_t) {robot_position_during_measurement.x, robot_position_during_measurement.y},
														(GEOMETRY_point_t) {hokuyo_adversaries[i].coordX, hokuyo_adversaries[i].coordY});

		teta = GEOMETRY_viewing_angle( robot_position_during_measurement.x,
				 	 	 	 	 	   robot_position_during_measurement.y,
									   hokuyo_adversaries[i].coordX,hokuyo_adversaries[i].coordY);

		hokuyo_adversaries[i].teta = GEOMETRY_modulo_angle(teta - robot_position_during_measurement.teta);
	}
}

static void HOKUYO_refreshAdversaries(void) {
	DETECTION_new_adversary_position(NULL, hokuyo_adversaries, adversaries_number);
}

static void HOKUYO_sendAdversariesDatas(void) {
	Uint8 i;

	if(adversaries_number==0) {
		SECRETARY_send_adversary_position(TRUE,0, 0, 0, 0, 0, 0x0000);
	} else {
		for(i=0;i<adversaries_number;i++) {
			SECRETARY_send_adversary_position((i == adversaries_number - 1) ? TRUE : FALSE, i,
											  hokuyo_adversaries[i].coordX, hokuyo_adversaries[i].coordY,
											  hokuyo_adversaries[i].teta, hokuyo_adversaries[i].dist,
											  ADVERSARY_DETECTION_FIABILITY_ALL);
		}
	}
}

#endif

/**
 * @brief Envoyer une commande à l'hokuyo
 * @param cmd La commande sous forme de chaine de caractères
 * @remark Le caractère de retour à la ligne est automatiquement envoyé à la fin de la commande
 */
static void HOKUYO_putsCommand(Uint8 cmd[]) {
	Uint32 i;

	for(i = 0; cmd[i]; i++) {
		VCP_write(cmd[i]);
	}

	VCP_write(LINE_FEED);
}

/**
 * @brief Lecture du buffer de réception de l'USB
 * @retval Indique si l'on à reçu l'indication de fin de trame (2 LINE_FEED consécutifs)
 */
static bool_e HOKUYO_readBuffer(void) {
	static Uint8 previous_data = 0;
	Uint8 data;

	while(!VCP_isRxEmpty()) {
		data = VCP_read();
		HOKUYO_datas[datas_index] = data;

		if(datas_index < NB_BYTES_FROM_HOKUYO) {
			datas_index++;
		} else {
			fatal_printf("HOKUYO : overflow in hokuyo_read_buffer !\n");
		}

		if(data == LINE_FEED && previous_data == LINE_FEED) {
			/* Deux LINE_FEED consécutifs indique la fin d'une trame */
			return TRUE;
		}

		previous_data = data;
	}

	return FALSE;
}

/**
 * @brief Décodage des données des trames Hokuyo, conversion des ces dernières vers la position
 * 		  sur le terrain et par rapport au robot. Un traitement des points est effectué pour
 * 		  supprimer ceux qui sont hors du terrain ou dans des zones définies.
 */
static void HOKUYO_parseDataFrame(void) {
	Sint16 power_intensity = -1;			// Intensité lumineuse pour le point mesuré
	Uint16 i;								// Index pour la boucle for
	Sint32 distance;						// Distance entre l'hokuyo et le point courant
	Sint32 angle = 0;						// Angle relatif au premier point mesurable par l'hokuyo en [°*100]
	Sint16 teta_relative = 0;				// Angle relatif au robot en [rad4096]
	Sint16 teta_absolute;					// Angle absolu par rapport au terrain en [rad4096]
	Sint32 x_absolute = 0;					// Position en x sur le terrain du point en cours
	Sint32 y_absolute = 0;					// Position en y sur le terrain du point en cours
	Sint16 cos;								// Cosinus de l'angle en cours
	Sint16 sin;								// Sinus de l'angle en cours
	Sint32 to_close_distance = 0;			// Distance minimum pour que le point soit pris en compte
	nb_valid_points = 0;					// Remise à zéro des points valides
	Sint16 angle_offset_rad = 0;			// Offset en [PI_4096] entre le centre de l'angle mort de l'hokuyo et le premier point mesuré

#ifdef USE_COMMAND_ME
	Uint16 a, b, c;							// Octets pour former la distance
	Uint16 d, e, f;							// Octets pour former l'intensité
#else
	Uint16 a, b;							// Octets pour former la distance
#endif

	angle_offset_rad = ((Sint32)(FIRST_STEP_DEG)) -
							  ((Sint32)(CMD_STARTING_STEP * ANGLE_RESOLUTION));
	angle_offset_rad = DEG100_TO_PI4096(angle_offset_rad);

#if defined(I_AM_CARTE_PROP)
	Sint16 offset_x, offset_y;				// Offset sur les valeurs de position sur le terrain
	Sint16 offset_pos_x, offset_pos_y;		// Offset sur les valeurs de position sur le terrain par rapport au placement de l'hokuyo sur le robot

	if(QS_WHO_AM_I_get() == BIG_ROBOT){
		to_close_distance = TOO_CLOSE_DISTANCE_BIG;
		offset_pos_x = HOKUYO_OFFSET_BIG_POS_X;
		offset_pos_y = HOKUYO_OFFSET_BIG_POS_Y;
		angle_offset_rad += HOKUYO_OFFSET_ANGLE_BIG;
	}else{
		to_close_distance = TOO_CLOSE_DISTANCE_SMALL;
		offset_pos_x = HOKUYO_OFFSET_SMALL_POS_X;
		offset_pos_y = HOKUYO_OFFSET_SMALL_POS_Y;
		angle_offset_rad += HOKUYO_OFFSET_ANGLE_SMALL;
	}
#elif defined(I_AM_CARTE_BEACON_EYE)
	to_close_distance = TOO_CLOSE_DISTANCE_BEACON_EYE;
#else
	UNUSED_VAR(teta_absolute);
	UNUSED_VAR(cos);
	UNUSED_VAR(sin);
#endif

	for(i = CMD_ECHO_SIZE; i < datas_index - 3;) {

		/* Fin d'un bloc de données, on saute le checksum et le LINE_FEED */
		if(HOKUYO_datas[i+1] == '\n') {
			i+=2;
		}
		/* Encore un LINE_FEED, c'est la fin de la trame Hokuyo */
		if(HOKUYO_datas[i] == '\n') {
			break;
		}
		a = (Uint16)HOKUYO_datas[i++];
		b = (Uint16)HOKUYO_datas[i++];

#ifdef USE_COMMAND_ME
		if(HOKUYO_datas[i+1] == '\n') {
			i+=2;
		}
		c = (Uint16)HOKUYO_datas[i++];
		d = (Uint16)HOKUYO_datas[i++];
		if(HOKUYO_datas[i+1] == '\n') {
			i+=2;
		}
		e = (Uint16)HOKUYO_datas[i++];
		f = (Uint16)HOKUYO_datas[i++];

		distance = THREE_CHARACTER_DECODING(a, b, c);
		/* Décale de 3, car 18 bits(16 bits ici) et bit de signe */
		power_intensity = ((((Sint32)(d)-0x30)<<12) + ((((Sint32)(e)-0x30)&0x3f)<<6) +((((Sint32)(f)-0x30)&0x3f))) >> 3;
		power_intensity = (power_intensity > 0)?power_intensity : -1;

#else
		distance = TWO_CHARACTER_DECODING(a, b);
#endif

		/* On élimine des distances trop petites (ET LES CAS DE REFLEXIONS TROP GRANDE OU LE CAPTEUR RENVOIE 1 !) */
		if(distance	> to_close_distance) {

#if defined(I_AM_CARTE_PROP)
			teta_relative = GEOMETRY_modulo_angle(DEG100_TO_PI4096(angle) - angle_offset_rad);
			teta_absolute = GEOMETRY_modulo_angle(teta_relative + robot_position_during_measurement.teta);
			COS_SIN_4096_get(teta_absolute,&cos,&sin);
			offset_x = (Sint16)(offset_pos_x*cos/4096.);
			offset_y = (Sint16)(offset_pos_y*sin/4096.);
			x_absolute = (distance*(Sint32)(cos))/4096 + robot_position_during_measurement.x + offset_x;
			y_absolute = (distance*(Sint32)(sin))/4096 + robot_position_during_measurement.y + offset_y;
#elif defined(I_AM_CARTE_BEACON_EYE)
			teta_relative = DEG100_TO_PI4096(angle) - angle_offset_rad;
			teta_absolute = GEOMETRY_modulo_angle(teta_relative);
			COS_SIN_4096_get(teta_absolute,&cos,&sin);
			if(ENVIRONMENT_getColor() == BOT_COLOR) {
				x_absolute = HOKUYO_OFFSET_BEACON_EYE_X - (distance*(Sint32)(cos))/4096;
				y_absolute = HOKUYO_OFFSET_BEACON_EYE_Y_BOT - (distance*(Sint32)(sin))/4096;
			} else {
				x_absolute = HOKUYO_OFFSET_BEACON_EYE_X + (distance*(Sint32)(cos))/4096;
				y_absolute = HOKUYO_OFFSET_BEACON_EYE_Y_TOP + (distance*(Sint32)(sin))/4096;
			}
#endif

			if(HOKUYO_isValidPoint(x_absolute, y_absolute) && nb_valid_points < NB_STEP_MAX) {
				detected_valid_points[nb_valid_points].teta = teta_relative; 	// Pour l'évitement
				detected_valid_points[nb_valid_points].coordX = x_absolute;
				detected_valid_points[nb_valid_points].coordY = y_absolute;
				detected_valid_points[nb_valid_points].power_intensity = power_intensity;
				nb_valid_points++;
			}
		}

		angle += ANGLE_RESOLUTION * ((CLUSTER_COUNT == 0) ? 1 : CLUSTER_COUNT);

	}
}

static bool_e HOKUYO_isValidPoint(Sint32 x, Sint32 y) {

	bool_e isValid = TRUE;					// Le point est-il pris en compte

	/* On test tout d'abord que le point est sur le terrain */
	if(	x < FIELD_SIZE_X - HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
		x > HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
		y < FIELD_SIZE_Y - HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
		y > HOKUYO_MARGIN_FIELD_SIDE_IGNORE)
	{

		/* Les 4 coins et deux balises fixes */
		if(x > FIELD_SIZE_X - MARGIN_BEACON_FIELD || x < MARGIN_BEACON_FIELD || absolute(x - FIELD_SIZE_X/2) < MARGIN_BEACON_FIELD) {
			if(y < MARGIN_BEACON_FIELD || y > FIELD_SIZE_Y - MARGIN_BEACON_FIELD) {
				isValid = FALSE;
			}
		}

		//TODO A compléter pour le terrain de cette année

	} else {
		isValid = FALSE;
	}

	return isValid;
}

/**
 * @brief Analyse le tableau de points détectés et cherche des regroupements de points. Cela se
 *        traduit par la présence d'un robot.
 */
static void HOKUYO_detectRobots(void) {
	Uint16 i;
	int nb_pts;
	Uint16 distance;
	Sint32 x_comp, y_comp, sumX, sumY;

	adversaries_number = 0;

	/* Pas de point observé, on ne fait rien */
	if(nb_valid_points < 1) {
		return;
	}

	/* Le premier point débute le premier objet */
	x_comp = detected_valid_points[0].coordX;
	y_comp = detected_valid_points[0].coordY;
	sumX = detected_valid_points[0].coordX;
	sumY = detected_valid_points[0].coordY;
	nb_pts = 1;

	/* Pour tous les points valides à partir du second */
	for(i=1;i<nb_valid_points;i++) {

		distance = GEOMETRY_manhattan_distance((GEOMETRY_point_t){detected_valid_points[i].coordX, detected_valid_points[i].coordY},
											   (GEOMETRY_point_t){x_comp, y_comp});

		if(distance <= DISTANCE_POINTS_IN_THE_SAME_OBJECT) {

			/* Le point est dans l'objet, on l'intègre à l'objet */
			sumX+=detected_valid_points[i].coordX;
			sumY+=detected_valid_points[i].coordY;
			nb_pts++;

		} else	{

			/* Si la distance est plus grande (le point n'appartient pas à l'objet), on clos l'objet en court */
			if(adversaries_number < HOKUYO_MAX_FOES - 1 && nb_pts > 2) {
				hokuyo_adversaries[adversaries_number].coordX = sumX / nb_pts;
				hokuyo_adversaries[adversaries_number].coordY = sumY / nb_pts;
				adversaries_number++;
			}

			nb_pts=1;
			/* Nouveau départ pour le nouvel objet */
			x_comp=detected_valid_points[i].coordX;
			y_comp=detected_valid_points[i].coordY;
			sumX=detected_valid_points[i].coordX;
			sumY=detected_valid_points[i].coordY;
		}
	}

	/* Traitement pour clore le dernier objet */
	if(adversaries_number < HOKUYO_MAX_FOES - 1 && nb_pts > 2)
	{
		hokuyo_adversaries[adversaries_number].coordX = sumX / nb_pts;
		hokuyo_adversaries[adversaries_number].coordY = sumY / nb_pts;
		adversaries_number++;
	}
}

static void HOKUYO_generateCommand( Uint8* cmd,
									commandSymbol_e commandSymbol,
									Uint16 startingStep,
									Uint16 endStep,
									Uint8 clusterCount,
									Uint8 scanInterval,
									Uint8 numberOfScans,
									char *stringCharacters)
{
	/* Vérification de tous les paramètres */
	if(startingStep < FIRST_MEASUREMENT_POINT) {
		startingStep = FIRST_MEASUREMENT_POINT;
	}
	if(startingStep > LAST_MEASUREMENT_POINT) {
		startingStep = LAST_MEASUREMENT_POINT;
	}
	if(endStep < startingStep) {
		endStep = startingStep;
	}
	if(endStep > LAST_MEASUREMENT_POINT) {
		endStep = LAST_MEASUREMENT_POINT;
	}
	if(clusterCount < CLUSTER_COUNT_MIN) {
		clusterCount = CLUSTER_COUNT_MIN;
	}
	if(clusterCount > CLUSTER_COUNT_MAX) {
		clusterCount = CLUSTER_COUNT_MAX;
	}
	if(scanInterval < SCAN_INTERVAL_MIN) {
		scanInterval = SCAN_INTERVAL_MIN;
	}
	if(scanInterval > SCAN_INTERVAL_MAX) {
		scanInterval = SCAN_INTERVAL_MAX;
	}
	if(numberOfScans < NUMBER_OF_SCANS_MIN) {
		numberOfScans = NUMBER_OF_SCANS_MIN;
	}
	if(numberOfScans > NUMBER_OF_SCANS_MAX) {
		numberOfScans = NUMBER_OF_SCANS_MAX;
	}

	Uint8 index = 0;
	sprintf((char*)cmd, "%c%c", ((commandSymbol >> 8) & 0xFF), ((commandSymbol) & 0xFF));
	index += COMMAND_SYMBOL_SIZE;
	sprintf((char*)(cmd + index), "%04d", startingStep);
	index += STARTING_STEP_SIZE;
	sprintf((char*)(cmd + index), "%04d", endStep);
	index += END_STEP_SIZE;
	sprintf((char*)(cmd + index), "%02d", clusterCount);
	index += CLUSTER_COUNT_SIZE;
	sprintf((char*)(cmd + index), "%01d", scanInterval);
	index += SCAN_INTERVAL_SIZE;
	sprintf((char*)(cmd + index), "%02d", numberOfScans);
	index += NUMBER_OF_SCANS_SIZE;
	strcpy((char*)(cmd + index), stringCharacters);
}

static Uint16 HOKUYO_calculateFrameSize(commandSymbol_e commandSymbol) {
	Uint16 size = 0;
	Uint16 nbDataBlock = 0;
	Uint16 nbEncodedStep = 0;

	/* Nombre d'octet pour encoder les points mesurés (les bornes sont incluses d'où le +1) */
	nbEncodedStep = (CMD_END_STEP - CMD_STARTING_STEP + 1) * ((commandSymbol == CMD_MS) ? 2 : 6);
	/* Nombre de bloc de données */
	nbDataBlock = (nbEncodedStep / BLOCK_DATA_SIZE) + (((nbEncodedStep % BLOCK_DATA_SIZE) > 0) ? 1 : 0);


	/* Echo de la commande envoyée */
	size += CMD_ECHO_SIZE +		// Echo de la commande envoyée
			nbEncodedStep +		// Nombre d'octet pour encoder chaque point mesuré
			nbDataBlock + 1 +	// Nombre de LINE_FEED (un après chaque bloc de données + 1 à la fin en plus)
			nbDataBlock;		// Nombre de checksum (un après chaque bloc de données)

	return size;
}

/**
  * @brief Fonction d'interruption pour l'USB OTG FS ou HS
  */
#ifdef USE_USB_OTG_FS
void OTG_FS_IRQHandler(void)
#else
void OTG_HS_IRQHandler(void)
#endif
{
	if (USB_OTG_IsHostMode(&USB_OTG_Core)) {
		USBH_OTG_ISR_Handler(&USB_OTG_Core);
	} else {
		//USB Device.
		//USBD_OTG_ISR_Handler(&USB_OTG_Core);
	}
}

#endif /* QS_HOKUYO_HOKUYO_C_ */

#endif
