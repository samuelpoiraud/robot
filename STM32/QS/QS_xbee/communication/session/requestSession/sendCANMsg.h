#ifndef SEND_CAN_MSG_H_
	#define SEND_CAN_MSG_H_
	#include "../../../../QS_all.h"

	#ifdef USE_XBEE

		#include "../../comType.h"
		#include "../comSessionType.h"

		void sendCANMsg_create(comSession_s *Session, time32_t Timeout, Sint8 AvailableSend, Uint8 nbMsg, CAN_msg_t * msg);

	#endif

#endif /* SEND_CAN_MSG_H_ */
