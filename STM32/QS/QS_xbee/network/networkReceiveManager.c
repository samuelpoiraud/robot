#include "networkReceiveManager.h"
#include "lowLevel/networkReceiver.h"

#ifdef USE_XBEE

	#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_XBEE
	#define LOG_PREFIX LOG_PREFIX_XBEE
	#include "../../QS_outputlog.h"

	#define MAX_NB_OBSERVER	10

	typedef struct{
		bool_e used;
		networkReceiveObserver_ptr notify;
		frameType_e frameType;
	}networkReceiveObserver_s;

	networkReceiveObserver_s observers[MAX_NB_OBSERVER];

	static void NETWORK_RECEIVE_MANAGER_notify(networkMessageReceive_t *Frame);

	void NETWORK_RECEIVE_MANAGER_init(){
		Uint8 i;
		for(i=0; i<MAX_NB_OBSERVER; i++){
			observers[i].used = FALSE;
		}
	}

	void NETWORK_RECEIVE_MANAGER_processMain(){

		networkMessageReceive_t msg;

		while(NETWORK_RECEIVER_getMsg(&msg)){
			info_printf("I - 0x%2x\n", msg.header.formated.frameType);

			NETWORK_RECEIVE_MANAGER_notify(&msg);
		}
	}

	void NETWORK_RECEIVE_MANAGER_attach(networkReceiveObserver_ptr Obs, frameType_e FrameType){
		Uint8 i;
		bool_e found = FALSE;
		for(i=0; i<MAX_NB_OBSERVER; i++){
			if(observers[i].used == FALSE && found == FALSE){
				found = TRUE;
				observers[i].used = TRUE;
				observers[i].notify = Obs;
				observers[i].frameType= FrameType;
			}
		}

		if(!found)
			error_printf("Attach impossible (overflow) pour le FrameType 0x%2x\n", FrameType);
		else
			info_printf("Attach réussie pour le FrameType 0x%2x\n", FrameType);

	}

	void NETWORK_RECEIVE_MANAGER_deattach(networkReceiveObserver_ptr Obs, frameType_e FrameType){
		Uint8 i;
		for(i=0; i<MAX_NB_OBSERVER; i++){
			if(observers[i].notify == Obs && observers[i].frameType == FrameType)
				observers[i].used = FALSE;
		}
	}

	static void NETWORK_RECEIVE_MANAGER_notify(networkMessageReceive_t *Msg){
		Uint8 i;
		for(i=0; i<MAX_NB_OBSERVER; i++){
			if(observers[i].used == TRUE && observers[i].frameType == Msg->header.formated.frameType)
				observers[i].notify(Msg);
		}
	}

#endif
