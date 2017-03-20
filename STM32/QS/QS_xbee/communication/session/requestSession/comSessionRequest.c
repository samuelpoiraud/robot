#include "comSessionRequest.h"

#ifdef USE_XBEE

	#include "../../comTransmitter.h"
	#include "../../comSessionManager.h"

	#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_COMMUNICATION
	#define LOG_PREFIX LOG_PREFIX_COMMUNICATION
	#include "../../../../QS_outputlog.h"

	void comSessionRequestCreate(comSession_s * session, const char * sessionName, Uint64 destinationAddress64bit, time32_t timeout, Sint8 availableSend, comMsgId_t idFrame){
		debug_printf("create\n");
		session->timeout = timeout;
		session->availableSend = availableSend;
		session->state = REQUEST_RUN_STATE_SEND;
		session->timeBase = global.absolute_time;
		session->ackReceived = FALSE;
		session->ackState = FALSE;
		session->msgReceived = FALSE;
		session->idFrame = idFrame;
		session->sessionName = sessionName;

		session->destinationAddress64bit = destinationAddress64bit;

		session->run = &comSessionRequestRun;
		session->observer.param = session;
		session->observer.notify = (networkSessionTransmitRequestObserverFct)&comSessionRequestNotify;
		session->connect = NULL;
		session->end = NULL;

		session->networkSessionController.abord = NULL;
		session->networkSessionController.param = NULL;
	}

	void comSessionRequestRun(comSession_s * session){
		switch(session->state){
			case REQUEST_RUN_STATE_SEND:
				debug_printf("send\n");
				if(session->availableSend != 0){
					if(session->availableSend != -1)
						session->availableSend--;
					session->networkSessionController = COM_TRANSMITTER_send(&(session->observer), session->destinationAddress64bit, &(session->msg));
					session->state = REQUEST_RUN_STATE_WAIT_ACK;
				}else{
					session->state = REQUEST_RUN_STATE_ERROR;
				}
				break;

			case REQUEST_RUN_STATE_WAIT_ACK:
				if(session->ackReceived){
					if(session->ackState){
						session->state = REQUEST_RUN_STATE_WAIT_RECEIVE;
					}else{
						session->state = REQUEST_RUN_STATE_ERROR_SEND;
					}
				}else if(global.absolute_time - session->timeBase > session->timeout){
					session->state = REQUEST_RUN_STATE_TIMEOUT;
				}
				break;

			case REQUEST_RUN_STATE_WAIT_RECEIVE:
				if(session->msgReceived){
					session->state = REQUEST_RUN_STATE_END;
				}else if(global.absolute_time - session->timeBase > session->timeout){
					session->state = REQUEST_RUN_STATE_TIMEOUT;
				}
				break;

			case REQUEST_RUN_STATE_TIMEOUT:
				debug_printf("timeout\n");
				session->idFrame = COM_SESSION_MNG_getNextId();
				session->msg.header.id= session->idFrame;

				session->state = REQUEST_RUN_STATE_SEND;
				session->timeBase = global.absolute_time;
				session->ackReceived = FALSE;
				session->ackState = FALSE;
				session->msgReceived = FALSE;

				break;

			case REQUEST_RUN_STATE_END:
				if(session->end != NULL)
					session->end(session);
				COM_SESSION_MNG_closeSession(session);
				break;

			case REQUEST_RUN_STATE_ERROR_SEND:
				debug_printf("error\n");
				session->idFrame = COM_SESSION_MNG_getNextId();
				session->msg.header.id = session->idFrame;

				session->state = REQUEST_RUN_STATE_SEND;
				session->timeBase = global.absolute_time;
				session->ackReceived = FALSE;
				session->ackState = FALSE;
				session->msgReceived = FALSE;

				break;

			case REQUEST_RUN_STATE_ERROR:
				COM_SESSION_MNG_closeSession(session);
				break;
		}
	}

	void comSessionRequestNotify(comSession_s * session, bool_e transmitted){
		session->ackState = transmitted;
		session->ackReceived = TRUE;
	}

	bool_e comSessionRequestConnectCheck(comSession_s * session, comMsg_t * msg, comMsgType_e msgType){
		if(session->state != REQUEST_RUN_STATE_WAIT_RECEIVE){
			error_printf("connect : %s : Réception de réponse mais pas d'acquittement du XBEE\n", session->sessionName);

		}else if(msg->header.type != msgType){
			error_printf("connect : %s : Mauvais msgType\n", session->sessionName);

		}else if(msg->header.way != COM_MSG_WAY_ACK){
			error_printf("connect : %s : Mauvais type\n", session->sessionName);

		}

		return FALSE;
	}

#endif
