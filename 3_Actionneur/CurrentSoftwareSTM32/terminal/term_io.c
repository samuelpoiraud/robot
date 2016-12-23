#include <stdarg.h>
#include "term_io.h"
#include "../QS/QS_uart.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_rx24.h"
#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../clock.h"

#ifdef I_AM_ROBOT_BIG
	#include "../Harry/ore/ore_gun/ore_gun.h"
	#include "../Harry/ore/ore_gun/ore_gun_config.h"
	#include "../Harry/ore/ore_shovel/ore_shovel.h"
	#include "../Harry/ore/ore_shovel/ore_shovel_config.h"
	#include "../Harry/ore/ore_catch/ore_catch.h"
	#include "../Harry/ore/ore_catch/ore_catch_config.h"
	#include "../Harry/ore/ore_mixer/ore_mixer.h"
	#include "../Harry/ore/ore_mixer/ore_mixer_config.h"
	#include "../Harry/ore/ore_sweeper/ore_sweeper.h"
	#include "../Harry/ore/ore_sweeper/ore_sweeper_config.h"
    #include "../Harry/big_bearing_ball/big_ball_back_left/big_ball_back_left.h"
    #include "../Harry/big_bearing_ball/big_ball_back_left/big_ball_back_left_config.h"
    #include "../Harry/big_bearing_ball/big_ball_back_right/big_ball_back_right.h"
    #include "../Harry/big_bearing_ball/big_ball_back_right/big_ball_back_right_config.h"
    #include "../Harry/big_bearing_ball/big_ball_front_left/big_ball_front_left.h"
    #include "../Harry/big_bearing_ball/big_ball_front_left/big_ball_front_left_config.h"
    #include "../Harry/big_bearing_ball/big_ball_front_right/big_ball_front_right.h"
    #include "../Harry/big_bearing_ball/big_ball_front_right/big_ball_front_right_config.h"
	#include "../Harry/cylinder/cylinder_hatch/cylinder_hatch_left.h"
	#include "../Harry/cylinder/cylinder_hatch/cylinder_hatch_left_config.h"
	#include "../Harry/cylinder/cylinder_hatch/cylinder_hatch_right.h"
	#include "../Harry/cylinder/cylinder_hatch/cylinder_hatch_right_config.h"
	#include "../Harry/cylinder/cylinder_slope/cylinder_slope_left.h"
	#include "../Harry/cylinder/cylinder_slope/cylinder_slope_left_config.h"
	#include "../Harry/cylinder/cylinder_slope/cylinder_slope_right.h"
	#include "../Harry/cylinder/cylinder_slope/cylinder_slope_right_config.h"
	#include "../Harry/cylinder/cylinder_dispenser/cylinder_dispenser_left.h"
	#include "../Harry/cylinder/cylinder_dispenser/cylinder_dispenser_left_config.h"
	#include "../Harry/cylinder/cylinder_dispenser/cylinder_dispenser_right.h"
	#include "../Harry/cylinder/cylinder_dispenser/cylinder_dispenser_right_config.h"
    #include "../Harry/big_bearing_ball/bearing_ball_wheel/bearing_ball_wheel.h"
    #include "../Harry/big_bearing_ball/bearing_ball_wheel/bearing_ball_wheel_config.h"
    #include "../Harry/cylinder/cylinder_pusher/cylinder_pusher_left.h"
    #include "../Harry/cylinder/cylinder_pusher/cylinder_pusher_left_config.h"
    #include "../Harry/cylinder/cylinder_pusher/cylinder_pusher_right.h"
    #include "../Harry/cylinder/cylinder_pusher/cylinder_pusher_right_config.h"
    #include "../Harry/cylinder/cylinder_elevator/cylinder_elevator_left.h"
    #include "../Harry/cylinder/cylinder_elevator/cylinder_elevator_left_config.h"
    #include "../Harry/cylinder/cylinder_elevator/cylinder_elevator_right.h"
    #include "../Harry/cylinder/cylinder_elevator/cylinder_elevator_right_config.h"
    #include "../Harry/cylinder/cylinder_slider/cylinder_slider_left.h"
    #include "../Harry/cylinder/cylinder_slider/cylinder_slider_left_config.h"
    #include "../Harry/cylinder/cylinder_slider/cylinder_slider_right.h"
    #include "../Harry/cylinder/cylinder_slider/cylinder_slider_right_config.h"
    #include "../Harry/cylinder/cylinder_turn_arm/cylinder_turn_left_arm.h"
    #include "../Harry/cylinder/cylinder_turn_arm/cylinder_turn_left_arm_config.h"
    #include "../Harry/cylinder/cylinder_turn_arm/cylinder_turn_right_arm.h"
    #include "../Harry/cylinder/cylinder_turn_arm/cylinder_turn_right_arm_config.h"


#else

#endif

typedef Sint16(*sensor_position_fun_t)(void);

typedef enum{
	term_AX12,
	term_RX24,
	term_MOTOR,
	term_PWM
}terminal_type;

