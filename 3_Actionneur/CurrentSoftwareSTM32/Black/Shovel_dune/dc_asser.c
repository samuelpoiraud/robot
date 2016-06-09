
#include "dc_asser.h"
#include "../../QS/QS_pwm.h"
#include "../../QS/QS_rx24.h"
#include "../../QS/QS_outputlog.h"
#include "../../QS/QS_ports.h"
#include "shovel_dune_config.h"

#define DC_ASSER_INC				20
#define DC_ASSER_NEUTRAL_EPSILON	8
#define DC_ASSER_PROPORTIONNAL		0.5

#ifdef I_AM_ROBOT_BIG

static volatile Uint16 destination_position[DC_ASSER_MOTOR_NB];
static volatile bool_e dc_asser_state = FALSE;
static volatile Uint8 speed = SHOVEL_DUNE_DC_SPEED;

void DC_ASSER_init(){
	PWM_init();
}

bool_e DC_ASSER_set_position(dc_asser_motor_id_e id, Uint16 position){
	assert(id < DC_ASSER_MOTOR_NB);

	destination_position[id] = position;

	if(dc_asser_state)
		return TRUE;
	else{
		error_printf("DC_ASSER désactivé, mise en position impossible");
		return FALSE;
	}
}

void DC_ASSER_set_speed(Uint8 custom_speed){
	speed = custom_speed;
	if(speed > 100)
		speed = 100;
}

void DC_ASSER_move_up(dc_asser_motor_id_e id){
	assert(id < DC_ASSER_MOTOR_NB);

	destination_position[id] += DC_ASSER_INC;
}

void DC_ASSER_move_down(dc_asser_motor_id_e id){
	assert(id < DC_ASSER_MOTOR_NB);

	destination_position[id] -= DC_ASSER_INC;

}

void DC_ASSER_stop(){
	DC_ASSER_set_state(FALSE);
	PWM_stop(SHOVEL_DUNE_HELPER_RIGHT_ID);
	PWM_stop(SHOVEL_DUNE_HELPER_LEFT_ID);
}

void DC_ASSER_process_it(){
	Uint8 speed_calculate;
	if(dc_asser_state){

		// DC_ASSER_MOTOR_LEFT
		if(RX24_is_ready(SHOVEL_DUNE_LEFT_RX24_ID) && absolute(destination_position[DC_ASSER_MOTOR_LEFT] - RX24_get_position(SHOVEL_DUNE_LEFT_RX24_ID)) > DC_ASSER_NEUTRAL_EPSILON){

			speed_calculate = DC_ASSER_PROPORTIONNAL*absolute(destination_position[DC_ASSER_MOTOR_LEFT] - RX24_get_position(SHOVEL_DUNE_LEFT_RX24_ID));
			if(speed_calculate > speed)
				speed_calculate = speed;

			if(destination_position[DC_ASSER_MOTOR_LEFT] - RX24_get_position(SHOVEL_DUNE_LEFT_RX24_ID) > 0){
				SHOVEL_DUNE_DC_LEFT_UP_WAY_FCT();
				PWM_run(speed_calculate, SHOVEL_DUNE_HELPER_LEFT_ID);
			}else{
				SHOVEL_DUNE_DC_LEFT_DOWN_WAY_FCT();
				PWM_run(speed_calculate, SHOVEL_DUNE_HELPER_LEFT_ID);
			}
		}else{
			PWM_stop(SHOVEL_DUNE_HELPER_LEFT_ID);
		}

		// DC_ASSER_MOTOR_RIGHT
		if(RX24_is_ready(SHOVEL_DUNE_RIGHT_RX24_ID) && absolute(destination_position[DC_ASSER_MOTOR_RIGHT] - RX24_get_position(SHOVEL_DUNE_RIGHT_RX24_ID)) > DC_ASSER_NEUTRAL_EPSILON){

			speed_calculate = DC_ASSER_PROPORTIONNAL*absolute(destination_position[DC_ASSER_MOTOR_RIGHT] - RX24_get_position(SHOVEL_DUNE_RIGHT_RX24_ID));
			if(speed_calculate > speed)
				speed_calculate = speed;

			if(destination_position[DC_ASSER_MOTOR_RIGHT] - RX24_get_position(SHOVEL_DUNE_RIGHT_RX24_ID) > 0){
				SHOVEL_DUNE_DC_RIGHT_UP_WAY_FCT();
				PWM_run(speed_calculate, SHOVEL_DUNE_HELPER_RIGHT_ID);
			}else{
				SHOVEL_DUNE_DC_RIGHT_DOWN_WAY_FCT();
				PWM_run(speed_calculate, SHOVEL_DUNE_HELPER_RIGHT_ID);
			}
		}else{
			PWM_stop(SHOVEL_DUNE_HELPER_RIGHT_ID);
		}
	}
}

void DC_ASSER_set_state(bool_e state){
	dc_asser_state = state;
}

#endif
