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
		DEFAULT_NOTE = 0
	}BUZZER_note_e;

void BUZZER_process_1ms(void);


//Demander un buzz pour une durée donnée en ms
void BUZZER_play(time32_t duration_ms, BUZZER_note_e note);

void BUZZER_process_main(void);

	
#endif /* ndef BUZZER_H */
