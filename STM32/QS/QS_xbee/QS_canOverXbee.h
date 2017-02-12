#include "../QS_all.h"

#ifndef QS_CAN_OVER_XBEE_H
	#define	QS_CAN_OVER_XBEE_H

	#ifdef USE_XBEE

		#include "communication/comType.h"
		#include "xbeeConfig.h"

		void CAN_OVER_XBEE_init();

		void CAN_OVER_XBEE_processMain(void);

		Uint64 CAN_OVER_XBEE_getDestinationAddress64bit();

		robot_id_e CAN_OVER_XBEE_getRobotByDestinationAddress64bit(Uint64 destinationAddress64bit);

		void CAN_OVER_XBEE_sendCANMsg(Uint8 nbMsg, CAN_msg_t * msg);

	#endif

#endif /* ifndef QS_CAN_OVER_XBEE_H */
