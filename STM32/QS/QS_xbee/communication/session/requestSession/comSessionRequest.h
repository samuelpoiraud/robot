#ifndef COM_SESSION_REQUEST_H_
	#define COM_SESSION_REQUEST_H_
	#include "../../../../QS_all.h"

	#ifdef USE_XBEE

		#include "../../comType.h"
		#include "../comSessionType.h"

		typedef enum{
			REQUEST_RUN_STATE_SEND = 0,
			REQUEST_RUN_STATE_WAIT_ACK,
			REQUEST_RUN_STATE_WAIT_RECEIVE,
			REQUEST_RUN_STATE_TIMEOUT,
			REQUEST_RUN_STATE_END,
			REQUEST_RUN_STATE_ERROR_SEND,
			REQUEST_RUN_STATE_ERROR
		}comSessionRequestRunState_e;

		void comSessionRequestCreate(comSession_s * session, const char * sessionName,  Uint64 destinationAddress64bit, time32_t timeout, Sint8 availableSend, comMsgId_t IdFrame);
		void comSessionRequestRun(comSession_s * session);
		void comSessionRequestNotify(comSession_s * session, bool_e transmitted);
		bool_e comSessionRequestConnectCheck(comSession_s * session, comMsg_t * Msg, comMsgType_e msgType);

	#endif

#endif /* COM_SESSION_REQUEST_H_ */
