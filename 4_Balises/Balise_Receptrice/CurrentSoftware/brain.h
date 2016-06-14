/*
 *	Club Robot ESEO 2009 - 2015
 *
 *
 *	Fichier : brain.h
 *	Package : Balises Receptrice
 *	Description : Fonctions permettant la localisation du robot adverse à partir des données de réceptions sur un cycle.
 *	Auteur : Nirgal(PIC) / Arnaud(STM32)
 *	Version 201002
 */

#ifndef BRAIN_H
	#define BRAIN_H

	#include "QS/QS_all.h"
	#include "eyes.h"

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
