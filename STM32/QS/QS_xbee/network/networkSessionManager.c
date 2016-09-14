#include "networkSessionManager.h"
#include "networkReceiveManager.h"
#include "../../QS_outputlog.h"

#ifdef USE_XBEE

	#define MAX_NB_SESSION	10

	typedef struct{
		bool_e used;
		networkSession_s networkSession;
	}networkSessionMng_s;

	typedef Uint8 networkHashKey_t;

	static networkSessionMng_s networkSessionMng[MAX_NB_SESSION];

	static networkHashKey_t NETWORK_SESSION_MNG_keyGenerator(Uint8 FrameID);
	static void NETWORK_SESSION_MNG_notify(networkMessageReceive_t * NetworkMessage);
	static bool_e NETWORK_SESSION_MNG_connectSession(networkMessageReceive_t * NetworkMessage, Uint8 FrameID);

	void NETWORK_SESSION_MNG_init(){
		Uint8 i;
		for(i=0; i<MAX_NB_SESSION; i++){
			networkSessionMng[i].used = FALSE;
		}

		NETWORK_RECEIVE_MANAGER_attach(&NETWORK_SESSION_MNG_notify, FRAME_TYPE_TRANSMIT_STATUS);
		NETWORK_RECEIVE_MANAGER_attach(&NETWORK_SESSION_MNG_notify, FRAME_TYPE_AT_COMMAND_RESPONSE);
		NETWORK_RECEIVE_MANAGER_attach(&NETWORK_SESSION_MNG_notify, FRAME_TYPE_REMOTE_AT_COMMAND_RESPONSE);
	}

	void NETWORK_SESSION_MNG_processMain(){
		Uint8 i;
		for(i=0; i<MAX_NB_SESSION; i++){
			if(networkSessionMng[i].used && networkSessionMng[i].networkSession.run != NULL)
				networkSessionMng[i].networkSession.run(&(networkSessionMng[i].networkSession));
		}
	}

	networkSession_s * NETWORK_SESSION_MNG_openSession(networkSession_s * networkSession){
		assert(networkSession->run != NULL);
		assert(networkSession->connect != NULL);

		networkHashKey_t key = NETWORK_SESSION_MNG_keyGenerator(networkSession->frameID);

		if(networkSessionMng[key].used)
			return NULL;

		networkSessionMng[key].used = TRUE;
		networkSessionMng[key].networkSession = *networkSession;

		return &(networkSessionMng[key].networkSession);
	}

	void NETWORK_SESSION_MNG_closeSession(Uint8 frameID){
		networkHashKey_t key = NETWORK_SESSION_MNG_keyGenerator(frameID);
		networkSessionMng[key].used = FALSE;
	}

	static bool_e NETWORK_SESSION_MNG_connectSession(networkMessageReceive_t *NetworkMessage, Uint8 FrameID){
		networkHashKey_t key = NETWORK_SESSION_MNG_keyGenerator(FrameID);

		if(networkSessionMng[key].used == FALSE)
			return FALSE;

		networkSessionMng[key].networkSession.connect(&(networkSessionMng[key].networkSession), NetworkMessage);

		return TRUE;
	}

	static networkHashKey_t NETWORK_SESSION_MNG_keyGenerator(Uint8 FrameID){
		return (FrameID % MAX_NB_SESSION);
	}

	static void NETWORK_SESSION_MNG_notify(networkMessageReceive_t *networkMessage){
		switch(networkMessage->header.formated.frameType){
			case FRAME_TYPE_TRANSMIT_STATUS:
				NETWORK_SESSION_MNG_connectSession(networkMessage, networkMessage->frame.transmitStatus.frameID);
				break;

			case FRAME_TYPE_AT_COMMAND_RESPONSE:
				NETWORK_SESSION_MNG_connectSession(networkMessage, networkMessage->frame.atCommandResponse.frameID);
				break;

			case FRAME_TYPE_REMOTE_AT_COMMAND_RESPONSE:
				NETWORK_SESSION_MNG_connectSession(networkMessage, networkMessage->frame.remoteAtCommandResponse.frameID);
				break;

			default:
				break;
		}
	}

#endif
