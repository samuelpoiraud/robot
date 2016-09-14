#ifndef NETWORK_SESSION_H
	#define	NETWORK_SESSION_H
	#include "../../../QS_all.h"

	#ifdef USE_XBEE
		#include "../../xbeeMsgType.h"
		#include "../../xbeeConfig.h"
		#include "../interface/networkSessionObserver.h"
		#include "../interface/networkSessionController.h"

		struct networkSession{
		/// Config attribute
			Uint8 frameID;
			time32_t timeout;

		/// Internal attribute
			Uint8 state;
			time32_t timeBase;
			networkMessageTransmit_t msgTransmit;


		/// Communication flag
			bool_e msgReceived;
			bool_e aborded;

		/// Communication data
			networkMessageReceive_t msgReceive;

		/// Public method
			void (*run)(struct networkSession *Session);
			void (*connect)(struct networkSession *Session, networkMessageReceive_t *networkMessage);
			networkSessionObserver_ptr notify;
			void * observer;
			networkSessionController_ptr abord;
		};

		typedef struct networkSession networkSession_s;

	#endif // def USE_XBEE

#endif /* ifndef NETWORK_SESSION_H */
