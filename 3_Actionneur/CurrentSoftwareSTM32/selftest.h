/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id: Arm.h 1313 2014-01-13 12:29:15Z amurzeau $
 *
 *  Package : Carte Actionneur
 *  Description : Self test
 *  Auteur : Alexis
 *  Robot : Pierre & Guy
 */

#ifndef SELFTEST_H
#define SELFTEST_H

#include "../QS/QS_all.h"

typedef enum {
	SELFTEST_IN_PROGRESS,
	SELFTEST_OK,
	SELFTEST_ERROR

	// Seul selftest.c a le droit d'utiliser cette valeur
#ifdef SELFTEST_C
	,SELFTEST_UNUSED
#endif
} SELFTEST_state_e;

void SELFTEST_new_selftest(Uint8 nb_actionneurs);
void SELFTEST_set_state(Uint11 act_sid, SELFTEST_state_e test_status);

#endif // SELFTEST_H
