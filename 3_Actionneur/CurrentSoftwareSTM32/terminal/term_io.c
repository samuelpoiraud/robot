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

#define TIME_TO_REFRESH_POS		500

void uart_chekcer(char c){
	typedef enum{
		NONE,
		DC_MOTOR_TOP_BOT,
		RX24_ARM,
		AX12_FOREARM,
		AX12_SUCKER
	}state_e;
	static state_e state = NONE;
	static Uint16 position;
	static clock_time_t time_ask_position;

	ARM_init();

	switch(c){
		case '0' :
			debug_printf("NONE Selected\n");
			state = NONE;
			break;

		case '1' :
			debug_printf("DC_MOTOR_TOP_BOT Selected\n");
			state = DC_MOTOR_TOP_BOT;
			break;

		case '2' :
			debug_printf("RX24_ARM\n");
			state = RX24_ARM;
			break;

		case '3' :
			debug_printf("AX12_FOREARM Selected\n");
			state = AX12_FOREARM;
			break;

		case '4' :
			debug_printf("AX12_SUCKER Selected\n");
			state = AX12_SUCKER;
			break;
	}

	switch(state){
		case DC_MOTOR_TOP_BOT :
			if(CLOCK_get_time() - time_ask_position > TIME_TO_REFRESH_POS){
				time_ask_position = CLOCK_get_time()*100;
				position = ARM_readDCMPos();
			}
			if(c == '+')
				position += 2;
			else if(c == '-')
				position -= 2;
			DCM_setPosValue(ARM_ACT_UPDOWN_ID, 0, position);
			DCM_goToPos(ARM_ACT_UPDOWN_ID, 0);
			DCM_restart(ARM_ACT_UPDOWN_ID);
			break;

		case RX24_ARM :
			if(CLOCK_get_time() - time_ask_position > TIME_TO_REFRESH_POS){
				time_ask_position = CLOCK_get_time()*100;
				position = AX12_get_position(ARM_ACT_RX24_ID);
			}
			if(c == '+')
				position += 2;
			else if(c == '-')
				position -= 2;
			AX12_set_position(ARM_ACT_RX24_ID, position);
			break;

		case AX12_FOREARM :
			if(CLOCK_get_time() - time_ask_position > TIME_TO_REFRESH_POS){
				time_ask_position = CLOCK_get_time()*100;
				position = AX12_get_position(ARM_ACT_AX12_MID_ID);
			}
			if(c == '+')
				position += 2;
			else if(c == '-')
				position -= 2;
			AX12_set_position(ARM_ACT_AX12_MID_ID, position);
			break;

		case AX12_SUCKER :
			if(CLOCK_get_time() - time_ask_position > TIME_TO_REFRESH_POS){
				time_ask_position = CLOCK_get_time()*100;
				position = AX12_get_position(ARM_ACT_AX12_TRIANGLE_ID);
			}
			if(c == '+')
				position += 2;
			else if(c == '-')
				position -= 2;
			AX12_set_position(ARM_ACT_AX12_TRIANGLE_ID, position);
			break;

		case NONE :
		default :
			break;
	}

}
