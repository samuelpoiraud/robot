#ifndef NETWORK_SESSION_MANAGER_H
	#define	NETWORK_SESSION_MANAGER_H
	#include "../../QS_all.h"

	#ifdef USE_XBEE
		#include "../xbeeMsgType.h"
		#include "../xbeeConfig.h"
		#include "networkSession/networkSession.h"

		void NETWORK_SESSION_MNG_init();
		void NETWORK_SESSION_MNG_processMain();

		networkSession_s * NETWORK_SESSION_MNG_openSession(networkSession_s * networkSession);
		void NETWORK_SESSION_MNG_closeSession(Uint8 frameID);

	#endif // def USE_XBEE

#endif /* ifndef NETWORK_SESSION_MANAGER_H */
