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

#define QUANTUM_CLOCK_IT 10

void CLOCK_init();

time32_t CLOCK_get_time(); // [0.1s] en centaine de ms, 2 correspond à 200ms

time32_t CLOCK_get_time_10(); // [0.01] en dizaine de ms, 2 correspond à 20ms

#endif /* ndef ndef CLOCK_H */
