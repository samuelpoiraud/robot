#ifndef NETWORK_SESSION_OBSERVER_H
	#define	NETWORK_SESSION_OBSERVER_H
	#include "../../../QS_all.h"

	#ifdef USE_XBEE
		#include "../../xbeeMsgType.h"
		#include "../../xbeeConfig.h"

		typedef void* networkSessionObserver;

		typedef void (*networkSessionTransmitRequestObserverFct)(void * param, bool_e transmitted);
		typedef struct{
			networkSessionTransmitRequestObserverFct notify;
			void * param;
		}networkSessionTransmitRequestObserver;

		typedef void (*networkSessionAtCmdObserverFct)(void * param, networkMessageReceive_t * frame);
		typedef struct{
			networkSessionAtCmdObserverFct notify;
			void * param;
		}networkSessionAtCmdObserver;

	#endif // def USE_XBEE

#endif /* ifndef NETWORK_SESSION_OBSERVER_H */
