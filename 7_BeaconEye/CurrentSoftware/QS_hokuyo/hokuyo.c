/*
 * hokuyo.c
 *
 *  Created on: 16 févr. 2017
 *      Author: guill
 */

#ifndef QS_HOKUYO_HOKUYO_C_
#define QS_HOKUYO_HOKUYO_C_

#include "hokuyo.h"
#include "hokuyo_config.h"
#include "../QS/QS_ports.h"
#include "../QS/QS_uart.h"
#include "../QS/QS_buttons.h"
#include "../QS/QS_who_am_i.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_maths.h"

#ifdef STM32F40XX
	#include "../QS/QS_sys.h"
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


//--------------- Privates variables ----------------

/* Stockage des données brutes issues du capteur */
#ifdef USE_CCMRAM
	static Uint8 HOKUYO_datas[NB_BYTES_FROM_HOKUYO] __attribute__((section(".ccm")));
#else
	static Uint8 HOKUYO_datas[NB_BYTES_FROM_HOKUYO];
#endif
static Uint32 datas_index = 0;

/* Points considés comme valides après traitement */
static HOKUYO_adversary_position detected_valid_points[NB_STEP_MAX];
static Uint16 nb_valid_points = 0;

/* Adversaires détectés après traitement */
static HOKUYO_adversary_position hokuyo_adversaries[HOKUYO_MAX_FOES];
static Uint8 adversaries_number = 0;

/* Sécurité contre la double initialisation */
static bool_e hokuyo_initialized = FALSE;

/* Flag indiquant que l'hokuyo est doconnecté */
volatile bool_e flag_device_disconnected = FALSE;

/* Temps en [ms] depuis le dernier envoi des adversaires */
volatile Uint16 time_since_last_sent_adversaries_datas = 0;

//---------------------------------------------------


//--------------- Privates functions ----------------

static void HOKUYO_putsCommand(Uint8 cmd[]);
static bool_e HOKUYO_readBuffer(void);
static void HOKUYO_parseDataFrame(void);
static void HOKUYO_detectRobots(void);

#ifndef I_AM_CARTE_BEACON_EYE
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

		debug_printf("Fin Init usb for Hokuyo (%d ms)\n", (global.absolute_time - startTime));
	}

	hokuyo_initialized = TRUE;
}

