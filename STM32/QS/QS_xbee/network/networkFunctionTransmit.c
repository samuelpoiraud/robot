#include "networkFunctionTransmit.h"
#include "networkSession/transmitRequestSession.h"
#include "networkSessionManager.h"

#ifdef USE_XBEE

static Uint8 getNextFrameID();

static volatile Uint8 frameID = 1;

static Uint8 getNextFrameID(){

	Uint8 ret = frameID;

	if(frameID >= 255)
		frameID = 1;
	else
		frameID++;

	return ret;
}

networkSessionController_t transmitRequest(void * Observer, networkSessionTransmitRequestObserver_ptr Notify, Uint8 Data[], Uint8 NbData, bool_e Ack, Uint64 DestinationAddress64bit, Uint16 DestinationAddress16bit){
	assert(Data != NULL);
	assert(NbData < XBEE_MAX_DATA_REQUEST);

	Uint8 frameID = getNextFrameID();

	networkMessageTransmit_t msg;

	msg.header.formated.frameType = FRAME_TYPE_TRANSMIT_REQUEST;
	msg.header.formated.lenght = NbData + TRANSMIT_REQUEST_SIZE;

	msg.frame.transmitRequest.frameID = frameID;
	msg.frame.transmitRequest.destinationAddress64bit = DestinationAddress64bit;
	msg.frame.transmitRequest.destinationAddress16bit = DestinationAddress16bit;
	msg.frame.transmitRequest.broadcastRadius = BROADCAST_RADIUS_DEFAULT_VALUE;
	msg.frame.transmitRequest.options = 0;

	Uint8 i;
	for(i=0; i < NbData; i++)
		msg.frame.transmitRequest.dataRF[i] = Data[i];

	networkSessionController_t controller;

	networkSession_s session = transmitRequestSession_create(Observer, Notify, msg, Ack, 1000);

	networkSession_s *session_ptr = NETWORK_SESSION_MNG_openSession(&session);
	if(session_ptr != NULL){
		controller.abord = session_ptr->abord;
		controller.param = session_ptr;
	}else{
		Notify(Observer, FALSE);
	}

	return controller;
}


#endif
