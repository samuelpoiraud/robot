#include "comReceiveManager.h"

#ifdef USE_XBEE

	#include "comSessionManager.h"
	#include "session/answerSession/receiveCANMsg.h"

	#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_COMMUNICATION
	#define LOG_PREFIX LOG_PREFIX_COMMUNICATION
	#include "../../QS_outputlog.h"

	static void COM_RECEIVE_MANAGER_sessionBuilder(comMsg_t * msg);


	void COM_RECEIVE_MANAGER_dispatch(comMsg_t * msg){

		if(msg->header.way == COM_MSG_WAY_ACK){
			COM_SESSION_MNG_connectSession(msg);
		}else{
			COM_RECEIVE_MANAGER_sessionBuilder(msg);
		}
	}

	static void COM_RECEIVE_MANAGER_sessionBuilder(comMsg_t * msg){
		comSession_s session;
		bool_e found = TRUE;

		switch(msg->header.type){
			case COM_MSG_TYPE_CAN_MSG:
				receiveCANMsg_create(&session, COM_SESSION_DEFAULT_TIMEOUT, COM_SESSION_DEFAULT_AVAILABE_SEND, msg);
				break;

			default:
				found = FALSE;
				break;
		}

		if(found){
			COM_SESSION_MNG_openSession(&session);
		}
	}

#endif
