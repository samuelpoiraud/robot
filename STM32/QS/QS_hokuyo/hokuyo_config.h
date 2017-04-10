/*
 * hokuyo_config.h
 *
 *  Created on: 16 févr. 2017
 *      Author: guill
 */

#ifndef QS_HOKUYO_HOKUYO_CONFIG_H_
#define QS_HOKUYO_HOKUYO_CONFIG_H_


//------------------- Réglage de la commande -------------------

/**
 * Commande MS : Distance codée avec l'algo 2 d'encodage (entre 0 et 6 ms)
 * Commande ME : Distance + intensité codées sur l'algo 3 d'encodage chacun (entre 100 et 110 ms)
 */
//#define USE_COMMAND_ME
#define	BLIND_ANGLE							10		// [°] Angle de chaque coté de l'hokuyo où l'on ne prend pas en compte les points

//--------------------------------------------------------------


//--------------------------- Timeout --------------------------

#define HOKUYO_BUFFER_READ_TIMEOUT			500  	// [ms]
#define HOKUYO_ECHO_READ_TIMEOUT			100  	// [ms]

//--------------------------------------------------------------


//-------------------- Traitement des points -------------------

#define STARTING_STEP						0		// Position du premier point mesuré par l'hokuyo (entre 0 et 1080)
#define CLUSTER_COUNT						0		// Nombre de point que l'hokuyo peut fusionner
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
#define HOKUYO_OFFSET_ANGLE_BIG				0       // [PI4096] Angle entre le 0 du gros robot le centre de l'angle mort de l'hokuyo
#define HOKUYO_OFFSET_ANGLE_SMALL			0       // [PI4096] Angle entre le 0 du petit robot le centre de l'angle mort de l'hokuyo

//--------------------------------------------------------------


//------------------------ Adversaires -------------------------

#define HOKUYO_MAX_FOES						16		// Nombre de robots adverses maximum sur le terrain
#define PERIOD_SEND_ADVERSARIES_DATAS		120		// Fréquence d'envoi de position adverse en [ms]
#define DISTANCE_POINTS_IN_THE_SAME_OBJECT 	150		// [mm] Distance entre deux points détectés par l'hokuyo pour être regroupé afin de former un robot

//--------------------------------------------------------------

#ifndef USE_CCMRAM
	#warning "La CCMRAM est désactivée pour l'hokuyo"
#endif

#endif /* QS_HOKUYO_HOKUYO_CONFIG_H_ */
