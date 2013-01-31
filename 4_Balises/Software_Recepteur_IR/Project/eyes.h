/*
 *	Club Robot ESEO 2009 - 2010
 *	
 *
 *	Fichier : eyes.h
 *	Package : Balises 2010
 *	Description : gestion des TSOP récepteurs infrarouge
 *	Auteur : Nirgal
 *	Version 200907
 */

#ifndef EYES_H
	#define EYES_H
	
	#include "QS/QS_all.h"

	//ATTENTION, les définitions de LED_RUN et LED_CAN de la QS ne sont pas bonnes pour notre carte...
	//Tout les fichiers qui incluent ce header auront recu correction et pourront utiliser ces LEDS.
	#undef LED_CAN	 	//Suppression des définitions
	#undef LED_RUN		//Suppression des définitions

	#define LED_RUN LATEbits.LATE8		//Redéfinition
	#define LED_CAN LATEbits.LATE9		//Redéfinition

	typedef enum
	{
		ADVERSARY_1 = 0,
		ADVERSARY_2,
		ADVERSARY_NUMBER
	}adversary_e;
	
	typedef Uint16 time16_t;
	
	typedef struct
	{
		Uint8 counts[NOMBRE_TSOP];	//la valeur max de 255 serait atteinte en 75ms... (avec 1 ajouté chaque 300us)
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
	void EYES_step_init(void);
	void EYES_next_step(void);
	void EYES_init_adversary_detection(volatile EYES_adversary_detections_t * adversary_detection);
		
		#ifdef CODE_FAIT_EN_IRLANDE
		
						#ifdef BALISE_RECEPTEUR_C
						#include "../QS/QS_adc.h"
						
						
						#include <stdio.h>
						
						//FONCTIONS PRIVEES...
						
						//processus d'acquisition des valeurs analogiques en provenance des récepteurs TSOP
						//à l'issue du process, les valeurs récupérées sont ajoutée dans un buffer circulaire, sout formes de booléens.
						//le compteur d'index de ce buffer est incrémenté
						void process_acquerir_adc(void);
						
						//cette fonction effectue une analyse des réceptions données pour UN récepteur, 
						//et délivre une conclusion
						//TRUE si on peut considérer que ce récepteur "voit" le signal, FALSE sinon
						//le paramètre TSOP prend les valeurs INDEX_TSOP_4 à INDEX_TSOP_15
						bool_e analyse_tsop(Uint16 tsop);
						
						//cette fonction lance l'analyse de chacun des récepteurs,
						//les résultats booléens sont stockés dans un tableau global.signaux_recus[n° du récepteur]	
						//ces résultats signifient : "le récepteur xx 'voit' ou 'ne voit pas' actuellement le signal"
						Uint16 process_analyse_recepteurs(void);
						
						//mise à jour de l'affichage des leds en fonction des récepteurs qui reçoivent
						void process_affichage_leds(void);
						
						
					#endif //def BALISE_RECEPTEUR_C
					
					//interruption appelée très régulièrement, dans laquelle sont lancées tout les traitements.
					//les communications avec l'extérieur ne sont pas gérées ici.
					//tout les algos de détection et de déduction, calculs... sont fait ici
					//sauf (peut etre) le calcul de la position x/y qui peut facilement se faire à chaque envoi seulement...
					//comme le reste nécessite du moyennage, autant tout gérer ici ! (env. 1ms)
					//remarque, il est nécessaire que cette IT intervienne TRES PRECISEMENT à la même fréquence que la modulation des TSOP
					//noius calerons la modulation des TSOP sur 1ms
					//(rappel : double modulation, 455khz pour pouvoir capter, et 1khz pour coder le signal !)
					void _ISR _T1Interrupt();
					
					
					
					//cette fonction PUBLIQUE (appelée quand c'est nécessaire, avant l'envoi d'une info) met à jour la position en x/y du robot 
					//en utilisant la position relative en alpha/distance
					//ainsi que la position connue de notre robot
					//@precondition : on suppose connue la position actuelle de notre robot.
					//@return true si l'info est peut etre bonne
					//@return false si l'info est certainement fausse !
					bool_e MAJ_xy(void);
					
					
					void process_affichage_leds_reception_simple(void);
	#endif 
	
#endif /* ndef EYES_H */
