#ifndef NETWORK_SYSTEM_H
	#define	NETWORK_SYSTEM_H
	#include "../../QS_all.h"

	#ifdef USE_XBEE
		#include "../xbeeMsgType.h"
		#include "../xbeeConfig.h"

		void NETWORK_SYSTEM_init();

		void NETWORK_SYSTEM_processMain();

	#endif // def USE_XBEE

#endif /* ifndef NETWORK_SYSTEM_H */
