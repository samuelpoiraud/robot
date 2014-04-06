#include <stdarg.h>
#include "term_io.h"
#include "../QS/QS_all.h"
#include "../QS/QS_uart.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_DCMotor2.h"
#include "../QS/QS_outputlog.h"
#include "../clock.h"
#ifdef I_AM_ROBOT_BIG
	#include "../Pierre/Arm_config.h"
	#include "../config/config_big/config_pin.h"
#else
	#include "../Guy/Arm_config.h"
	#include "../config/config_small/config_pin.h"
#endif
#include "../Common/Arm_data.h"
#include "../Common/Arm.h"
#include "../Common/Small_arm_config.h"

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
	DECLARE_MOTOR(2, '5', ARM_ACT_UPDOWN, ARM_readDCMPos),
	DECLARE_AX12_RX24(2, '4', SMALL_ARM_AX12)
};

Uint8 terminal_motor_size = sizeof(terminal_motor)/sizeof(terminal_motor_s);

#define EGAL_CARA_INC(c)			(c == 'p' || c == '+')
#define EGAL_CARA_DEC(c)			(c == 'm' || c == '-')
#define EGAL_CARA_PRINT(c)		(c == ' ')



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
		for(i=0;i<terminal_motor_size;i++){
			if(terminal_motor[i].is_ax12)
				debug_printf("%s : %d\n", terminal_motor[i].name, AX12_get_position(terminal_motor[i].id));
			else
				debug_printf("%s : %d\n" , terminal_motor[i].name, terminal_motor[i].fun());
		}
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
