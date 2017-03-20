#include "comTransmitter.h"

#ifdef USE_XBEE

	#include "../../QS_xbee/network/networkFunctionTransmit.h"
	#include "../QS_canOverXbee.h"

	#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_COMMUNICATION
	#define LOG_PREFIX LOG_PREFIX_COMMUNICATION
	#include "../../QS_outputlog.h"

	static void COM_TRANSMITTER_serialize(comMsg_t * Msg, Uint8 Data[], Uint8 *NbData);

	networkSessionController_t COM_TRANSMITTER_send(networkSessionTransmitRequestObserver * observer, Uint64 destinationAddress64bit, comMsg_t * msg){
		Uint8 data[COM_MSG_SIZE_MAX];
		Uint8 nbData;

		info_printf("O - %d  |  %d  |  %s\n", msg->header.type, msg->header.id, (msg->header.way == COM_MSG_WAY_SEND)?"COM_MSG_WAY_SEND":"COM_MSG_WAY_ACK");
		COM_TRANSMITTER_serialize(msg, data, &nbData);

		return transmitRequest(observer, data, nbData, TRUE, destinationAddress64bit, 0xFFFE);
	}

	static void COM_TRANSMITTER_serialize(comMsg_t * msg, Uint8 data[], Uint8 *nbData){
		*nbData = 0;
		Uint8 i;

		for(i=0; i < COM_MSG_SIZE_HEADER; i++)
			data[(*nbData)++] = msg->header.rawData[i];

		for(i=0; i < msg->header.size; i++){
			if(i + COM_MSG_SIZE_HEADER > COM_MSG_SIZE_MAX){
				error_printf("serializer overflow\n");
				return;
			}

			data[(*nbData)++] = msg->body.rawData[i];
		}
	}

#endif
