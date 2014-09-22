/*
 *  Club Robot ESEO 2006 - 2010
 *  Game Hoover, Phoboss, Archi-Tech', PACMAN
 *
 *  Fichier : QS_types.h
 *  Package : Qualit� Soft
 *  Description : D�finition des types pour tout code du robot
 *  Auteur : Axel Voitier (et un peu savon aussi)
 *	Revision 2008-2009 : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *  Version 20100415
 */

#ifndef QS_TYPES_H
	#define QS_TYPES_H

	/* Type de base pour le dsPIC */
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

	// time_t � l'origine, mais modifi� pour �tre compatible avec le simulateur EVE
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

	typedef enum
	{
		RED=0, BLUE=1, YELLOW=1
	} color_e;

	typedef struct
	{
		Uint11 sid;
		Uint8 data[8];
		Uint8 size;
	}CAN_msg_t;

	typedef enum
	{
		CARTE_PROP,
		CARTE_ACT,
		CARTE_P,
		CARTE_SUPER,
		CARTE_BALISE
	} cartes_e;

	typedef enum
	{
		BEACON_ID_MOTHER = 0,
		BEACON_ID_CORNER = 1,
		BEACON_ID_MIDLE = 2,
		BEACON_ID_ROBOT_1 = 3,
		BEACON_ID_ROBOT_2 = 4,
		BEACONS_NUMBER
	} beacon_id_e;

	/*sens de trajectoire - utilis� dans le code propulsion et partag� pour la strat�gie... */
	typedef enum {
		ANY_WAY=0,
		BACKWARD,
		FORWARD
	} way_e;

	/*�tat de la carte propulsion - utilis� dans le code propulsion et partag� pour la strat�gie... */
	typedef enum
	{
		NO_ERROR = 0,
		UNABLE_TO_GO_ERROR,
		IMMOBILITY_ERROR,
		ROUNDS_RETURNS_ERROR,
		UNKNOW_ERROR
	}SUPERVISOR_error_source_e;

	/*type de trajectoire - utilis� dans le code propulsion et partag� pour la strat�gie... */
	typedef enum
	{
		TRAJECTORY_TRANSLATION = 0,
		TRAJECTORY_ROTATION,
		TRAJECTORY_STOP,
		TRAJECTORY_AUTOMATIC_CURVE,
		TRAJECTORY_NONE,
		WAIT_FOREVER
	} trajectory_e;

	/*type d'�vitement - utilis� dans le code propulsion et partag� pour la strat�gie... */
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
		CUSTOM	//Les valeurs suivantes sont �galement valables (jusqu'� 255... et indiquent un choix de vitesse personnalis� !)
	 } PROP_speed_e;

#endif /* ndef QS_TYPES_H */
