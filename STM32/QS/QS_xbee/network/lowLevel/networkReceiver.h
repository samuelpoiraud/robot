#ifndef NETWORK_RECEIVER_H
	#define	NETWORK_RECEIVER_H
	#include "../../../QS_all.h"

	#ifdef USE_XBEE
		#include "../../xbeeMsgType.h"
		#include "../../xbeeConfig.h"

		void NETWORK_RECEIVER_init();

		bool_e NETWORK_RECEIVER_msgReady();

		bool_e NETWORK_RECEIVER_getMsg(networkMessageReceive_t * networkMessage);

	#endif // def USE_XBEE

#endif /* ifndef NETWORK_RECEIVER_H */
