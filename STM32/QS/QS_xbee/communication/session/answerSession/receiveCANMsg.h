#ifndef RECEIVE_CAN_MSG_H_
	#define RECEIVE_CAN_MSG_H_
	#include "../../../../QS_all.h"

	#ifdef USE_XBEE

		#include "../../comType.h"
		#include "../comSessionType.h"

		void receiveCANMsg_create(comSession_s * session, time32_t timeout, Sint8 availableSend, comMsg_t * msg);

	#endif

#endif /* RECEIVE_CAN_MSG_H_ */
