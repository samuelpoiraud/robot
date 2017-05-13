
#ifndef AVOIDANCE_H_
	#define AVOIDANCE_H_

	#include "QS/QS_all.h"

	typedef enum
	{
		NORTH_US = 0,
		NORTH_FOE = 1,
		SOUTH_US = 2,
		SOUTH_FOE = 3
	}foe_pos_e;

	typedef enum{
		FOE_TYPE_ALL = 0,
		FOE_TYPE_IR,
		FOE_TYPE_HOKUYO
	}foe_type_e;

	void FOE_ANALYSER_init();
	void FOE_ANALYSER_process_main();

	bool_e foe_in_square(bool_e verbose, Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2, foe_type_e foe_type);
	bool_e foe_in_square_color(bool_e verbose, Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2, foe_type_e foe_type);

	void debug_foe_reason(foe_origin_e origin, Sint16 angle, Sint16 distance);

	// Envoi un message CAN qui va forcer l'évitement du robot à la propulsion
	void AVOIDANCE_forced_foe_dected();

	void AVOIDANCE_activeSmallAvoidance(bool_e activeSmallAvoidance);

#endif
