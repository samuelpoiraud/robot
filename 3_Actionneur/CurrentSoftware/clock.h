/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : clock.h
 *	Package : Actionneur
 *	Description : decompte du temps sur actionneur
 *  Auteurs : Jacen
 *  Version 20100412
 */

#include "QS/QS_all.h"

#ifndef CLOCK_H
#define CLOCK_H

#include "QS/QS_timer.h"

typedef Uint16 clock_time_t;

void CLOCK_init();

clock_time_t CLOCK_get_time(); //en centaine de ms, 2 correspond à 200ms

#endif /* ndef ndef CLOCK_H */