typedef struct{
	terminal_type type;
	Uint8 inc_quantum;
	char char_selection;
	char name[30];
	Uint8 id;
	Sint16 min_value;
	Sint16 max_value;
	Uint8 sens;
	sensor_position_fun_t fun;
	GPIO_TypeDef* GPIOx_sens;
	uint16_t GPIO_Pin_sens;
}terminal_motor_s;

/*
 * inc      -> nombre de degrés par incrémentation dans le terminal
 * char		-> le caractère que l'on doit envoyer pour sélectionner l'actionneur
 * prefix   -> le nom de l'actionneur
 * fun      -> fonction qui retourne la position du bras
 */
#define DECLARE_RX24(inc, char, prefix) {term_RX24, inc, char, #prefix, prefix##_ID, prefix##_MIN_VALUE, prefix##_MAX_VALUE, 0, NULL, NULL, 0}
#define DECLARE_AX12(inc, char, prefix) {term_AX12, inc, char, #prefix, prefix##_ID, prefix##_MIN_VALUE, prefix##_MAX_VALUE, 0, NULL, NULL, 0}
#define DECLARE_MOTOR(inc, char, prefix, fun) {term_MOTOR, inc, char, #prefix, prefix##_ID, prefix##_MIN_VALUE, prefix##_MAX_VALUE, 0, fun, NULL, 0}
#define DECLARE_PWM(inc, char, prefix) {term_PWM, inc, char, #prefix, prefix##_PWM_NUM, 0, 100, 0, NULL, prefix##_SENS}

terminal_motor_s terminal_motor[] = {
	#ifdef I_AM_ROBOT_BIG
		DECLARE_RX24(2, 'O', ORE_GUN_RX24),
		DECLARE_RX24(2, 'O', ORE_SHOVEL_RX24),
		DECLARE_AX12(2, 'O', ORE_CATCH_AX12),
		DECLARE_AX12(2, 'O', ORE_MIXER_AX12),
		DECLARE_AX12(2, 'O', ORE_SWEEPER_AX12),
        DECLARE_AX12(2, 'O', BIG_BALL_FRONT_LEFT_AX12),
        DECLARE_AX12(2, 'O', BIG_BALL_FRONT_RIGHT_AX12),
        DECLARE_AX12(2, 'O', BIG_BALL_BACK_LEFT_AX12),
        DECLARE_AX12(2, 'O', BIG_BALL_BACK_RIGHT_AX12),
		DECLARE_AX12(2, 'O', CYLINDER_HATCH_LEFT_AX12),
		DECLARE_AX12(2, 'O', CYLINDER_HATCH_RIGHT_AX12),
		DECLARE_AX12(2, 'O', CYLINDER_SLOPE_LEFT_AX12),
		DECLARE_AX12(2, 'O', CYLINDER_SLOPE_RIGHT_AX12),
		DECLARE_AX12(2, 'O', CYLINDER_DISPENSER_LEFT_AX12),
		DECLARE_AX12(2, 'O', CYLINDER_DISPENSER_RIGHT_AX12),
        DECLARE_AX12(2, 'O', BEARING_BALL_WHEEL_AX12),
        DECLARE_AX12(2, 'O', CYLINDER_PUSHER_LEFT_AX12),
        DECLARE_AX12(2, 'O', CYLINDER_PUSHER_RIGHT_AX12),
        DECLARE_RX24(2, 'O', CYLINDER_ELEVATOR_LEFT_RX24),
        DECLARE_RX24(2, 'O', CYLINDER_ELEVATOR_RIGHT_RX24),
        DECLARE_RX24(2, 'O', CYLINDER_SLIDER_LEFT_RX24),
        DECLARE_RX24(2, 'O', CYLINDER_SLIDER_RIGHT_RX24),
        DECLARE_RX24(2, 'O', CYLINDER_TURN_LEFT_ARM_RX24),
        DECLARE_RX24(2, 'O', CYLINDER_TURN_RIGHT_ARM_RX24),


	#else

	#endif
};

Uint8 terminal_motor_size = sizeof(terminal_motor)/sizeof(terminal_motor_s);

#define EGAL_CHAR_INC(c)			(c == 'p' || c == 'P' || c == '+')
#define EGAL_CHAR_DEC(c)			(c == 'm' || c == 'M'|| c == '-')
#define EGAL_CHAR_ACTIVE(c)			(c == 'r')
#define EGAL_CHAR_PRINT(c)			(c == ' ')
#define EGAL_CHAR_HELP(c)			(c == 'h' || c == 'H')

void TERMINAL_init(){
	PORTS_pwm_init();
	PWM_init();
	AX12_init();
	RX24_init();
}

