#ifndef NETWORK_TRANSMITTER_H
	#define	NETWORK_TRANSMITTER_H
	#include "../../../QS_all.h"

	#ifdef USE_XBEE
		#include "../../xbeeMsgType.h"
		#include "../../xbeeConfig.h"

		void NETWORK_TRANSMITTER_send(networkMessageTransmit_t *networkMessage);

	#endif // def USE_XBEE

#endif /* ifndef NETWORK_TRANSMITTER_H */
