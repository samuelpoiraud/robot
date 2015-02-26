#include <stdarg.h>
#include "term_io.h"
#include "../QS/QS_uart.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_pwm.h"
#include "../QS/QS_ports.h"
#include "../clock.h"

#ifdef I_AM_ROBOT_BIG
	#include "../Holly/Popcorn/Pop_collect_left_config.h"
	#include "../Holly/Popcorn/Pop_collect_right_config.h"
	#include "../Holly/Popcorn/Pop_drop_left_config.h"
	#include "../Holly/Popcorn/Pop_drop_right_config.h"
	#include "../Holly/Spot/back_spot_left.h"
	#include "../Holly/Spot/back_spot_left_config.h"
	#include "../Holly/Spot/back_spot_right.h"
	#include "../Holly/Spot/back_spot_right_config.h"
	#include "../Holly/Spot/spot_pompe_right.h"
	#include "../Holly/Spot/spot_pompe_left.h"
	#include "../Holly/Carpet/carpet_launcher_left.h"
	#include "../Holly/Carpet/carpet_launcher_left_config.h"
	#include "../Holly/Carpet/carpet_launcher_right.h"
	#include "../Holly/Carpet/carpet_launcher_right_config.h"
#else
	#include "../Wood/Pince_gauche_config.h"
	#include "../Wood/Pince_droite_config.h"
	#include "../Wood/Clap_config.h"
	#include "../Wood/Pop_drop_left_Wood_config.h"
	#include "../Wood/Pop_drop_right_Wood_config.h"
#endif

typedef Sint16(*sensor_position_fun_t)(void);

typedef enum{
	term_AX12_RX24,
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
 * inc      -> nombre de degrés par incrémentation dans le terminale
 * char		-> le caractère que l'on doit envoyer pour sélectionner l'actionneur
 * prefix   -> le nom de l'actionneur
 * fun      -> fonction qui retourne la position du bras
 */
#define DECLARE_AX12_RX24(inc, char, prefix) {term_AX12_RX24, inc, char, #prefix, prefix##_ID, prefix##_MIN_VALUE, prefix##_MAX_VALUE, 0, NULL, NULL, 0}
#define DECLARE_MOTOR(inc, char, prefix, fun) {term_MOTOR, inc, char, #prefix, prefix##_ID, prefix##_MIN_VALUE, prefix##_MAX_VALUE, 0, fun, NULL, 0}
#define DECLARE_PWM(inc, char, prefix) {term_PWM, inc, char, #prefix, prefix##_PWM_NUM, 0, 100, 0, NULL, prefix##_SENS}

terminal_motor_s terminal_motor[] = {
	// DECLARE_AX12_RX24(2, '0', EXEMPLE_AX12),

	#ifdef I_AM_ROBOT_BIG
		DECLARE_AX12_RX24(2, '0', POP_COLLECT_LEFT_AX12),
		DECLARE_AX12_RX24(2, '1', POP_COLLECT_RIGHT_AX12),
		DECLARE_AX12_RX24(2, '2', POP_DROP_LEFT_AX12),
		DECLARE_AX12_RX24(2, '3', POP_DROP_RIGHT_AX12),
		DECLARE_AX12_RX24(2, '4', BACK_SPOT_LEFT_AX12),
		DECLARE_AX12_RX24(2, '5', BACK_SPOT_RIGHT_AX12),
		DECLARE_PWM(2, '7', SPOT_POMPE_RIGHT),
		DECLARE_AX12_RX24(2, '8', CARPET_LAUNCHER_LEFT_AX12),
		DECLARE_AX12_RX24(2, '9', CARPET_LAUNCHER_RIGHT_AX12)
	#else
		DECLARE_AX12_RX24(2, '0', PINCE_GAUCHE_AX12),
		DECLARE_AX12_RX24(2, '1', PINCE_DROITE_AX12),
		DECLARE_AX12_RX24(2, '3', CLAP_AX12),
		DECLARE_AX12_RX24(2, '4', POP_DROP_LEFT_WOOD_AX12),
		DECLARE_AX12_RX24(2, '5', POP_DROP_RIGHT_WOOD_AX12)
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
	static Uint16 position = 150;
	Uint8 i;

	for(i=0;i<terminal_motor_size;i++){
		if(terminal_motor[i].char_selection == c && state != i){
			debug_printf("%s selected\n", terminal_motor[i].name);
			state = i;
			if(terminal_motor[i].type == term_AX12_RX24)
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
			if(terminal_motor[i].type == term_AX12_RX24)
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
			if(terminal_motor[i].type == term_AX12_RX24)
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

	if(terminal_motor[state].type == term_AX12_RX24)
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
