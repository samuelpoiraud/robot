
#ifndef FOE_ANALYSER_H_
	#define FOE_ANALYSER_H_

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
	bool_e i_am_in_square_color(Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2);

	bool_e is_in_square_color(Sint16 x1, Sint16 x2, Sint16 y1, Sint16 y2, GEOMETRY_point_t current);

#endif
