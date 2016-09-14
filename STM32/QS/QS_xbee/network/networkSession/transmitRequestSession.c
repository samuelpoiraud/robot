#include "transmitRequestSession.h"
#include "../lowLevel/networkTransmitter.h"
#include "../networkSessionManager.h"
#include "../../../QS_outputlog.h"

#ifdef USE_XBEE

	typedef enum{
		STATE_SEND,
		STATE_WAIT_ACK,
		STATE_END,
		STATE_ERROR,
		STATE_TIMEOUT,
		STATE_ABORD
	}state_e;

	static void run(networkSession_s *Session);
	static void connect(networkSession_s *Session, networkMessageReceive_t *NetworkMessage);
	static void abord(void *Session);

	networkSession_s transmitRequestSession_create(void * Observer, networkSessionTransmitRequestObserver_ptr Notify, networkMessageTransmit_t Msg, bool_e Ack, time32_t Timeout){

		networkSession_s session;

		session.frameID = Msg.frame.transmitRequest.frameID;
		session.timeout = Timeout;
		session.msgTransmit = Msg;
		session.msgReceived = FALSE;
		session.aborded = FALSE;

		session.state = STATE_SEND;
		session.timeBase = global.absolute_time;

		session.run = &run;
		session.connect = &connect;
		session.notify = NETWORK_CONVERT_SESSION_OBSERVER(Notify);
		session.observer = Observer;
		session.abord = &abord;

		//info_printf("XBEE : %d OPEN\n", session.frameID);

		return session;
	}

	static void run(networkSession_s *Session){
		switch(Session->state){
			case STATE_SEND:
				if(Session->aborded){
					Session->state = STATE_ABORD;
				}else{
					NETWORK_TRANSMITTER_send(&(Session->msgTransmit));
					Session->state = STATE_WAIT_ACK;
				}
				break;

			case STATE_WAIT_ACK:
				if(Session->aborded){
					Session->state = STATE_ABORD;
				}else if(Session->msgReceived){
					if(Session->msgReceive.frame.transmitStatus.deliveryStatus == DELIVERY_STATUS_SUCCESS)
						Session->state = STATE_END;
					else
						Session->state = STATE_ERROR;
				}else if(global.absolute_time - Session->timeBase > Session->timeout){
					Session->state = STATE_TIMEOUT;
				}
				break;

			case STATE_END:
				NETWORK_CONVERT_SESSION_TRANSMIT_REQUEST_OBSERVER(Session->notify)(Session->observer, TRUE);
				NETWORK_SESSION_MNG_closeSession(Session->frameID);
				//info_printf("XBEE : %d CLOSE\n", Session->frameID);
				break;

			case STATE_ERROR:
				NETWORK_CONVERT_SESSION_TRANSMIT_REQUEST_OBSERVER(Session->notify)(Session->observer, FALSE);
				NETWORK_SESSION_MNG_closeSession(Session->frameID);
				//info_printf("XBEE : %d CLOSE ERROR\n", Session->frameID);
				break;

			case STATE_TIMEOUT:
				NETWORK_CONVERT_SESSION_TRANSMIT_REQUEST_OBSERVER(Session->notify)(Session->observer, FALSE);
				NETWORK_SESSION_MNG_closeSession(Session->frameID);
				//info_printf("XBEE : %d CLOSE TIMEOUT\n", Session->frameID);
				break;

			case STATE_ABORD:
				NETWORK_SESSION_MNG_closeSession(Session->frameID);
				break;
		}
	}

	static void connect(networkSession_s *Session, networkMessageReceive_t *NetworkMessage){
		if(NetworkMessage->header.formated.frameType != FRAME_TYPE_TRANSMIT_STATUS){
			error_printf("connect : TRANSMIT_REQUEST : Mauvais frameType\n");
			return;
		}

		Session->msgReceive = *NetworkMessage;
		Session->msgReceived = TRUE;
	}

	static void abord(void *Session){
		((networkSession_s*)Session)->aborded = TRUE;
	}

#endif
