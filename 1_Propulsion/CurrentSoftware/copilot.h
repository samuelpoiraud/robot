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


	typedef enum {
			NOT_BRAKING = 0,
			BRAKING
	} braking_e;
	
	void COPILOT_init(void);

	//Consulte la roadmap pour savoir s'il faut set_order un nouvel ordre !
	void COPILOT_process_it(void);

	void COPILOT_try_order(order_t * order, bool_e change_order_in_multipoint_without_reaching_destination);	//On essaye de lancer un nouvel ordre (abouti soit sur un do_order de cet ordre, soit sur un autre do_order temporaire...)

	//Attention aux cas où l'ordre fait est différent de l'ordre tenté (rotation préalable notamment...)
	void COPILOT_do_order(order_t * order);	//On réalise un nouvel ordre


	//RAZ des destinations... (position actuelle considérée comme bonne)
	void COPILOT_reset_absolute_destination(void);
	void COPILOT_set_absolute_destination(Sint16 x, Sint16 y, Sint16 teta);
	void COPILOT_updatrelative_e_destination_rotation(void);
	void COPILOT_updatrelative_e_destination_translation(void);
	way_e COPILOT_get_way(void);
	border_mode_e COPILOT_get_border_mode(void);
	trajectory_e COPILOT_get_trajectory(void);

	bool_e COPILOT_is_arrived(void);
	bool_e COPILOT_is_arrived_translation(void);
	bool_e COPILOT_is_arrived_rotation(void);
	braking_e COPILOT_braking_rotation_get(void);

	braking_e COPILOT_braking_translation_get(void);

	#endif	//def _COPILOT_H
