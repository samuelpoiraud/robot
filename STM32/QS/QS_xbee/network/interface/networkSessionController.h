#ifndef NETWORK_SESSION_CONTROLLER_H
	#define	NETWORK_SESSION_CONTROLLER_H
	#include "../../../QS_all.h"

	#ifdef USE_XBEE
		#include "../../xbeeMsgType.h"
		#include "../../xbeeConfig.h"

		typedef void (*networkSessionController_ptr)(void * param);

		typedef struct{
			networkSessionController_ptr abord;
			void * param;
		}networkSessionController_t;

	#endif // def USE_XBEE

#endif /* ifndef NETWORK_SESSION_CONTROLLER_H */
