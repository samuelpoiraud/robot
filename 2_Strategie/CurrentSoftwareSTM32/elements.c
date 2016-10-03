#include "elements.h"
#include "environment.h"

#define LOG_PREFIX "element: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ELEMENTS
#include "QS/QS_outputlog.h"
#include "QS/QS_IHM.h"
#include "QS/QS_can_over_xbee.h"
#include "QS/QS_watchdog.h"
#include "utils/actionChecker.h"
#include "QS/QS_stateMachineHelper.h"
#include "utils/generic_functions.h"

volatile bool_e elements_flags[F_ELEMENTS_FLAGS_NB];

void ELEMENTS_init(){
	Uint8 i;

	for(i=0;i<F_ELEMENTS_FLAGS_NB;i++)
	{
		elements_flags[i] = FALSE;
	}

}

void ELEMENTS_process_main(){

}

bool_e ELEMENTS_get_flag(elements_flags_e flag_id)
{
	assert(flag_id < F_ELEMENTS_FLAGS_NB);
	return elements_flags[flag_id];
}

void ELEMENTS_set_flag(elements_flags_e flag_id, bool_e new_state)
{
	assert(flag_id < F_ELEMENTS_FLAGS_NB);
	elements_flags[flag_id] = new_state;

#ifdef USE_SYNC_ELEMENTS
	if(flag_id < F_ELEMENTS_FLAGS_END_SYNCH)
	{
		CAN_msg_t msg;
		msg.sid = XBEE_SYNC_ELEMENTS_FLAGS;
		msg.size = SIZE_XBEE_SYNC_ELEMENTS_FLAGS;
		msg.data.xbee_sync_elements_flags.flagId = flag_id;
		msg.data.xbee_sync_elements_flags.flag = new_state;
		CANMsgToXbee(&msg,FALSE);
	}
#endif

}

#ifdef USE_SYNC_ELEMENTS
void ELEMENTS_receive_flags(CAN_msg_t* msg)
{
	if(msg->data.xbee_sync_elements_flags.flagId < F_ELEMENTS_FLAGS_END_SYNCH){
		//debug_printf("\n\n\n\nReception message Xbee\n\n\n\n");
		elements_flags[msg->data.xbee_sync_elements_flags.flagId] = msg->data.xbee_sync_elements_flags.flag;
	}
}
#endif


#define	TIMEOUT_ANSWER	200

error_e ELEMENTS_check_communication(CAN_msg_t * msg)
{
#ifdef USE_SYNC_ELEMENTS
	CREATE_MAE(SM_ID_CHECK_COMMUNICATION,
				INIT,
				SEND_REQUEST,
				WAIT_FOR_ANSWER,
				WAIT_TIMEOUT,
				TIMEOUT,
				ANSWER_RECEIVED,
				END);
	static bool_e watchdog_flag = FALSE;
	static watchdog_id_t watchdog_id = 0;

	switch(state)
	{
		case INIT:
			if(msg == NULL)	//On s'autorise à continuer seulement si ce n'est pas la réception d'un message qui nous active...
				state = SEND_REQUEST;
			break;
		case SEND_REQUEST:{
			CAN_msg_t request;
			request.sid = XBEE_COMMUNICATION_AVAILABLE;
			request.size = 0;
			CANMsgToXbee(&request,FALSE);
			state = WAIT_FOR_ANSWER;
			break;}
		case WAIT_FOR_ANSWER:
			if(entrance)
			{
				watchdog_id = WATCHDOG_create_flag(TIMEOUT_ANSWER,&watchdog_flag);
			}
			if(watchdog_flag)
			{
				state = WAIT_TIMEOUT;
			}
			else if(msg)
			{
				if(msg->sid == XBEE_COMMUNICATION_RESPONSE)
				{
					ELEMENTS_set_flag(F_COMMUNICATION_AVAILABLE, TRUE);
					state = ANSWER_RECEIVED;
					WATCHDOG_stop(watchdog_id);
				}
			}
			break;
		case ANSWER_RECEIVED:
			state = END;
			break;

		case WAIT_TIMEOUT:
			state = wait_time(1000, WAIT_TIMEOUT,TIMEOUT);
			break;
		case TIMEOUT:
			RESET_MAE();
			ELEMENTS_set_flag(F_COMMUNICATION_AVAILABLE, FALSE);
			return END_WITH_TIMEOUT;
			break;
		case END:
			RESET_MAE();
			return END_OK;
			break;
		default:
			RESET_MAE();
			break;
	}
	#endif
	return IN_PROGRESS;

}

