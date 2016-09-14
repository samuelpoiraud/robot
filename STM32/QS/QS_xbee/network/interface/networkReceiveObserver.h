#ifndef NETWORK_RECEIVE_OBSERVER_H
	#define	NETWORK_RECEIVE_OBSERVER_H
	#include "../../../QS_all.h"

	#ifdef USE_XBEE
		#include "../../xbeeMsgType.h"
		#include "../../xbeeConfig.h"

		typedef void (*networkReceiveObserver_ptr)(networkMessageReceive_t *Msg);

	#endif // def USE_XBEE

#endif /* ifndef NETWORK_RECEIVE_OBSERVER_H */
