#include "comReceiver.h"

#ifdef USE_XBEE

	#include "../../QS_xbee/xbeeMsgType.h"
	#include "../../QS_xbee/network/interface/networkReceiveObserver.h"
	#include "../../QS_xbee/network/networkReceiveManager.h"
	#include "../QS_canOverXbee.h"
	#include "comType.h"
	#include "comReceiveManager.h"

	#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_COMMUNICATION
	#define LOG_PREFIX LOG_PREFIX_COMMUNICATION
	#include "../../QS_outputlog.h"

	static void COM_RECEIVER_notify(networkMessageReceive_t * msg);
	static bool_e COM_RECEIVER_parse(networkMessageReceive_t * msgNetwork, comMsg_t * msgCom);

	void COM_RECEIVER_init(){
		NETWORK_RECEIVE_MANAGER_attach(&COM_RECEIVER_notify, FRAME_TYPE_RECEIVE_PACKET);
	}

	static void COM_RECEIVER_notify(networkMessageReceive_t * msgNetwork){
		comMsg_t msgCom;
		if(COM_RECEIVER_parse(msgNetwork, &msgCom)){
			info_printf("I - %d  |  %d  |  %s\n", msgCom.header.type, msgCom.header.id, ((msgCom.header.way == COM_MSG_WAY_SEND)?"COM_MSG_WAY_SEND":"COM_MSG_WAY_ACK"));
			COM_RECEIVE_MANAGER_dispatch(&msgCom);
		}
	}

	static bool_e COM_RECEIVER_parse(networkMessageReceive_t * msgNetwork, comMsg_t * msgCom){
		if(msgNetwork->header.formated.frameType != FRAME_TYPE_RECEIVE_PACKET){
			error_printf("parse : Parse d'un mauvais type de trame %d != RECEIVE_PACKET\n", msgNetwork->header.formated.frameType);
			return FALSE;
		}

		if(msgNetwork->header.formated.lenght < RECEIVE_PACKET_SIZE + COM_MSG_SIZE_MAX){
			error_printf("parse : Message trop court %d\n", msgNetwork->header.formated.lenght);
			return FALSE;
		}

		if(CAN_OVER_XBEE_getRobotByDestinationAddress64bit(msgNetwork->frame.receivePacket.destinationAddress64bit)){
			error_printf("parse : Provenance ( 0x%llx )incorrecte\n", msgNetwork->frame.receivePacket.destinationAddress64bit);
			return FALSE;
		}

		Uint8 i;
		for(i=0; i < COM_MSG_SIZE_HEADER; i++){
			msgCom->header.rawData[i] = msgNetwork->frame.receivePacket.receivedData[i];
		}

		for(; i < msgNetwork->header.formated.lenght - 1; i++){
			msgCom->body.rawData[i-COM_MSG_SIZE_HEADER] = msgNetwork->frame.receivePacket.receivedData[i];
		}

		msgCom->header.size = msgNetwork->header.formated.lenght - RECEIVE_PACKET_SIZE - COM_MSG_SIZE_HEADER;

		return TRUE;
	}

#endif
