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

	typedef enum
	{
		DEFAULT_NOTE = 440,	//LA
		NOTE_DO = 262,
		NOTE_RE = 294,
		NOTE_MI = 330,
		NOTE_FA = 349,
		NOTE_SOL = 392,
		NOTE_LA = 440,
		NOTE_SI = 494,
		NOTE_DO2 = 523
	}BUZZER_note_e;

void BUZZER_process_1ms(void);


//Demander un buzz pour une durée donnée en ms
void BUZZER_play(time32_t duration_ms, BUZZER_note_e note, Uint8 nb_buzz);

void BUZZER_process_main(void);

	
#endif /* ndef BUZZER_H */
