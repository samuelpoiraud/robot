/*
 *	Club Robot ESEO 2012 - 2013
 *
 *	Fichier : Eyes.h
 *	Package : Balises
 *	Description : Acquisition et traitement des donn�es IR en provenance des balises autour du terrain.
 *	Auteur : Nirgal
 *	Version 201208
 */
 
#ifndef EYES_H
	#define EYES_H
	#include "QS/QS_all.h"
	#include "main.h"
		
	typedef Uint16 time16_t;
	
	typedef struct
	{
		time16_t begin;	//Contient ensuite midle... (le milieu de la d�tection)
		time16_t end;
		time16_t duration;
	}detection_t;
	
	typedef struct
	{
		Uint8		current_detection_index;	//Initialis� � 0, l'index est incr�ment� � chaque nouvelle d�tection. Sauf au del� la 3�me o� il pointe vers la d�tection que l'on �crase !
		bool_e		more_than_3_detections;	//Vrai lorsqu'on a ajout� une 4�me d�tection en �crasant la moins longue des trois premi�res. L'index variera donc diff�remment d�sormais !
		detection_t detections[10];		//A l'issue de l'acquisition, on sait s'il y a 3 d�tections retenues, ou moins (on connais alors le nombre exact avec l'index)
	}beacon_detections_t;
	
	typedef struct
	{
		adversary_e adversary;
		bool_e 		initialized;
		beacon_detections_t beacons_detections[BEACON_ID_MIDLE + 1];
	}adversary_beacons_detections_t;
	
	
	#define EYE_SIGNAL_BUFFER_SIZE	(Uint8)(100)	//Le nombre max de signal que l'on accepte est de 100 fronts (au del�, c'est qu'on le capte beaucoup "trop" pour d�finir proprement un point milieu !!!
						//Sachant que le nombre max de front (th�orique, si l'on capte 'tout le temps' le signal est : 
						  // un front toutes les 300us
						  // Pour 100ms : environ 330 fronts.
	


	typedef struct
	{
		Uint16 buffer[EYE_SIGNAL_BUFFER_SIZE];
		Uint16 index;
	}eye_buffer_t;
	
	typedef struct
	{
		adversary_e adversary;
		eye_buffer_t buffers[BEACON_ID_MIDLE + 1];
		bool_e initialized;
	}adversary_eyes_buffers_t;


	void EYES_init(void);
	void EYES_step(adversary_e new_adversary);
	void EYES_process_main(void);
//	void EYES_init_adversary_beacons_detections(adversary_beacons_detections_t * adversary_beacons_detection);
	void EYES_init_adversary_eyes_buffers(adversary_eyes_buffers_t * adversary_eyes_buffers);
	
#endif /* def EYES_H */
