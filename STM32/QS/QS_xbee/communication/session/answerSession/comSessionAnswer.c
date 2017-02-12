#include "comSessionAnswer.h"

#ifdef USE_XBEE

	#include "../../comTransmitter.h"
	#include "../../comSessionManager.h"

	#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_COMMUNICATION
	#define LOG_PREFIX LOG_PREFIX_COMMUNICATION
	#include "../../../../QS_outputlog.h"

	void comSessionAnswerCreate(comSession_s * session, const char * sessionName, time32_t timeout, Sint8 availableSend, comMsgId_t idFrame){
		session->timeout = timeout;
		session->availableSend = availableSend;
		session->state = ANSWER_RUN_STATE_SEND;
		session->timeBase = global.absolute_time;
		session->ackReceived = FALSE;
		session->ackState = FALSE;
		session->msgReceived = FALSE;
		session->idFrame = idFrame;
		session->sessionName = sessionName;

		session->run = &comSessionAnswerRun;
		session->observer.param = session;
		session->observer.notify = (networkSessionTransmitRequestObserverFct)&comSessionAnswerNotify;
		session->connect = NULL;
		session->end = NULL;

		session->networkSessionController.abord = NULL;
		session->networkSessionController.param = NULL;
	}

	void comSessionAnswerRun(comSession_s * session){
		switch(session->state){
			case ANSWER_RUN_STATE_SEND:
				if(session->availableSend != 0){
					if(session->availableSend != -1)
						session->availableSend--;
					COM_TRANSMITTER_send(&(session->observer), &(session->msg));
					session->state = ANSWER_RUN_STATE_WAIT_ACK;
				}else{
					session->state = ANSWER_RUN_STATE_END_ERROR;
				}
				break;

			case ANSWER_RUN_STATE_WAIT_ACK:
				if(session->ackReceived){
					if(session->ackState){
						session->state = ANSWER_RUN_STATE_END;
					}else{
						session->state = ANSWER_RUN_STATE_ERROR_SEND;
					}
				}else if(global.absolute_time - session->timeBase > session->timeout){
					session->state = ANSWER_RUN_STATE_TIMEOUT;
				}
				break;

			case ANSWER_RUN_STATE_TIMEOUT:
				session->timeBase = global.absolute_time;
				session->state = ANSWER_RUN_STATE_SEND;
				break;

			case ANSWER_RUN_STATE_END:
				if(session->end != NULL)
					session->end(session);
				COM_SESSION_MNG_closeSession(session);
				break;

			case ANSWER_RUN_STATE_END_ERROR:
				COM_SESSION_MNG_closeSession(session);
				break;

			case ANSWER_RUN_STATE_ERROR_SEND:
				session->timeBase = global.absolute_time;
				session->state = ANSWER_RUN_STATE_SEND;
				break;

			case ANSWER_RUN_STATE_ERROR:
				COM_SESSION_MNG_closeSession(session);
				break;
		}
	}

	void comSessionAnswerNotify(comSession_s * session, bool_e transmitted){
		session->ackState = transmitted;
		session->ackReceived = TRUE;
	}

	bool_e comSessionAnswerMsgChecker(comSession_s * session, comMsg_t * msg, comMsgType_e msgType){

		if(msg->header.type != COM_MSG_TYPE_CAN_MSG){
			error_printf("create : %s : Mauvais type\n", session->sessionName);
			return TRUE;
		}

		if(msg->header.way != COM_MSG_WAY_SEND){
			error_printf("create : %s : Mauvais sens\n", session->sessionName);
			return TRUE;
		}

		return FALSE;
	}

#endif
