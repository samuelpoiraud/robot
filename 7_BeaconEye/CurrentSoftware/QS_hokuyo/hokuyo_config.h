/*
 * hokuyo_config.h
 *
 *  Created on: 16 févr. 2017
 *      Author: guill
 */

#ifndef QS_HOKUYO_HOKUYO_CONFIG_H_
#define QS_HOKUYO_HOKUYO_CONFIG_H_

/**
 * Commande MS : Distance codée avec l'algo 2 d'encodage
 * Commande ME : Distance + intensité codées sur l'algo 3 d'encodage chacun
 */
//#define USE_COMMAND_ME

//#define USE_CCMRAM

#ifndef USE_CCMRAM
	#warning "Réactiver la CCMRAM pour l'hokuyo"
#endif

#ifdef I_AM_CARTE_BEACON_EYE
	#include "../IHM/terminal.h"
	#include "../environment.h"
	#define terminal_debug(...)	 TERMINAL_printf(__VA_ARGS__)
#else
	#define terminal_debug(...)	 (void)0
#endif

/* Hokuyo frame */
#define LINE_FEED							0x0A	// Caractère de retour à la ligne
#define DATA_BLOCK_SIZE						64
#ifdef USE_COMMAND_ME
	#define NB_BYTES_FROM_HOKUYO			6750
	#define HOKUYO_COMMAND					((Uint8*)"ME0000108001001")
#else
	#define NB_BYTES_FROM_HOKUYO			2500
	#define HOKUYO_COMMAND					((Uint8*)"MS0000108001001")
#endif

/* Timeout */
#define HOKUYO_BUFFER_READ_TIMEOUT			500  	// [ms]
#define HOKUYO_ECHO_READ_TIMEOUT			100  	// [ms]

/* Hokuyo specification */
#define FIRST_MEASUREMENT_POINT				0
#define SENSOR_FRONT_STEP					540
#define LAST_MEASUREMENT_POINT				1080
#define SLIT_DIVISION						1440	// Nombre de pas de mesures pour 360°
#define NB_STEP_MAX							(LAST_MEASUREMENT_POINT - FIRST_MEASUREMENT_POINT)
#define ANGLE_RESOLUTION					(360 * 100 / SLIT_DIVISION) // [°*100]
#define FIRST_STEP_DEG						4500	// [°*100]
#define FRONT_STEP_DEG						18000   // [°*100]
#define LAST_POINT_DEG						31525   // [°*100]

/* Treatement point */
#define STARTING_STEP						0		// Position du premier point mesuré par l'hokuyo (entre 0 et 1080)
#define CLUSTER_COUNT						0		// Nombre de point que l'hokuyo peut fusionner
#define TO_CLOSE_DISTANCE					100		// [mm] Distance minimum pour prendre en compte un point
#define TO_FAR_DISTANCE						3500	// [mm] Distance à partir de laquelle un point est considéré comme trop éloigné
#define FIELD_SIZE_Y 						3000	// [mm] Longueur du terrain
#define FIELD_SIZE_X 						2000	// [mm]	Largeur du terrain
#define HOKUYO_MARGIN_FIELD_SIDE_IGNORE 	100		// [mm] Marge sur les bords du terrain
#define MARGIN_BEACON_FIELD					100		// [mm] Marge pour les balises extérieures
#define HOKUYO_OFFSET_BIG_POS_X				-10		// [mm] Position x de l'hokuyo par rapport au centre du gros robot
#define HOKUYO_OFFSET_BIG_POS_Y				4.5		// [mm] Position y de l'hokuyo par rapport au centre du gros robot
#define HOKUYO_OFFSET_SMALL_POS_X			0		// [mm] Position x de l'hokuyo par rapport au centre du petit robot
#define HOKUYO_OFFSET_SMALL_POS_Y			0		// [mm] Position y de l'hokuyo par rapport au centre du petit robot
#define HOKUYO_OFFSET_BEACON_EYE_X			1000	// [mm] Position x du centre de l'hokuyo par rapport au terrain
#define HOKUYO_OFFSET_BEACON_EYE_Y_BOT		3062	// [mm] Position y du centre de l'hokuyo par rapport au terrain pour la BOT_COLOR
#define HOKUYO_OFFSET_BEACON_EYE_Y_TOP		-62		// [mm] Position y du centre de l'hokuyo par rapport au terrain pour la TOP_COLOR
#define TOO_CLOSE_DISTANCE_BIG				250		// [mm] Distance d'un point trop proche de nous qui doit être ignoré pour le gros robot
#define TOO_CLOSE_DISTANCE_SMALL			150		// [mm] Distance d'un point trop proche de nous qui doit être ignoré pour le petit robot
#define TOO_CLOSE_DISTANCE_BEACON_EYE		100		// [mm] Distance d'un point trop proche de nous qui doit être ignoré pour la BeaconEye
#define	BLIND_ANGLE							10		// [°] Angle de chaque coté de l'hokuyo où l'on ne prend pas en compte les points
#define HOKUYO_OFFSET_ANGLE_BIG				0       // [PI4096] Angle entre le 0 du gros robot le centre de l'angle mort de l'hokuyo
#define HOKUYO_OFFSET_ANGLE_SMALL			0       // [PI4096] Angle entre le 0 du petit robot le centre de l'angle mort de l'hokuyo

/* Adversary */
#define HOKUYO_MAX_FOES						16		// Nombre de robots adverses maximum sur le terrain
#define PERIOD_SEND_ADVERSARIES_DATAS		120		// Fréquence d'envoi de position adverse en [ms]
#define DISTANCE_POINTS_IN_THE_SAME_OBJECT 	150		// [mm] Distance entre deux points détectés par l'hokuyo pour être regroupé afin de former un robot

/* Utility */
#define DEG100_TO_PI4096(x)					((((Sint32)(x))*183)>>8) 								// [°*100] en [PI4096]
#define TWO_CHARACTER_DECODING(a, b)		((((Uint16)a)-0x30)<<6)+((((Uint16)b)-0x30)&0x3f)		// [mm] Decode octets hokuyo, algo pour 2 caractères


#endif /* QS_HOKUYO_HOKUYO_CONFIG_H_ */
