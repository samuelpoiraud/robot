#include <stdarg.h>
#include "term_io.h"
#include "../QS/QS_all.h"
#include "../QS/QS_uart.h"
#include "../QS/QS_ax12.h"
#include "../QS/QS_outputlog.h"
#include "../clock.h"
#include "../Common/Arm_config.h"
#include "../Common/Arm_data.h"
#include "../Common/Arm.h"

#define TIME_TO_REFRESH_POS		2000

#define TIME_SET_POSITION	500

#define INC_DC_MOTOR		2
#define INC_RX24_ARM		2
#define INC_AX12_FOREARM	2
#define INC_AX12_SUCKER		2

#define CARA_INC			'p'
#define CARA_DEC			'm'
#define CARA_PRINT			' '

void uart_checker(char c){
	typedef enum{
		NONE,
		DC_MOTOR_TOP_BOT,
		RX24_ARM,
		AX12_FOREARM,
		AX12_SUCKER
	}state_e;
	static state_e state = NONE;
	static Uint16 position = 150;
	static clock_time_t time_ask_position = 0;

	ARM_init();

	switch(c){
		case '0' :
			debug_printf("NONE Selected\n");
			state = NONE;
			break;

		case '1' :
			debug_printf("DC_MOTOR_TOP_BOT Selected\n");
			state = DC_MOTOR_TOP_BOT;
			position = ARM_readDCMPos();
			time_ask_position = CLOCK_get_time()*100;
			break;

		case '2' :
			debug_printf("RX24_ARM\n");
			state = RX24_ARM;
			position = AX12_get_position(ARM_ACT_RX24_ID);
			time_ask_position = CLOCK_get_time()*100;
			break;

		case '3' :
			debug_printf("AX12_FOREARM Selected\n");
			state = AX12_FOREARM;
			position = AX12_get_position(ARM_ACT_AX12_MID_ID);
			time_ask_position = CLOCK_get_time()*100;
			break;

		case '4' :
			debug_printf("AX12_SUCKER Selected\n");
			state = AX12_SUCKER;
			position = AX12_get_position(ARM_ACT_AX12_TRIANGLE_ID);
			time_ask_position = CLOCK_get_time()*100;
			break;

		case CARA_PRINT :
			debug_printf("{%d, %d, %d, %d}\n", ARM_readDCMPos(), AX12_get_position(ARM_ACT_RX24_ID),
						 AX12_get_position(ARM_ACT_AX12_MID_ID), AX12_get_position(ARM_ACT_AX12_TRIANGLE_ID));
			break;
	}

	if(c != CARA_INC && c != CARA_DEC)
		return;

	switch(state){
		case DC_MOTOR_TOP_BOT :
			if(CLOCK_get_time()*100 - time_ask_position > TIME_TO_REFRESH_POS){
				time_ask_position = CLOCK_get_time()*100;
				position = ARM_readDCMPos();
			}
			if(c == CARA_INC)
				position = (position+INC_DC_MOTOR > ARM_ACT_UPDOWN_MAX_VALUE) ? position : position+INC_DC_MOTOR;
			else if(c == CARA_DEC)
				position = ((Sint16)position-INC_DC_MOTOR < ARM_ACT_UPDOWN_MIN_VALUE) ? position : position-INC_DC_MOTOR;
				DCM_setPosValue(ARM_ACT_UPDOWN_ID, 0, position);
				DCM_goToPos(ARM_ACT_UPDOWN_ID, 0);
				DCM_restart(ARM_ACT_UPDOWN_ID);
			break;

		case RX24_ARM :
			if(CLOCK_get_time()*100 - time_ask_position > TIME_TO_REFRESH_POS){
				time_ask_position = CLOCK_get_time()*100;
				position = AX12_get_position(ARM_ACT_RX24_ID);
			}
			if(c == CARA_INC)
				position = (position+INC_RX24_ARM> ARM_ACT_RX24_MAX_VALUE) ? position : position+INC_RX24_ARM;
			else if(c == CARA_DEC)
				position = ((Sint16)position-INC_RX24_ARM < ARM_ACT_RX24_MIN_VALUE) ? position : position-INC_RX24_ARM;
				AX12_set_position(ARM_ACT_RX24_ID, position);
			break;

		case AX12_FOREARM :
			if(CLOCK_get_time()*100 - time_ask_position > TIME_TO_REFRESH_POS){
				time_ask_position = CLOCK_get_time()*100;
				position = AX12_get_position(ARM_ACT_AX12_MID_ID);
			}
			if(c == CARA_INC)
				position = (position+INC_AX12_FOREARM> ARM_ACT_AX12_MID_MAX_VALUE) ? position : position+INC_AX12_FOREARM;
			else if(c == CARA_DEC)
				position = ((Sint16)position-INC_AX12_FOREARM < ARM_ACT_AX12_MID_MIN_VALUE) ? position : position-INC_AX12_FOREARM;
				AX12_set_position(ARM_ACT_AX12_MID_ID, position);
			break;

		case AX12_SUCKER :
			if(CLOCK_get_time()*100 - time_ask_position > TIME_TO_REFRESH_POS){
				time_ask_position = CLOCK_get_time()*100;
				position = AX12_get_position(ARM_ACT_AX12_TRIANGLE_ID);
			}
			if(c == CARA_INC)
				position = (position+INC_AX12_SUCKER> ARM_ACT_AX12_TRIANGLE_MAX_VALUE) ? position : position+INC_AX12_SUCKER;
			else if(c == CARA_DEC)
				position = ((Sint16)position-INC_AX12_SUCKER < ARM_ACT_AX12_TRIANGLE_MIN_VALUE) ? position : position-INC_AX12_SUCKER;
				AX12_set_position(ARM_ACT_AX12_TRIANGLE_ID, position);
			break;

		case NONE :
		default :
			break;
	}

}
