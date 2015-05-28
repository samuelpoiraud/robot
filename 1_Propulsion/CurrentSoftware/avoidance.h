 /*
 *  Club Robot ESEO 2014
 *
 *  Fichier : pathChecker.h
 *  Package : Propulsion
 *  Description : Analyse la trajectoire que va emprunter le robot par rapport au robot adversaire, afin de savoir si le chemin est réalisable
 *  Auteur : Arnaud
 *  Version 201203
 */

#ifndef _AVOIDANCE_H
	#define _AVOIDANCE_H
	#include "QS/QS_all.h"

	void AVOIDANCE_init();

	void AVOIDANCE_process_it();

	bool_e AVOIDANCE_target_safe(way_e way, bool_e verbose);

	//bool_e AVOIDANCE_target_safe_curve(way_e way, bool_e verbose);

	bool_e AVOIDANCE_foe_in_zone(bool_e verbose, Sint16 x, Sint16 y, bool_e check_on_all_traject);

	void AVOIDANCE_said_foe_detected();

	bool_e AVOIDANCE_foe_near();

	void AVOIDANCE_process_CAN_msg(CAN_msg_t *msg);

#endif  //def _PATHCHECKER_H
