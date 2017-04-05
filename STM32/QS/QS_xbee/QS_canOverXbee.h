#include "../QS_all.h"

#ifndef QS_CAN_OVER_XBEE_H
	#define	QS_CAN_OVER_XBEE_H

	#ifdef USE_XBEE

		#include "communication/comType.h"

		typedef void(*CAN_OVER_XBEE_callbackAction_t)(robot_id_e robotId, CAN_msg_t * can_msg);

		void CAN_OVER_XBEE_init();

		void CAN_OVER_XBEE_processMain(void);


		bool_e CAN_OVER_XBEE_InputMsgIsReady();

		bool_e CAN_OVER_XBEE_getInputMsg(robot_id_e * robotId, CAN_msg_t * msg);



		void CAN_OVER_XBEE_sendBroadcastCANMsg(CAN_msg_t * msg);

		void CAN_OVER_XBEE_sendCANMsgToModule(robot_id_e robotId, CAN_msg_t * msg);

		void CAN_OVER_XBEE_sendCANMsg(CAN_msg_t * msg);


		//Enregistre un pointeur sur fonction qui sera appelé à chaque message CAN envoyé.
		void CAN_OVER_XBEE_setSendCallback(CAN_OVER_XBEE_callbackAction_t action);

		bool_e CAN_OVER_XBEE_isValidAddress(Uint64 destinationAddress64bit);

		// Ne pas utilisé cette fonction
		void CAN_OVER_XBEE_addInputMsg(robot_id_e robotId, CAN_msg_t * msg);

	#endif

#endif /* ifndef QS_CAN_OVER_XBEE_H */
