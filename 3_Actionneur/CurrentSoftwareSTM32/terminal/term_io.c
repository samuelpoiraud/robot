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
	#include "../Black/Fishs/fish_brush_front_config.h"
	#include "../Black/Fishs/fish_brush_back_config.h"
	#include "../Black/Sand/top_plate_left_rx24_config.h"
	#include "../Black/Sand/top_plate_right_rx24_config.h"
#else
	#include "../Pearl/Sand/left_arm_config.h"
	#include "../Pearl/Sand/right_arm_config.h"
	#include "../Pearl/Sand/circle_config.h"
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
		DECLARE_RX24(2, '0', FISH_BRUSH_FRONT_RX24),
		DECLARE_RX24(2, '1', FISH_BRUSH_BACK_RX24),
		DECLARE_RX24(2, '2', TOP_PLATE_LEFT_RX24),
		DECLARE_RX24(2, '3', TOP_PLATE_RIGHT_RX24)
	#else
		DECLARE_AX12(2, 'A', LEFT_ARM_AX12),
		DECLARE_AX12(2, 'B', RIGHT_ARM_AX12),
		DECLARE_AX12(2, 'C', CIRCLE_AX12)
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
			else if(terminal_motor[i].type == term_MOTOR)
				position = terminal_motor[i].fun();
			else{
				position = PWM_get_duty(terminal_motor[i].id);
				terminal_motor[i].sens = GPIO_ReadOutputDataBit(terminal_motor[i].GPIOx_sens, terminal_motor[i].GPIO_Pin_sens);
			}
		}
	}

	if(EGAL_CHAR_PRINT(c)){
		debug_printf("---------------------- Position ----------------------\n");
		for(i=0;i<terminal_motor_size;i++){
			if(terminal_motor[i].type == term_AX12)
				debug_printf("%s : %d\n", terminal_motor[i].name, AX12_get_position(terminal_motor[i].id));
			else if(terminal_motor[i].type == term_MOTOR)
				debug_printf("%s : %d /  real : %d\n" , terminal_motor[i].name, terminal_motor[i].fun(), 0 /*conv_potar_updown_to_dist(terminal_motor[i].fun())*/);
			else
				debug_printf("%s : %d %s\n" , terminal_motor[i].name, PWM_get_duty(terminal_motor[i].id), (terminal_motor[i].sens)?"NORMAL":"REVERSE");
		}
	}

	if(EGAL_CHAR_HELP(c)){
		debug_printf("---------------------- Terminal ----------------------\n");
		debug_printf("Actionneur : \n");
		for(i=0;i<terminal_motor_size;i++){
			if(terminal_motor[i].type == term_AX12)
				debug_printf("(%s) cmd : %c   ID : %d   NAME : %s\n", AX12_is_ready(terminal_motor[i].id) ? "  Connecté" : "Déconnecté", terminal_motor[i].char_selection, terminal_motor[i].id, terminal_motor[i].name);
			else if(terminal_motor[i].type == term_MOTOR)
				debug_printf("(%s) cmd : %c   ID : %d   NAME : %s\n", "----------", terminal_motor[i].char_selection, terminal_motor[i].id, terminal_motor[i].name);
			else
				debug_printf("(%s) cmd : %c   PWM : %d   NAME : %s\n", "----------", terminal_motor[i].char_selection, terminal_motor[i].id, terminal_motor[i].name);
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
	else if(terminal_motor[state].type == term_PWM)
		PWM_run(position, terminal_motor[state].id);
#ifdef USE_DCMOTOR2
	else{
		DCM_setPosValue(terminal_motor[state].id, 0, position);
		DCM_goToPos(terminal_motor[state].id, 0);
		DCM_restart(terminal_motor[state].id);
	}
#endif
}
