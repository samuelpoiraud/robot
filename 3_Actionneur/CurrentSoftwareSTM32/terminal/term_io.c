#include <stdarg.h>
#include "term_io.h"
#include "../QS/QS_all.h"
#include "../QS/QS_uart.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_outputlog.h"
#include "../clock.h"
#include "../config/config_global.h"
#ifdef I_AM_ROBOT_BIG
	#include "../config/config_big/config_pin.h"
	#include "../Pierre/Arm_config.h"
	#include "../Pierre/PFilet_config.h"
	#include "../Pierre/PFruit_config.h"
	#include "../Pierre/PTorch_locker_config.h"
	#include "../Pierre/PGache_config.h"
	#include "../Pierre/Small_arm_config.h"
#else
	#include "../Guy/Arm_config.h"
	#include "../Guy/Small_arm_config.h"
	#include "../config/config_small/config_pin.h"
#endif
#include "../Common/Arm_data.h"
#include "../Common/Arm.h"

typedef struct{
	bool_e is_ax12;
	Uint8 inc_quantum;
	char cara_selection;
	char name[30];
	Uint8 id;
	Sint16 min_value;
	Sint16 max_value;
	sensor_read_fun_t fun;
}terminal_motor_s;

#define DECLARE_AX12_RX24(inc, cara, prefix) {TRUE, inc, cara, #prefix, prefix##_ID, prefix##_MIN_VALUE, prefix##_MAX_VALUE, NULL}
#define DECLARE_MOTOR(inc, cara, prefix, fun) {FALSE, inc, cara, #prefix, prefix##_ID, prefix##_MIN_VALUE, prefix##_MAX_VALUE, fun}

terminal_motor_s terminal_motor[] = {
	DECLARE_AX12_RX24(2, '1', ARM_ACT_RX24),
	DECLARE_AX12_RX24(2, '2', ARM_ACT_AX12_MID),
	DECLARE_AX12_RX24(2, '3', ARM_ACT_AX12_TRIANGLE),
	DECLARE_MOTOR(2,	 '4', ARM_ACT_UPDOWN, ARM_readDCMPos),
	DECLARE_AX12_RX24(2, '5', SMALL_ARM_AX12),
	#ifdef I_AM_ROBOT_BIG
		DECLARE_AX12_RX24(2, '6', FILET_AX12),
		DECLARE_AX12_RX24(2, '7', FRUIT_LABIUM_AX12),
		DECLARE_AX12_RX24(2, '8', TORCH_LOCKER_AX12_1),
		DECLARE_AX12_RX24(2, '9', TORCH_LOCKER_AX12_2),
		DECLARE_AX12_RX24(2, '0', GACHE_AX12),
	#else

	#endif
};

Uint8 terminal_motor_size = sizeof(terminal_motor)/sizeof(terminal_motor_s);

#define EGAL_CARA_INC(c)			(c == 'p' || c == 'P' || c == '+')
#define EGAL_CARA_DEC(c)			(c == 'm' || c == 'M'|| c == '-')
#define EGAL_CARA_PRINT(c)		(c == ' ')
#define EGAL_CARA_HELP(c)		(c == 'h' || c == 'H')



void uart_checker(unsigned char c){

	static Uint8 state = -1;
	static Uint16 position = 150;
	Uint8 i;

	for(i=0;i<terminal_motor_size;i++){
		if(terminal_motor[i].cara_selection == c && state != i){
			debug_printf("%s selected\n", terminal_motor[i].name);
			state = i;
			if(terminal_motor[i].is_ax12)
				position = AX12_get_position(terminal_motor[i].id);
			else
				position = terminal_motor[i].fun();
		}
	}

	if(EGAL_CARA_PRINT(c)){
		debug_printf("---------------------- Position ----------------------\n");
		for(i=0;i<terminal_motor_size;i++){
			if(terminal_motor[i].is_ax12)
				debug_printf("%s : %d\n", terminal_motor[i].name, AX12_get_position(terminal_motor[i].id));
			else
				debug_printf("%s : value : %d /  real : %d\n" , terminal_motor[i].name, terminal_motor[i].fun(), conv_potar_updown_to_dist(terminal_motor[i].fun()));
		}
	}

	if(EGAL_CARA_HELP(c)){
		debug_printf("---------------------- Terminal ----------------------\n");
		debug_printf("Actionneur : \n");
		for(i=0;i<terminal_motor_size;i++){
			if(terminal_motor[i].is_ax12)
				debug_printf("(%s) cmd : %c   ID : %d   NAME : %s\n", AX12_is_ready(terminal_motor[i].id) ? "  Connecté" : "Déconnecté", terminal_motor[i].cara_selection, terminal_motor[i].id, terminal_motor[i].name);
			else
				debug_printf("(%s) cmd : %c   ID : %d   NAME : %s\n", "----------", terminal_motor[i].cara_selection, terminal_motor[i].id, terminal_motor[i].name);
		}
		debug_printf("\nCommande : \n");
		debug_printf("p/+ incrémenter\nm/- décrémenter\nESPACE affichage position\nh affichage aide\n");
	}

	if(state == -1)
		return;

	if(!EGAL_CARA_INC(c) && !EGAL_CARA_DEC(c))
		return;

	if(EGAL_CARA_INC(c))
		position = ((Sint16)position+terminal_motor[state].inc_quantum >= terminal_motor[state].max_value) ? position : position+terminal_motor[state].inc_quantum;
	else if(EGAL_CARA_DEC(c))
		position = ((Sint16)position-terminal_motor[state].inc_quantum <= terminal_motor[state].min_value) ? position : position-terminal_motor[state].inc_quantum;

	if(terminal_motor[state].is_ax12)
		AX12_set_position(terminal_motor[state].id, position);
	else{
		DCM_setPosValue(terminal_motor[state].id, 0, position);
		DCM_goToPos(terminal_motor[state].id, 0);
		DCM_restart(terminal_motor[state].id);
	}
}
