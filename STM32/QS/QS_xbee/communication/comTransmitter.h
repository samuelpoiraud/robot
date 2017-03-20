#ifndef COM_TRANSMITTER_H_
	#define COM_TRANSMITTER_H_
	#include "../../QS_all.h"

	#ifdef USE_XBEE

		#include "comType.h"
		#include "../../QS_xbee/network/interface/networkSessionObserver.h"
		#include "../../QS_xbee/network/interface/networkSessionController.h"

		networkSessionController_t COM_TRANSMITTER_send(networkSessionTransmitRequestObserver * observer, Uint64 destinationAddress64bit, comMsg_t * msg);

	#endif

#endif /* COM_TRANSMITTER_H_ */
