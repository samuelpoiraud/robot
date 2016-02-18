/*
 *	Club Robot ESEO 2009 - 2010
 *	
 *
 *	Fichier : balise_localisation.h
 *	Package : Balises 2010
 *	Description : fonctions permettant la localisation du robot adverse � partir des donn�es de r�ceptions sur un cycle.
 *	Auteur : Nirgal
 *	Version 201002
 */

#ifndef BRAIN_H
	#define BRAIN_H
	
	#include "QS/QS_all.h"
	#include "eyes.h"
	
	//ATTENTION, les d�finitions de LED_RUN et LED_CAN de la QS ne sont pas bonnes pour notre carte...
	//Tout les fichiers qui incluent ce header auront recu correction et pourront utiliser ces LEDS.
	#undef LED_CAN	 	//Suppression des d�finitions
	#undef LED_RUN		//Suppression des d�finitions

	#define LED_RUN LATEbits.LATE8		//Red�finition
	#define LED_CAN LATEbits.LATE9		//Red�finition

	typedef struct
	{
		Uint16 distance;	//[cm]
		Uint16 past_distance;
		Sint16 angle;
		Sint16 past_angle;
		error_t error;
	}adversary_location_t;	
	
	void BRAIN_task_add(volatile EYES_adversary_detections_t * adversary_detection);	
	void BRAIN_process_main(void);
	void BRAIN_init(void);
	volatile adversary_location_t * BRAIN_get_adversary_location(void);
	
#endif /* ndef BRAIN_H */
