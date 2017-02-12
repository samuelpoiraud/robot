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

	void CAN_OVER_XBEE_init(){

		XBEE_init();

		COM_RECEIVER_init();
		COM_SESSION_MNG_init();
	}

	Uint64 CAN_OVER_XBEE_getDestinationAddress64bit(){

		switch(QS_WHO_AM_I_get()){
			case BIG_ROBOT:
				return 0x0013A20040614966;

			case SMALL_ROBOT:
				return 0x0013A200405DFB8D;

			case BEACON_EYE:
				return 0x0013A200405DF9FB;

			default:
				error_printf("Robot actuel inconnu\n");
				return 0x00;
		}
	}

	robot_id_e CAN_OVER_XBEE_getRobotByDestinationAddress64bit(Uint64 destinationAddress64bit){
		switch(destinationAddress64bit){
			case 0x0013A20040614966:
				return BIG_ROBOT;

			case 0x0013A200405DFB8D:
				return SMALL_ROBOT;

			case 0x0013A200405DF9FB:
				return BEACON_EYE;

			default:
				error_printf("Message recut d'un inconnu\n");
				return 0xFF;
		}
	}

	void CAN_OVER_XBEE_processMain(void){
		XBEE_processMain();
		COM_SESSION_MNG_processMain();
	}

	void CAN_OVER_XBEE_sendCANMsg(Uint8 nbMsg, CAN_msg_t * msg){
		comSession_s session;

		sendCANMsg_create(&session, COM_SESSION_DEFAULT_TIMEOUT, -1, nbMsg, msg);

		COM_SESSION_MNG_openSession(&session);
	}

#endif
