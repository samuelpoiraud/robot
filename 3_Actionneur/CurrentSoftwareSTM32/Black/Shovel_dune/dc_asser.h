
#ifndef DC_ASSER_H
	#define	DC_ASSER_H

	#include "../../QS/QS_all.h"

	typedef enum{
		DC_ASSER_MOTOR_LEFT = 0,
		DC_ASSER_MOTOR_RIGHT,
		DC_ASSER_MOTOR_NB
	}dc_asser_motor_id_e;

	void DC_ASSER_init();

	bool_e DC_ASSER_set_position(dc_asser_motor_id_e id, Uint16 position);

	void DC_ASSER_move_up(dc_asser_motor_id_e id);

	void DC_ASSER_move_down(dc_asser_motor_id_e id);

	void DC_ASSER_set_speed(Uint8 custom_speed);

	void DC_ASSER_stop();

	void DC_ASSER_process_it();

	void DC_ASSER_set_state(bool_e state);

#endif 	/* DC_ASSER_H */
