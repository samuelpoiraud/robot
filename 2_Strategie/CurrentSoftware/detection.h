/*
 *	Club Robot ESEO 2009 - 2010
 *	PACMAN
 *
 *	Fichier : detection.h
 *	Package : Carte Principale
 *	Description : Traitement des informations pour d�tection
 *	Auteur : Jacen (Modifi� par Ronan)
 *	Version 20110417
 */

/* notes sur la d�tection :
On g�re pour chaque capteur et pour la position de l'adversaire deux informations
temporelles : updated et update_time.
updated signifie qu'une information nouvelle est arriv�e et qu'un
nouveau calcul du comportement � adopter est n�cessaire.
update_time sert � d�terminer si une donn�e est � consid�rer lors
d'un calcul, qui a pu �tre initi� par un autre �vennement (m�j d'un
autre capteur pour les informations de capteur, ou d�marrage d'une nouvelle
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
	/* attention, code li� � TELEMETER_NUMBER dans telemeter.h */
	/* les telemetres doivent rester en t�te de liste */
	

	/*type enum qui repr�sente les positions des diff�rents lingots*/
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

//d�claration des zones de d�tection des lingots dans les diff�rents totems
	#define FIRST_TOTEM_Y_MIN 1000
	#define FIRST_TOTEM_Y_MAX 1250
	#define SECOND_TOTEM_Y_MIN 1750
	#define SECOND_TOTEM_Y_MAX 2000
	#define NORTH_TOTEM_X_MIN 875
	#define NORTH_TOTEM_X_MAX 945
	#define SOUTH_TOTEM_X_MIN 1055
	#define SOUTH_TOTEM_X_MAX 1125

	/*d�claration des zones du centres des totems, 
	si un point est d�tect� dans cette zone, 
	on est sur qu'il n'y a plus de lingot � cette position*/
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
		Uint16 	distance; 						/*distance de l'obstacle mesur�e par le capteur*/
		Sint16 	foeX;							/*abscisse absolue du robot ennemie*/
    	Sint16 	foeY;							/*ordonn�e absolue du robot ennemie*/
    	time32_t	update_time;
    	bool_e	updated;
	}detection_information_t;
	
	void DETECTION_init(void);
	
	/*	mise � jour de l'information de d�tection avec le contenu
		courant de l'environnement */
	void DETECTION_update(void);
	
	/*	mise � jour de l'information de la position des adversaires � l'aide uniquement des telemetres*/
	void DETECTION_update_foe_only_by_telemeter();
	
	/*	mise � jour de l'information de la position des adversaires � l'aide des balises*/
	void DETECTION_update_foe_by_beacon();
	
	/*	mise � jour de l'information de la position des adversaires � l'aide des telemetres et des balises*/
	void DETECTION_update_foe_by_telemeter();

	/* d�tection des lingots dans totems*/
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
		/*attention, code li� � detection_sensor_e dans detection.h */
		
		/* envoie position adversaire en debug */
		static void CAN_send_foe_pos();

		#ifdef DEBUG_DETECTION
			#define detection_printf(...)	debug_printf(__VA_ARGS__)
		#else
			#define detection_printf(...)	(void)0
		#endif

	#endif /* def DETECTION_C */

#endif /* ndef DETECTION_H */