void HOKUYO_processMain(void) {

	typedef enum {
		INIT = 0,
		HOKUYO_WAIT,
		ASK_NEW_MEASUREMENT,
		WAIT_ECHO_COMMAND,
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
				state = ASK_NEW_MEASUREMENT;
			}
		break;

		case ASK_NEW_MEASUREMENT:
			HOKUYO_readBuffer();

			if(VCP_isRxEmpty()) {
				HOKUYO_putsCommand(HOKUYO_COMMAND);
				datas_index = 0;
				state = WAIT_ECHO_COMMAND;
			} else {
				debug_printf("Nouvelle mesure avec un buffer non vide\n");
			}

#ifndef I_AM_CARTE_BEACON_EYE
			/* On enregistre la position du robot avant de lancer le scan */
			robot_position_during_measurement = global.position;
#endif

		break;

		case WAIT_ECHO_COMMAND:
			if(entrance) {
				buffer_read_time_begin = global.absolute_time;
			}

			//TODO Vérifier la taille des données et récupérer code d'erreur

			if(HOKUYO_readBuffer()) {
				if(HOKUYO_datas[datas_index - 2] == LINE_FEED && HOKUYO_datas[datas_index - 1] == LINE_FEED && datas_index >= 21){
					state = BUFFER_READ;
					debug_printf("Echo received\n");
				} else if(datas_index > 30) {
					state = ASK_NEW_MEASUREMENT;
				}
			}

			if(global.absolute_time - buffer_read_time_begin > HOKUYO_ECHO_READ_TIMEOUT) {
				debug_printf("TimeOut Hokuyo echo\n");
				flag_device_disconnected = TRUE;
			}
		break;

		case BUFFER_READ:
			if(entrance) {
				buffer_read_time_begin = global.absolute_time;
			}

			//TODO Vérifier le code d'erreur et la taille des données

			if(HOKUYO_readBuffer()) {

#ifdef USE_COMMAND_ME
#warning "le code ci dessous, dans le mode ME n'est pas débogué... il est dégeu."
			if(datas_index > 1 && HOKUYO_datas[datas_index-2]==0x0A && HOKUYO_datas[datas_index-1]==0x0A && datas_index>=6730) // 0x0A -> LF (en ASCII)
				state=TREATMENT_DATA;
			else if(datas_index>6738)
				state=ASK_NEW_MEASUREMENT;
			else if(global.absolute_time - buffer_read_time_begin > HOKUYO_BUFFER_READ_TIMEOUT)
				state=ASK_NEW_MEASUREMENT;
#else

				if(HOKUYO_datas[datas_index - 2] == LINE_FEED && HOKUYO_datas[datas_index - 1] == LINE_FEED && datas_index >= 2257) {
					state = TREATMENT_DATA;
					debug_printf("Data received\n");
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
			HOKUYO_parseDataFrame();
			state=DETECTION_ADVERSARIES;
		break;

		case DETECTION_ADVERSARIES:
			HOKUYO_detectRobots();

#ifndef I_AM_CARTE_BEACON_EYE
			HOKUYO_computeDistanceTeta();
#endif

			state=SEND_ADVERSARIES_DATAS;
		break;

		case SEND_ADVERSARIES_DATAS:

#ifndef I_AM_CARTE_BEACON_EYE
			if(time_since_last_sent_adversaries_datas > PERIOD_SEND_ADVERSARIES_DATAS) {
				time_since_last_sent_adversaries_datas = 0;
				HOKUYO_sendAdversariesDatas();
			}
			HOKUYO_refreshAdversaries();
#endif

			//terminal_debug("Nb Adversaires : %d sur %d", HOKUYO_getAdversariesNumber(), HOKUYO_getNbValidPoints());
			if(HOKUYO_getAdversariesNumber() != 0) {
				terminal_debug("Nb Adversaires : %d sur %d", HOKUYO_getAdversariesNumber(), HOKUYO_getNbValidPoints());
			}

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
		debug_printf("\nAdv n%d X=[%ld]", i, hokuyo_adversaries[i].coordX);
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

HOKUYO_adversary_position* HOKUYO_getValidPoints(void) {
	return detected_valid_points;
}

Uint16 HOKUYO_getNbValidPoints(void) {
	return nb_valid_points;
}

HOKUYO_adversary_position* HOKUYO_getAdversaryPosition(Uint8 i) {
	assert(i < adversaries_number);
	return &hokuyo_adversaries[i];
}

void HOKUYO_deviceDisconnected(void) {
	flag_device_disconnected = TRUE;
}

#ifndef I_AM_CARTE_BEACON_EYE

static void HOKUYO_computeDistanceTeta(void) {
	Uint8 i;
	Sint16 teta;

	for(i = 0; i < adversaries_number; i++) {
		hokuyo_adversaries[i].dist = CALCULATOR_distance(robot_position_during_measurement.x,
														 robot_position_during_measurement.y,
														 hokuyo_adversaries[i].coordX,hokuyo_adversaries[i].coordY);
		teta = CALCULATOR_viewing_angle(robot_position_during_measurement.x,
				 	 	 	 	 	 	robot_position_during_measurement.y,
										hokuyo_adversaries[i].coordX,hokuyo_adversaries[i].coordY);
		hokuyo_adversaries[i].teta = CALCULATOR_modulo_angle(teta - robot_position_during_measurement.teta);
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
	Uint16 a, b;							// Octets pour former la distance
	Uint16 i;								// Index pour la boucle for
	Sint32 distance;						// Distance entre l'hokuyo et le point courant
	Sint32 angle = 0;						// Angle relatif au premier point mesurable par l'hokuyo en [°*100]
	Sint16 teta_relative;					// Angle relatif au robot en [rad4096]
	Sint16 teta_absolute;					// Angle absolu par rapport au terrain en [rad4096]
	Sint32 x_absolute;						// Position en x sur le terrain du point en cours
	Sint32 y_absolute;						// Position en y sur le terrain du point en cours
	Sint16 cos;								// Cosinus de l'angle en cours
	Sint16 sin;								// Sinus de l'angle en cours
	bool_e point_filtered;					// Le point courant est-il pris en compte
	Sint32 to_close_distance;				// Distance minimum pour que le point soit pris en compte
	nb_valid_points = 0;					// Remise à zéro des points valides
	Sint16 angle_offset_rad = 0;			// Offset en [PI_4096] entre le centre de l'angle mort de l'hokuyo et le premier point mesuré

	angle_offset_rad = ((Sint32)(FIRST_STEP_DEG)) +
							  ((Sint32)(STARTING_STEP * ANGLE_RESOLUTION));
	angle_offset_rad = DEG100_TO_PI4096(angle_offset_rad);

#ifndef I_AM_CARTE_BEACON_EYE
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
#else
	to_close_distance = TOO_CLOSE_DISTANCE_BEACON_EYE;
#endif

	for(i = 26; i < datas_index - 3;) {

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

		//distance = TWO_CHARACTER_DECODING(a, b);
		distance = ((a-0x30)<<6)+((b-0x30)&0x3f);

		/* On élimine des distances trop petites (ET LES CAS DE REFLEXIONS TROP GRANDE OU LE CAPTEUR RENVOIE 1 !) */
		if(distance	> to_close_distance) {

#ifndef I_AM_CARTE_BEACON_EYE
			teta_relative = GEOMETRY_modulo_angle(DEG100_TO_PI4096(angle) - angle_offset_rad);
			teta_absolute = GEOMETRY_modulo_angle(teta_relative + robot_position_during_measurement.teta);
			COS_SIN_4096_get(teta_absolute,&cos,&sin);
			offset_x = (Sint16)(offset_pos_x*cos/4096.);
			offset_y = (Sint16)(offset_pos_y*sin/4096.);
			x_absolute = (distance*(Sint32)(cos))/4096 + robot_position_during_measurement.x + offset_x;
			y_absolute = (distance*(Sint32)(sin))/4096 + robot_position_during_measurement.y + offset_y;
#else
			teta_relative = DEG100_TO_PI4096(angle) - angle_offset_rad;
			teta_absolute = GEOMETRY_modulo_angle(teta_relative);
			COS_SIN_4096_get(teta_absolute,&cos,&sin);
			if(ENVIRONMENT_getColor() == BOT_COLOR) {
				x_absolute = HOKUYO_OFFSET_BEACON_EYE_X + (distance*(Sint32)(cos))/4096;
				y_absolute = HOKUYO_OFFSET_BEACON_EYE_Y_BOT + (distance*(Sint32)(sin))/4096;
			} else {
				x_absolute = HOKUYO_OFFSET_BEACON_EYE_X -(distance*(Sint32)(cos))/4096;
				y_absolute = HOKUYO_OFFSET_BEACON_EYE_Y_TOP -(distance*(Sint32)(sin))/4096;
			}
#endif

			/* On garde les points qui sont sur le terrain */
			if(		x_absolute 	< 	FIELD_SIZE_X - HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					x_absolute	>	HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					y_absolute 	< 	FIELD_SIZE_Y - HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					y_absolute 	>	HOKUYO_MARGIN_FIELD_SIDE_IGNORE)
			{

				point_filtered = FALSE;	//On suppose que le point n'est pas filtré

				/* Les 4 coins et deux balises fixes */
				if( x_absolute > FIELD_SIZE_X - MARGIN_BEACON_FIELD	 ||
					x_absolute < MARGIN_BEACON_FIELD	 ||
					absolute(x_absolute - FIELD_SIZE_X/2) < MARGIN_BEACON_FIELD	)
				{
					if(y_absolute < MARGIN_BEACON_FIELD	 || y_absolute > FIELD_SIZE_Y - MARGIN_BEACON_FIELD	) {
						point_filtered = TRUE;
					}

				}

				//TODO A compléter pour le terrain de cette année

				/* On retire les points qui sont dans la zone que l'on veut aveugle */
				if(angle < 100 * BLIND_ANGLE || angle > 100 * (270 - BLIND_ANGLE)) {
					point_filtered = TRUE;
				}

				if(point_filtered == FALSE && nb_valid_points < NB_STEP_MAX) {
					detected_valid_points[nb_valid_points].teta = teta_relative; // Pour l'évitement
					detected_valid_points[nb_valid_points].coordX = x_absolute;
					detected_valid_points[nb_valid_points].coordY = y_absolute;
					nb_valid_points++;
				}
			}
		}

		angle += ANGLE_RESOLUTION * ((CLUSTER_COUNT == 0) ? 1 : CLUSTER_COUNT);

	}
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
	nb_pts=1;

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
