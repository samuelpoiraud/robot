#ifndef COM_SESSION_ANSWER_H_
	#define COM_SESSION_ANSWER_H_
	#include "../../../../QS_all.h"

	#ifdef USE_XBEE

		#include "../../comType.h"
		#include "../comSessionType.h"

		typedef enum{
			ANSWER_RUN_STATE_SEND = 0,
			ANSWER_RUN_STATE_WAIT_ACK,
			ANSWER_RUN_STATE_WAIT_RECEIVE,
			ANSWER_RUN_STATE_TIMEOUT,
			ANSWER_RUN_STATE_END,
			ANSWER_RUN_STATE_END_ERROR,
			ANSWER_RUN_STATE_ERROR_SEND,
			ANSWER_RUN_STATE_ERROR
		}comSessionAnswertRunState_e;

		void comSessionAnswerCreate(comSession_s * session, const char * sessionName, time32_t timeout, Sint8 availableSend, comMsgId_t idFrame);
		void comSessionAnswerRun(comSession_s * session);
		void comSessionAnswerNotify(comSession_s * session, bool_e transmitted);
		bool_e comSessionAnswerMsgChecker(comSession_s * session, comMsg_t * msg, comMsgType_e msgType);

	#endif

#endif /* COM_SESSION_ANSWER_H_ */
