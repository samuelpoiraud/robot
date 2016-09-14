#ifndef NETWORK_RECEIVE_MANAGER_H
	#define	NETWORK_RECEIVE_MANAGER_H
	#include "../../QS_all.h"

	#ifdef USE_XBEE
		#include "../xbeeMsgType.h"
		#include "../xbeeConfig.h"
		#include "interface/networkReceiveObserver.h"

		void NETWORK_RECEIVE_MANAGER_init();

		void NETWORK_RECEIVE_MANAGER_processMain();

		void NETWORK_RECEIVE_MANAGER_attach(networkReceiveObserver_ptr Obs, frameType_e FrameType);
		void NETWORK_RECEIVE_MANAGER_deattach(networkReceiveObserver_ptr Obs, frameType_e FrameType);


	#endif // def USE_XBEE

#endif /* ifndef NETWORK_RECEIVE_MANAGER_H */
