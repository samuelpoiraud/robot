 /*
 *  Club Robot ESEO 2006 - 2012
 *
 *  Fichier : motors.h
 *  Package : Propulsion
 *  Description : prototypes Fonctions qui g�rent les trajectoires
 *  Auteur : inspir� par Val' 2007 (de loin...) - Nirgal 2009
 *  Version 201203
 */
#ifndef _ROADMAP_H
	#define _ROADMAP_H

		#include "QS/QS_all.h"

		void ROADMAP_init(void);

#include "corrector.h"
	typedef struct {
		trajectory_e trajectory;
		Sint16 x;	//[mm]
		Sint16 y;	//[mm]
		Sint16 teta;		//[rad/4096]
		relative_e relative;
		multipoint_e multipoint;
		border_mode_e border_mode;
		way_e way;
		PROP_speed_e speed;
		acknowledge_e acknowledge;	//doit on rendre compte a la carte strat�gie
		corrector_e corrector;
		avoidance_e avoidance;
		time32_t wait_time_begin;
		time32_t total_wait_time;
	}order_t;

	void ROADMAP_add_order(trajectory_e trajectory,
						Sint16 x,
						Sint16 y,
						Sint16 teta,
						relative_e relative,
						now_e now,
						way_e way,
						border_mode_e border_mode,
						multipoint_e multipoint,
						PROP_speed_e speed,
						acknowledge_e acknowledge,
						corrector_e corrector,
						avoidance_e avoidance);

	void ROADMAP_add_simple_order(order_t order, bool_e add_at_begin, bool_e clean_buffer, bool_e buffer_mode);

	bool_e ROADMAP_exists_prioritary_order(void);

	bool_e ROADMAP_get_next(order_t * order);

	void ROADMAP_launch_next_order();

#endif	//def _ROADMAP_H
