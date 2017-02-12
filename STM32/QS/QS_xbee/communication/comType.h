
#include "../../QS_all.h"

#ifndef COM_TYPE_H_
	#define COM_TYPE_H_

	#ifdef USE_XBEE

		#include "../../QS_CANmsgList.h"

		#define COM_MSG_SIZE_HEADER			4

		#define COM_MSG_SIZE_MAX_BODY		32
		#define COM_MSG_SIZE_MAX_CAN_MSG	4

		#define COM_MSG_SIZE_MAX			(COM_MSG_SIZE_HEADER + COM_MSG_SIZE_MAX_BODY)

		#define COM_MSG_SIZE_CAN_ACK	1

		typedef enum{
			COM_MSG_TYPE_CAN_MSG
		}comMsgType_e;

		typedef enum{
			COM_MSG_WAY_SEND,
			COM_MSG_WAY_ACK
		}comMsgWay_e;

		typedef Uint8 comMsgId_t;

		typedef Uint8 comMsgSize_t;

		typedef union{
			Uint8 rawData[COM_MSG_SIZE_HEADER];

			struct{
				comMsgType_e type	:8;
				comMsgWay_e way		:8;
				comMsgId_t id		:8;
				comMsgSize_t size	:8;
			};
		}comMsgHeader_u;

		typedef union{
			Uint8 rawData[COM_MSG_SIZE_MAX_BODY];

			struct{
				CAN_msg_t msg[COM_MSG_SIZE_MAX_CAN_MSG];
			}canMsgSend;

			struct{
				bool_e dataReceived	:1;
			}canMsgAck;
		}comMsgBody_u;

		typedef struct{
			comMsgHeader_u header;
			comMsgBody_u body;
		}comMsg_t;

	#endif // def USE_XBEE

#endif /* COM_TYPE_H_ */
