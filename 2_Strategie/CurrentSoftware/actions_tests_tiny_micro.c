/*
 *	Club Robot ESEO 2008 - 2012
 *	Archi'Tech, CHOMP, Shark & Fish, Krusty & Tiny
 *
 *	Fichier : actions_tests_tiny_micro.c
 *	Package : Carte Principale
 *	Description : Tests des actions réalisables par le robot
 *	Auteur : Cyril, modifié par Vincent , Antoine , LMG , Herzaeone , Hoobbes
 *	Version 2012/01/14
 */

#define ACTIONS_TEST_TINY_MICRO_C

#include "actions_tests_tiny_micro.h"


#define DEFAULT_SPEED	(SLOW)

#define USE_CURVE	0





/* ----------------------------------------------------------------------------- */
/* 						Actions élémentaires de construction                     */
/* ----------------------------------------------------------------------------- */

//Cette fonction est appelée sur TINY à la fin du match.
void T_BALLINFLATER_start(void)
{
	CAN_msg_t msg;
	msg.sid = ACT_BALLINFLATER;
	msg.data[0] = ACT_BALLINFLATER_START;
	msg.data[1] = 10;	//[sec]
	CAN_send(&msg);
}	

/* ----------------------------------------------------------------------------- */
/* 								Fonction diverses                     			 */
/* ----------------------------------------------------------------------------- */


/* ----------------------------------------------------------------------------- */
/* 							Découpe de stratégies                     			 */
/* ----------------------------------------------------------------------------- */

