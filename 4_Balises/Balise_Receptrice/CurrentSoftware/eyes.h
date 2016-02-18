/*
 *	Club Robot ESEO 2009 - 2015
 *
 *
 *	Fichier : eyes.h
 *	Package : Balises 2015
 *	Description : gestion des TSOP récepteurs infrarouge
 *	Auteur : Nirgal(PIC) / Arnaud(STM32)
 *	Version 200907
 */

#ifndef EYES_H
	#define EYES_H

	#include "QS/QS_all.h"

	typedef Uint8 error_t;

	typedef enum
	{
		ADVERSARY_1 = 0,
		ADVERSARY_2,
		ADVERSARY_NUMBER
	}adversary_e;

	typedef Uint16 time16_t;

	typedef struct
	{
		Uint16 counts[NOMBRE_TSOP];	// 1 ajouté chaque 130us
		time16_t begin;
		time16_t end;
	}detection_t;

	#define MAX_DETECTIONS_NUMBER (32)		// nombre max de détections différentes pour 100ms !
	typedef struct
	{
		detection_t detections[MAX_DETECTIONS_NUMBER];
		Uint8 index;
		error_t error;
		adversary_e adversary;
		bool_e initialized;
	}EYES_adversary_detections_t;	//Pour un seul adversaire.


	void EYES_init(void);
	void EYES_set_adversary(adversary_e next_adversary);
	void EYES_init_adversary_detection(volatile EYES_adversary_detections_t * adversary_detection);
	void EYES_process_main(void);
	void EYES_process_it();

#endif /* ndef EYES_H */
