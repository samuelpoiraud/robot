#include "QS_canOverXbee.h"

#ifdef USE_XBEE

	#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_COMMUNICATION
	#define LOG_PREFIX LOG_PREFIX_COMMUNICATION
	#include "../QS_outputlog.h"
	#include "../QS_who_am_i.h"
	#include "communication/comReceiver.h"
	#include "communication/comSessionManager.h"
	#include "communication/session/requestSession/sendCANMsg.h"
	#include "communication/comSessionManager.h"
	#include "QS_xbee.h"
	#include "xbeeConfig.h"

	#define CAN_OVER_XBEE_SIZE_BUFFER	20

	typedef struct{
		CAN_msg_t msg;
		robot_id_e robotId;
	}inputMsg_s;

	static volatile const Uint64 address64bitByModule[NB_ROBOT_ID] = {
			0x0013A20040614966,		// BIG_ROBOT
			0x0013A200405DFB8D,		// SMALL_ROBOT
			0x0013A200405DF9FB		// BEACON_EYE
	};

	static volatile CAN_OVER_XBEE_callbackAction_t CAN_OVER_XBEE_sendCallback = NULL;

	static volatile inputMsg_s bufferInputMsg[CAN_OVER_XBEE_SIZE_BUFFER];
	static volatile Uint8 bufferReadIndex = 0;
	static volatile Uint8 bufferWriteIndex = 0;

	static Uint64 CAN_OVER_XBEE_getMyAddress64bit();
	static Uint64 CAN_OVER_XBEE_getOtherRobotAddress64bit();
	static robot_id_e CAN_OVER_XBEE_getRobotByDestinationAddress64bit(Uint64 destinationAddress64bit);
	static Uint64 CAN_OVER_XBEE_getDestinationAddress64bitByRobot(robot_id_e robotId);
	static void CAN_OVER_XBEE_sendCANMsgToAddress(Uint64 destinationAddress64bit, CAN_msg_t * msg);
	static void CAN_OVER_XBEE_InputMsgIncIndex(Uint8 * index);

	void CAN_OVER_XBEE_init(){

		XBEE_init();

		COM_RECEIVER_init();
		COM_SESSION_MNG_init();
	}

	void CAN_OVER_XBEE_processMain(void){
		XBEE_processMain();
		COM_SESSION_MNG_processMain();
	}

	bool_e CAN_OVER_XBEE_InputMsgIsReady(){
		return (bufferReadIndex != bufferWriteIndex);
	}

	bool_e CAN_OVER_XBEE_InputMsgIsFull(){
		Uint8 writeIndex = bufferWriteIndex;

		CAN_OVER_XBEE_InputMsgIncIndex(&writeIndex);

		return (bufferReadIndex == writeIndex);
	}

	bool_e CAN_OVER_XBEE_getInputMsg(robot_id_e * robotId, CAN_msg_t * msg){
		if(CAN_OVER_XBEE_InputMsgIsReady() == FALSE)
			return FALSE;

		*msg = bufferInputMsg[bufferReadIndex].msg;
		*robotId = bufferInputMsg[bufferReadIndex].robotId;

		CAN_OVER_XBEE_InputMsgIncIndex(&bufferReadIndex);

		return TRUE;
	}

	void CAN_OVER_XBEE_addInputMsg(robot_id_e robotId, CAN_msg_t * msg){
		if(CAN_OVER_XBEE_InputMsgIsFull()){
			error_printf("Buffer XBEE full message entrant détruit\n");
			return;
		}
		bufferInputMsg[bufferReadIndex].msg = *msg;
		bufferInputMsg[bufferReadIndex].robotId = robotId;

		CAN_OVER_XBEE_InputMsgIncIndex(&bufferWriteIndex);
	}

	void CAN_OVER_XBEE_sendBroadcastCANMsg(CAN_msg_t * msg){
		robot_id_e id;
		for(id=0; id<NB_ROBOT_ID; id++){
			CAN_OVER_XBEE_sendCANMsgToAddress(CAN_OVER_XBEE_getDestinationAddress64bitByRobot(id), msg);
		}
	}

	void CAN_OVER_XBEE_sendCANMsgToModule(robot_id_e robotId, CAN_msg_t * msg){
		CAN_OVER_XBEE_sendCANMsgToAddress(CAN_OVER_XBEE_getDestinationAddress64bitByRobot(robotId), msg);
	}

	void CAN_OVER_XBEE_sendCANMsg(CAN_msg_t * msg){
		CAN_OVER_XBEE_sendCANMsgToAddress(CAN_OVER_XBEE_getOtherRobotAddress64bit(), msg);
	}

	void CAN_OVER_XBEE_setSendCallback(CAN_OVER_XBEE_callbackAction_t action){
		CAN_OVER_XBEE_sendCallback = action;
	}


	/*** Internal Functions ***/

	static void CAN_OVER_XBEE_InputMsgIncIndex(Uint8 * index){
		if(*index >= CAN_OVER_XBEE_SIZE_BUFFER - 1)
			*index = 0;
		else
			(*index)++;
	}

	static void CAN_OVER_XBEE_sendCANMsgToAddress(Uint64 destinationAddress64bit, CAN_msg_t * msg){
		comSession_s * session;

		if(COM_SESSION_MNG_allocateSession(&session) == FALSE){
			error_printf("Envoi de message CAN impossible, allocation de session impossible\n");
			return;
		}

		sendCANMsg_create(session, COM_SESSION_DEFAULT_TIMEOUT, -1, destinationAddress64bit, 1, msg);

		COM_SESSION_MNG_openSession(session);

		if(CAN_OVER_XBEE_sendCallback != NULL){
			CAN_OVER_XBEE_sendCallback(CAN_OVER_XBEE_getRobotByDestinationAddress64bit(destinationAddress64bit), msg);
		}
	}

	static Uint64 CAN_OVER_XBEE_getMyAddress64bit(){
		robot_id_e robotId = QS_WHO_AM_I_get();
		assert(IS_ROBOT_ID(robotId));
		return address64bitByModule[robotId];
	}

	static Uint64 CAN_OVER_XBEE_getOtherRobotAddress64bit(){
		robot_id_e robotId = QS_WHO_AM_I_get();
		assert(IS_ROBOT_ID(robotId));
		robotId = (robotId == BIG_ROBOT)?SMALL_ROBOT:BIG_ROBOT;
		return address64bitByModule[robotId];
	}

	static robot_id_e CAN_OVER_XBEE_getRobotByDestinationAddress64bit(Uint64 destinationAddress64bit){
		robot_id_e i;
		for(i=0; i<NB_ROBOT_ID; i++){
			if(address64bitByModule[i] == destinationAddress64bit)
				return i;
		}
		return 0xFF;
	}

	static Uint64 CAN_OVER_XBEE_getDestinationAddress64bitByRobot(robot_id_e robotId){
		assert(IS_ROBOT_ID(robotId));
		return address64bitByModule[robotId];
	}

#endif
