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

	#include "config_use.h"
	
	#define BEACON_IR(f)	((f == FOE_1) ? BEACON_IR_FOE_1 : BEACON_IR_FOE_2)
	#define BEACON_US(f)	((f == FOE_1) ? BEACON_US_FOE_1 : BEACON_US_FOE_2)
	
	typedef enum
	{
		#ifdef USE_TELEMETER	
		/* attention, code li� � TELEMETER_NUMBER dans telemeter.h */
		/* les telemetres doivent rester en t�te de liste */
			TELE_FRONT_LEFT = 0,
			TELE_FRONT_RIGHT,
			TELE_BACK_LEFT,
			TELE_BACK_RIGHT,
		#endif
		BEACON_IR_FOE_1,
		BEACON_US_FOE_1,
		BEACON_IR_FOE_2,
		BEACON_US_FOE_2,
		SENSOR_NB
	}detection_sensor_e;


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
	
	void DETECTION_clean(void);
	
	/*	mise � jour de l'information de la position des adversaires � l'aide uniquement des telemetres*/
	void DETECTION_update_foe_only_by_telemeter();
	
	void DETECTION_pos_foe_update (CAN_msg_t* msg);
	
	/*	mise � jour de l'information de la position des adversaires � l'aide des telemetres et des balises*/
	void DETECTION_update_foe_by_telemeter();
	
	/* nettoyage des flags dans l'environnement */
	void DETECTION_clear_updates();
	
	#ifdef DETECTION_C
	
		#include "maths_home.h"
		#include "sick.h"
		
		#define BEACON_FAR_THRESHOLD	1000
		
		#define MINIMUM_TIME_FOR_BEACON_SYNCRONIZATION 260 // 260 ms
		
		#define MAXIMUM_TIME_FOR_BEACON_REFRESH 300 // 500ms
		
		#define BORDER_DELTA 50 // 50 mm
		

		/* envoie position adversaire en debug */

		#ifdef DEBUG_DETECTION
			#define detection_printf(...)	debug_printf(__VA_ARGS__)
		#else
			#define detection_printf(...)	(void)0
		#endif

	#endif /* def DETECTION_C */

#endif /* ndef DETECTION_H */
