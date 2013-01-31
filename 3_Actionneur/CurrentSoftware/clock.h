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

typedef Uint16 time_t;

void CLOCK_init();

time_t CLOCK_get_time();

#endif /* ndef ndef CLOCK_H */
