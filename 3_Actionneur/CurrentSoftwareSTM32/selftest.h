/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Carte Actionneur
 *  Description : Selftest
 *  Auteur : Alexis
 *  Robot : Pierre & Guy
 */

#ifndef SELFTEST_H
#define SELFTEST_H

#include "../QS/QS_all.h"
#include "queue.h"

typedef enum {
	// Seul selftest.c a le droit d'utiliser cette valeur
#ifdef SELFTEST_C
	SELFTEST_STATE_UNUSED = 0,
#endif
	SELFTEST_STATE_OK = 1,
	SELFTEST_STATE_ERROR = 2
} SELFTEST_state_e;

void SELFTEST_new_selftest(Uint8 nb_actionneurs);
bool_e SELFTEST_finish(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param);

#endif // SELFTEST_H
