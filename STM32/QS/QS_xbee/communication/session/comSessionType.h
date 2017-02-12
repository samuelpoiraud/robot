#ifndef COM_SESSION_H_
	#define COM_SESSION_H_
	#include "../../../QS_all.h"

	#ifdef USE_XBEE

		#include "../comType.h"
		#include "../../../QS_xbee/network/interface/networkSessionController.h"
		#include "../../../QS_xbee/network/interface/networkSessionObserver.h"

		#define COM_SESSION_DEFAULT_TIMEOUT				5000
		#define COM_SESSION_DEFAULT_AVAILABE_SEND		3

		typedef Uint8 sessionID_t;

		struct comSession{
			/// Config attribute
			sessionID_t sessionID;
			comMsgId_t idFrame;
			time32_t timeout;
			Sint8 availableSend;			// Nombre d'envoi possible restant (Si -1 infinie)

			/// Internal attribute
			Uint8 state;
			time32_t timeBase;
			comMsg_t msg;
			bool_e msgReceived;
			bool_e ackReceived;
			bool_e ackState;
			networkSessionController_t networkSessionController;
			const char * sessionName;

			/// Public method
			void (*run)(struct comSession *session);
			void (*connect)(struct comSession *session, comMsg_t *msg);
			void (*end)(struct comSession *session);
			networkSessionTransmitRequestObserver observer;
		};

		typedef struct comSession comSession_s;

	#endif

#endif /* COM_SESSION_H_ */