void TERMINAL_uart_checker(unsigned char c){

	static Uint8 state = -1;
	static Uint16 position = 512;
	Uint8 i;

	for(i=0;i<terminal_motor_size;i++){
		if(terminal_motor[i].char_selection == c && state != i){
			debug_printf("%s selected\n", terminal_motor[i].name);
			state = i;
			if(terminal_motor[i].type == term_AX12)
				position = AX12_get_position(terminal_motor[i].id);
			else if(terminal_motor[i].type == term_RX24)
				position = RX24_get_position(terminal_motor[i].id);
			else if(terminal_motor[i].type == term_MOTOR)
				position = terminal_motor[i].fun();
			else if(terminal_motor[i].type == term_PWM){
				position = PWM_get_duty(terminal_motor[i].id);
				terminal_motor[i].sens = GPIO_ReadOutputDataBit(terminal_motor[i].GPIOx_sens, terminal_motor[i].GPIO_Pin_sens);
			}else
				debug_printf("Erreur ce type de terminal n'existe pas\n");
		}
	}

	if(EGAL_CHAR_PRINT(c)){
		debug_printf("---------------------- Position ----------------------\n");
		for(i=0;i<terminal_motor_size;i++){
			if(terminal_motor[i].type == term_AX12)
				debug_printf("%s : position : %3d    couple : %3d   temperature : %3d\n", terminal_motor[i].name, AX12_get_position(terminal_motor[i].id), AX12_get_load_percentage(terminal_motor[i].id), AX12_get_temperature(terminal_motor[i].id));
			else if(terminal_motor[i].type == term_RX24)
				debug_printf("%s : position : %3d    couple : %3d   temperature : %3d\n", terminal_motor[i].name, RX24_get_position(terminal_motor[i].id), RX24_get_load_percentage(terminal_motor[i].id), RX24_get_temperature(terminal_motor[i].id));
			else if(terminal_motor[i].type == term_MOTOR)
				debug_printf("%s : %d /  real : %d\n" , terminal_motor[i].name, terminal_motor[i].fun(), 0 /*conv_potar_updown_to_dist(terminal_motor[i].fun())*/);
			else if(terminal_motor[i].type == term_PWM)
				debug_printf("%s : %d %s\n" , terminal_motor[i].name, PWM_get_duty(terminal_motor[i].id), (terminal_motor[i].sens)?"NORMAL":"REVERSE");
			else
				debug_printf("Erreur ce type de terminal n'existe pas\n");
		}
	}

	if(EGAL_CHAR_HELP(c)){
		debug_printf("---------------------- Terminal ----------------------\n");
		debug_printf("Actionneur : \n");
		for(i=0;i<terminal_motor_size;i++){
			if(terminal_motor[i].type == term_AX12)
				debug_printf("(%s) cmd : %c   ID : %d   NAME : %s\n", AX12_is_ready(terminal_motor[i].id) ? "  Connecté" : "Déconnecté", terminal_motor[i].char_selection, terminal_motor[i].id, terminal_motor[i].name);
			else if(terminal_motor[i].type == term_RX24)
				debug_printf("(%s) cmd : %c   ID : %d   NAME : %s\n", RX24_is_ready(terminal_motor[i].id) ? "  Connecté" : "Déconnecté", terminal_motor[i].char_selection, terminal_motor[i].id, terminal_motor[i].name);
			else if(terminal_motor[i].type == term_MOTOR)
				debug_printf("(%s) cmd : %c   ID : %d   NAME : %s\n", "----------", terminal_motor[i].char_selection, terminal_motor[i].id, terminal_motor[i].name);
			else if(terminal_motor[i].type == term_PWM)
				debug_printf("(%s) cmd : %c   PWM : %d   NAME : %s\n", "----------", terminal_motor[i].char_selection, terminal_motor[i].id, terminal_motor[i].name);
			else
				debug_printf("Erreur ce type de terminal n'existe pas\n");
		}
		debug_printf("\nCommande : \n");
		debug_printf("p/+ incrémenter\nm/- décrémenter\nr inverser sens PWM\nESPACE affichage position\nh affichage aide\n");
	}

	if(state == -1)
		return;

	if(EGAL_CHAR_ACTIVE(c)){
		toggle_led_2(terminal_motor[state].GPIOx_sens, terminal_motor[state].GPIO_Pin_sens);
		terminal_motor[state].sens = !terminal_motor[state].sens;
	}

	if(!EGAL_CHAR_INC(c) && !EGAL_CHAR_DEC(c))
		return;

	if(EGAL_CHAR_INC(c)){
		position = ((Sint16)position+terminal_motor[state].inc_quantum >= terminal_motor[state].max_value) ? position : position+terminal_motor[state].inc_quantum;
	}else if(EGAL_CHAR_DEC(c)){
		position = ((Sint16)position-terminal_motor[state].inc_quantum <= terminal_motor[state].min_value) ? position : position-terminal_motor[state].inc_quantum;
	}

	if(terminal_motor[state].type == term_AX12)
		AX12_set_position(terminal_motor[state].id, position);
	else if(terminal_motor[state].type == term_RX24)
			RX24_set_position(terminal_motor[state].id, position);
	else if(terminal_motor[state].type == term_PWM)
		PWM_run(position, terminal_motor[state].id);
#ifdef USE_DCMOTOR2
	else if(terminal_motor[state].type == term_MOTOR){
		DCM_setPosValue(terminal_motor[state].id, 0, position);
		DCM_goToPos(terminal_motor[state].id, 0);
		DCM_restart(terminal_motor[state].id);
	}
#endif
	else
		debug_printf("Erreur ce type de terminal n'existe pas\n");
}
