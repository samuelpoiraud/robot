/*
 *	Club Robot ESEO 2009 - 2010
 *	PACMAN
 *
 *	Fichier : detection.h
 *	Package : Carte Principale
 *	Description : Traitement des informations pour détection
 *	Auteur : Jacen (Modifié par Ronan)
 *	Version 20110417
 */

/* notes sur la détection :
On gère pour chaque capteur et pour la position de l'adversaire deux informations
temporelles : updated et update_time.
updated signifie qu'une information nouvelle est arrivée et qu'un
nouveau calcul du comportement à adopter est nécessaire.
update_time sert à déterminer si une donnée est à considérer lors
d'un calcul, qui a pu être initié par un autre évennement (màj d'un
autre capteur pour les informations de capteur, ou démarrage d'une nouvelle
trajectoire pour l'information de position de l'adversaire */

#include "QS/QS_all.h"

#ifndef DETECTION_H
	#define DETECTION_H
	
	#define BEACON_IR(f)	((f == FOE_1) ? BEACON_IR_FOE_1 : BEACON_IR_FOE_2)
	#define BEACON_US(f)	((f == FOE_1) ? BEACON_US_FOE_1 : BEACON_US_FOE_2)
	
	typedef enum
	{
		/*TELE_FRONT_LEFT = 0,
		TELE_FRONT_RIGHT,
		TELE_BACK_LEFT,
		TELE_BACK_RIGHT,
		*/BEACON_IR_FOE_1,
		BEACON_US_FOE_1,
		BEACON_IR_FOE_2,
		BEACON_US_FOE_2,
		SENSOR_NB
	}detection_sensor_e;
	/* attention, code lié à TELEMETER_NUMBER dans telemeter.h */
	/* les telemetres doivent rester en tête de liste */
	

	/*type enum qui représente les positions des différents lingots*/
	/*les quatres dernieres valeurs sert a indiquer lorsque le lingots n'est plus dans le totem*/
	typedef enum
	{
		NO_DETECTION,
		/*lingot dans le totem*/
		FIRST_TOTEM_NORTH,
		FIRST_TOTEM_SOUTH,
		SECOND_TOTEM_NORTH,
		SECOND_TOTEM_SOUTH,
		/*plus de lingot dans le totem*/
		FIRST_TOTEM_NORTH_EMPTY,
		FIRST_TOTEM_SOUTH_EMPTY,
		SECOND_TOTEM_NORTH_EMPTY,
		SECOND_TOTEM_SOUTH_EMPTY
	}detection_totem_e;

//déclaration des zones de détection des lingots dans les différents totems
	#define FIRST_TOTEM_Y_MIN 1000
	#define FIRST_TOTEM_Y_MAX 1250
	#define SECOND_TOTEM_Y_MIN 1750
	#define SECOND_TOTEM_Y_MAX 2000
	#define NORTH_TOTEM_X_MIN 875
	#define NORTH_TOTEM_X_MAX 945
	#define SOUTH_TOTEM_X_MIN 1055
	#define SOUTH_TOTEM_X_MAX 1125

	/*déclaration des zones du centres des totems, 
	si un point est détecté dans cette zone, 
	on est sur qu'il n'y a plus de lingot à cette position*/
	#define FIRST_TOTEM_CENTER_Y_MIN 1090
	#define FIRST_TOTEM_CENTER_Y_MAX 1180
	#define SECOND_TOTEM_CENTER_Y_MIN 1840
	#define SECOND_TOTEM_CENTER_Y_MAX 1910
	/*definition de la zone de tolerence de la detection */
	#define TOTEM_NORTH_CENTER_X_MIN 955
	#define TOTEM_NORTH_CENTER_X_MAX 975
	#define TOTEM_SOUTH_CENTER_X_MIN 1025
	#define TOTEM_SOUTH_CENTER_X_MAX 1045



	/* informations d'un capteur */
	typedef struct
	{
		Sint16 	angle;							/*angle de vue de l'adversaire (relatif au robot)*/
		Uint16 	distance; 						/*distance de l'obstacle mesurée par le capteur*/
		Sint16 	foeX;							/*abscisse absolue du robot ennemie*/
    	Sint16 	foeY;							/*ordonnée absolue du robot ennemie*/
    	time32_t	update_time;
    	bool_e	updated;
	}detection_information_t;
	
	void DETECTION_init(void);
	
	/*	mise à jour de l'information de détection avec le contenu
		courant de l'environnement */
	void DETECTION_update(void);
	
	/*	mise à jour de l'information de la position des adversaires à l'aide uniquement des telemetres*/
	void DETECTION_update_foe_only_by_telemeter();
	
	/*	mise à jour de l'information de la position des adversaires à l'aide des balises*/
	void DETECTION_update_foe_by_beacon();
	
	/*	mise à jour de l'information de la position des adversaires à l'aide des telemetres et des balises*/
	void DETECTION_update_foe_by_telemeter();

	/* détection des lingots dans totems*/
	detection_totem_e DETECTION_bullion_update();

	
	/* nettoyage des flags dans l'environnement */
	void DETECTION_clear_updates();
	
	#ifdef DETECTION_C
	
		#include "maths_home.h"
		#include "sick.h"
		
		#define BEACON_FAR_THRESHOLD	1000
		
		#define MINIMUM_TIME_FOR_BEACON_SYNCRONIZATION 260 // 260 ms
		
		#define MAXIMUM_TIME_FOR_BEACON_REFRESH 300 // 500ms
		
		#define BORDER_DELTA 50 // 50 mm
		
		/*localisation des capteurs*/
		#define TELEMETER_NUMBER	4
		/*attention, code lié à detection_sensor_e dans detection.h */
		
		/* envoie position adversaire en debug */
		static void CAN_send_foe_pos();

		#ifdef DEBUG_DETECTION
			#define detection_printf(...)	debug_printf(__VA_ARGS__)
		#else
			#define detection_printf(...)	(void)0
		#endif

	#endif /* def DETECTION_C */

#endif /* ndef DETECTION_H */
