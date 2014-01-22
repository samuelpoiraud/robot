/*
 *	Club Robot ESEO 2013 - 2014
 *	Pierre & Guy
 *
 *	Fichier : Buzzer.h
 *	Package : Supervision
 *	Description : Gestion du buzzer
 *	Auteur : Nirgal
 *  Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 201401
 */

 
 #ifndef BUZZER_H
	#define BUZZER_H
	#include "../QS/QS_all.h"


//Voir la bande passante du buzzer PKLCS1212E4001-R1 : 	http://www.farnell.com/datasheets/1626578.pdf
//Et les fréquences des notes : 						http://fr.wikipedia.org/wiki/Note_de_musique
	typedef enum
	{
		DEFAULT_NOTE = 4186,	//DO : c'est la note qui fait le plus de bruit (le buzzer crache 90dB à 10cm, 4,2kHz, 3V)
		NOTE_DO0 = 2093,
		NOTE_RE0 = 2349,
		NOTE_MI0 = 2637,
		NOTE_FA = 2794,
		NOTE_SOL = 3136,
		NOTE_LA = 3520,
		NOTE_SI = 3951,
		NOTE_DO = 4186,
		NOTE_RE = 4699,
		NOTE_MI = 5274
	}BUZZER_note_e;

void BUZZER_process_1ms(void);


//Demander un buzz pour une durée donnée en ms
void BUZZER_play(time32_t duration_ms, BUZZER_note_e note, Uint8 nb_buzz);

void BUZZER_process_main(void);

	
#endif /* ndef BUZZER_H */
