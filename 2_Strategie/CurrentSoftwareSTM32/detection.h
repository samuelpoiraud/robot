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

	#include "config_use.h"
	
	#define BEACON_IR(f)	((f == FOE_1) ? BEACON_IR_FOE_1 : BEACON_IR_FOE_2)
	#define BEACON_US(f)	((f == FOE_1) ? BEACON_US_FOE_1 : BEACON_US_FOE_2)
	
	typedef enum
	{
		#ifdef USE_TELEMETER	
		/* attention, code lié à TELEMETER_NUMBER dans telemeter.h */
		/* les telemetres doivent rester en tête de liste */
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
		Uint16 	distance; 						/*distance de l'obstacle mesurée par le capteur*/
		Sint16 	foeX;							/*abscisse absolue du robot ennemie*/
    	Sint16 	foeY;							/*ordonnée absolue du robot ennemie*/
    	time32_t	update_time;
    	bool_e	updated;
	}detection_information_t;
	
	void DETECTION_init(void);
	
	void DETECTION_clean(void);
	
	/*	mise à jour de l'information de la position des adversaires à l'aide uniquement des telemetres*/
	void DETECTION_update_foe_only_by_telemeter();
	
	void DETECTION_pos_foe_update (CAN_msg_t* msg);
	
	/*	mise à jour de l'information de la position des adversaires à l'aide des telemetres et des balises*/
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
