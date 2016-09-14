#ifndef NETWORK_SESSION_OBSERVER_H
	#define	NETWORK_SESSION_OBSERVER_H
	#include "../../../QS_all.h"

	#ifdef USE_XBEE
		#include "../../xbeeMsgType.h"
		#include "../../xbeeConfig.h"

		typedef void* networkSessionObserver_ptr;
		#define NETWORK_CONVERT_SESSION_OBSERVER(x)							((void*)(x))

		typedef void (*networkSessionTransmitRequestObserver_ptr)(void *, bool_e transmitted);
		#define NETWORK_CONVERT_SESSION_TRANSMIT_REQUEST_OBSERVER(x)		((networkSessionTransmitRequestObserver_ptr)(x))

		typedef void (*networkSessionAtCmdObserver_ptr)(void *, networkMessageReceive_t *frame);
		#define NETWORK_CONVERT_SESSION_AT_CMD_OBSERVER(x)					((networkSessionAtCmdObserver_ptr)(x))

	#endif // def USE_XBEE

#endif /* ifndef NETWORK_SESSION_OBSERVER_H */
