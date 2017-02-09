#ifndef TRANSMIT_REQUEST_SESSION_H
	#define	TRANSMIT_REQUEST_SESSION_H
	#include "../../../QS_all.h"

	#ifdef USE_XBEE
		#include "../../xbeeMsgType.h"
		#include "../../xbeeConfig.h"
		#include "networkSession.h"

	networkSession_s transmitRequestSession_create(networkSessionTransmitRequestObserver * observer, networkMessageTransmit_t Msg, bool_e Ack, time32_t Timeout);

	#endif // def USE_XBEE

#endif /* ifndef TRANSMIT_REQUEST_SESSION_H */
