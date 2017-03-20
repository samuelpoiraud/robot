#include "sendCANMsg.h"

#ifdef USE_XBEE

	#include "../../comSessionManager.h"
	#include "comSessionRequest.h"

	#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_COMMUNICATION
	#define LOG_PREFIX LOG_PREFIX_COMMUNICATION
	#include "../../../../QS_outputlog.h"

	static void connect(comSession_s *Session, comMsg_t *Msg);
	static void end(comSession_s *Session);

	static const char sessionName[] = "SEND_CAN_MSG";

	void sendCANMsg_create(comSession_s * session, time32_t timeout, Sint8 availableSend, Uint64 destinationAddress64bit, Uint8 nbMsg, CAN_msg_t * msg){

		comMsgId_t idFrame = COM_SESSION_MNG_getNextId();

		comSessionRequestCreate(session, sessionName, destinationAddress64bit, timeout, availableSend, idFrame);

		session->connect = &connect;
		session->end = &end;

		session->msg.header.id = idFrame;
		session->msg.header.type = COM_MSG_TYPE_CAN_MSG;
		session->msg.header.way = COM_MSG_WAY_SEND;
		session->msg.header.size = 0;

		//session->msg.body.canMsgSend.msg = NULL;
	}

	static void connect(comSession_s * session, comMsg_t * msg){

		if(comSessionRequestConnectCheck(session, msg, COM_MSG_TYPE_CAN_MSG) == FALSE)
			return;

		session->msgReceived = TRUE;
	}

	static void end(comSession_s * session){

	}

#endif
