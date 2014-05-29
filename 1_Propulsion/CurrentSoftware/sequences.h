 /*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : sequences.h
 *  Package : Propulsion
 *  Description : Séquences préprogrammées (et de test...) / calage bordure / ...
 *  Auteur : Nirgal (2009)
 *  Version 201203
 */

#ifndef _SEQUENCES_H
	#define _SEQUENCES_H
#include "QS/QS_all.h"
#include "QS/QS_CANmsgList.h"
#include "roadmap.h"

	void SEQUENCES_border_calibration(Sint16 angle, way_e direction, acknowledge_e acknowledge, Sint32 far_point_x, Sint32 far_point_y);

	void SEQUENCES_border_calibration_update_position(void);

	void SEQUENCES_rush_in_the_wall(Sint16 angle, way_e way, ASSER_speed_e rush_speed, acknowledge_e acquittement, Sint32 consigne_point_eloigne_x, Sint32 consigne_point_eloigne_y, border_mode_e border_mode, corrector_e corrector);

	void SEQUENCES_calibrate();

	void SEQUENCES_selftest(void);

	void SEQUENCES_trajectory_for_test_coefs(void);

#endif /*def _SEQUENCES_H*/
