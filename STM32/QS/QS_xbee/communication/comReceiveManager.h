#ifndef COM_RECEIVE_MANAGER_H_
	#define COM_RECEIVE_MANAGER_H_
	#include "../../../QS/QS_all.h"

	#ifdef USE_XBEE

		#include "comType.h"

		void COM_RECEIVE_MANAGER_dispatch(comMsg_t *msg);

	#endif

#endif /* COM_RECEIVE_MANAGER_H_ */
