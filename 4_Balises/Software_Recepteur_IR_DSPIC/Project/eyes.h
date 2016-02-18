/*
 *	Club Robot ESEO 2009 - 2010
 *	
 *
 *	Fichier : eyes.h
 *	Package : Balises 2010
 *	Description : gestion des TSOP r�cepteurs infrarouge
 *	Auteur : Nirgal
 *	Version 200907
 */

#ifndef EYES_H
	#define EYES_H
	
	#include "QS/QS_all.h"

	//ATTENTION, les d�finitions de LED_RUN et LED_CAN de la QS ne sont pas bonnes pour notre carte...
	//Tout les fichiers qui incluent ce header auront recu correction et pourront utiliser ces LEDS.
	#undef LED_CAN	 	//Suppression des d�finitions
	#undef LED_RUN		//Suppression des d�finitions

	#define LED_RUN LATEbits.LATE8		//Red�finition
	#define LED_CAN LATEbits.LATE9		//Red�finition

	typedef enum
	{
		ADVERSARY_1 = 0,
		ADVERSARY_2,
		ADVERSARY_NUMBER
	}adversary_e;
	
	typedef Uint16 time16_t;
	
	typedef struct
	{
		Uint16 counts[NOMBRE_TSOP];	// 1 ajout� chaque 130us
		time16_t begin;
		time16_t end;
	}detection_t;
	
	#define MAX_DETECTIONS_NUMBER (32)		// nombre max de d�tections diff�rentes pour 100ms !	
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
	
		#ifdef CODE_FAIT_EN_IRLANDE
		
						#ifdef BALISE_RECEPTEUR_C
						#include "../QS/QS_adc.h"
						
						
						#include <stdio.h>
						
						//FONCTIONS PRIVEES...
						
						//processus d'acquisition des valeurs analogiques en provenance des r�cepteurs TSOP
						//� l'issue du process, les valeurs r�cup�r�es sont ajout�e dans un buffer circulaire, sout formes de bool�ens.
						//le compteur d'index de ce buffer est incr�ment�
						void process_acquerir_adc(void);
						
						//cette fonction effectue une analyse des r�ceptions donn�es pour UN r�cepteur, 
						//et d�livre une conclusion
						//TRUE si on peut consid�rer que ce r�cepteur "voit" le signal, FALSE sinon
						//le param�tre TSOP prend les valeurs INDEX_TSOP_4 � INDEX_TSOP_15
						bool_e analyse_tsop(Uint16 tsop);
						
						//cette fonction lance l'analyse de chacun des r�cepteurs,
						//les r�sultats bool�ens sont stock�s dans un tableau global.signaux_recus[n� du r�cepteur]	
						//ces r�sultats signifient : "le r�cepteur xx 'voit' ou 'ne voit pas' actuellement le signal"
						Uint16 process_analyse_recepteurs(void);
						
						//mise � jour de l'affichage des leds en fonction des r�cepteurs qui re�oivent
						void process_affichage_leds(void);
						
						
					#endif //def BALISE_RECEPTEUR_C
					
					//interruption appel�e tr�s r�guli�rement, dans laquelle sont lanc�es tout les traitements.
					//les communications avec l'ext�rieur ne sont pas g�r�es ici.
					//tout les algos de d�tection et de d�duction, calculs... sont fait ici
					//sauf (peut etre) le calcul de la position x/y qui peut facilement se faire � chaque envoi seulement...
					//comme le reste n�cessite du moyennage, autant tout g�rer ici ! (env. 1ms)
					//remarque, il est n�cessaire que cette IT intervienne TRES PRECISEMENT � la m�me fr�quence que la modulation des TSOP
					//noius calerons la modulation des TSOP sur 1ms
					//(rappel : double modulation, 455khz pour pouvoir capter, et 1khz pour coder le signal !)
					void _ISR _T1Interrupt();
					
					
					
					//cette fonction PUBLIQUE (appel�e quand c'est n�cessaire, avant l'envoi d'une info) met � jour la position en x/y du robot 
					//en utilisant la position relative en alpha/distance
					//ainsi que la position connue de notre robot
					//@precondition : on suppose connue la position actuelle de notre robot.
					//@return true si l'info est peut etre bonne
					//@return false si l'info est certainement fausse !
					bool_e MAJ_xy(void);
					
					
					void process_affichage_leds_reception_simple(void);
	#endif 
	
#endif /* ndef EYES_H */
