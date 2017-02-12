#ifndef COM_SESSION_MANAGER_H
	#define	COM_SESSION_MANAGER_H
	#include "../QS/QS_all.h"

	#ifdef USE_XBEE

		#include "comType.h"
		#include "session/comSessionType.h"

		void COM_SESSION_MNG_init();
		void COM_SESSION_MNG_processMain();

		bool_e COM_SESSION_MNG_openSession(comSession_s * session);
		void COM_SESSION_MNG_closeSession(comSession_s * session);
		void COM_SESSION_MNG_connectSession(comMsg_t *msg);
		comMsgId_t COM_SESSION_MNG_getNextId();

	#endif

#endif /* ifndef COM_SESSION_MANAGER_H */
