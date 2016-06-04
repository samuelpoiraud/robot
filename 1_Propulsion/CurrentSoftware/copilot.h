 /*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : motors.h
 *  Package : Propulsion
 *  Description : prototypes Fonctions qui gèrent les trajectoires
 *  Auteur : inspiré par Val' 2007 (de loin...) - Nirgal 2009
 *  Version 201203
 */
#ifndef _COPILOT_H
	#define _COPILOT_H

		#include "QS/QS_all.h"
	#include "roadmap.h"


	typedef enum{
		NOT_BRAKING = 0,
		BRAKING
	}braking_e;

	void COPILOT_init(void);
	void COPILOT_process_it(void);						//Consulte la roadmap pour savoir s'il faut set_order un nouvel ordre !

	void COPILOT_reset_absolute_destination(void);		//RAZ des destinations... (position actuelle considérée comme bonne)

	order_t COPILOT_get_current_order(void);

	volatile order_t* COPILOT_get_buffer_order(void);
	void COPILOT_buffering_order(void);

	braking_e COPILOT_braking_rotation_get(void);
	braking_e COPILOT_braking_translation_get(void);

	way_e COPILOT_get_way(void);

	bool_e COPILOT_is_arrived(void);
	bool_e COPILOT_is_arrived_rotation(void);
	bool_e COPILOT_is_arrived_translation(void);

	border_mode_e COPILOT_get_border_mode(void);

	trajectory_e COPILOT_get_trajectory(void);

	void COPILOT_set_in_rush(bool_e in_rush_msg, Sint16 first_traj_acc_msg, Sint16 second_traj_acc_msg, Sint16 brake_acc_msg, Uint8 acc_rot_trans_msg);

#endif	//def _COPILOT_H
