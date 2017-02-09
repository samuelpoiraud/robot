#ifndef NETWORK_FUNCTION_TRANSMIT_H
	#define	NETWORK_FUNCTION_TRANSMIT_H
	#include "../../QS_all.h"

	#ifdef USE_XBEE
		#include "../xbeeMsgType.h"
		#include "../xbeeConfig.h"

		#include "interface/networkSessionObserver.h"
		#include "interface/networkSessionController.h"

		networkSessionController_t transmitRequest(networkSessionTransmitRequestObserver * observer, Uint8 Data[], Uint8 NbData, bool_e Ack, Uint64 DestinationAddress64bit, Uint16 DestinationAddress16bit);

	#endif // def USE_XBEE

#endif /* ifndef NETWORK_FUNCTION_TRANSMIT_H */
