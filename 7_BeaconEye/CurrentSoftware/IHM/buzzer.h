/*
 *	Club Robot ESEO 2013 - 2015
 *	Pierre & Guy, Holly & Wood
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
	#include "../QS/QS_CANmsgList.h"

void BUZZER_play(time32_t duration, Uint8 nbBuzz);

void BUZZER_processIt(Uint8 ms);

void BUZZER_processMain(void);

#endif /* ndef BUZZER_H */
