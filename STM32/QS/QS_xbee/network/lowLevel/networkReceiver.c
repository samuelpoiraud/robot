#include "networkReceiver.h"
#include "../../../QS_uart.h"
#include "../../../QS_outputlog.h"
#include "networkSpreader.h"

#ifdef USE_XBEE

	#define XBEE_RECEIVED_BUFFER_SIZE		10

	typedef enum{
		XBEE_PARSER_DELIMITER,
		XBEE_PARSER_LENGHT,
		XBEE_PARSER_FRAME_TYPE,
		XBEE_PARSER_FRAME,
		XBEE_PARSER_CHECKSUM
	}xbeeAPIParserState_e;

	typedef struct{
		networkMessageReceive_t buffer[XBEE_RECEIVED_BUFFER_SIZE];
		Uint8 readIndex;
		Uint8 writeIndex;
	}XBEE_FIFO_ReceivedFrame_t;

	static void NETWORK_RECEIVER_convertEndian(networkMessageReceive_t *Msg);
	static void NETWORK_RECEIVER_APIParser(char ByteRead);

	static void XBEE_FIFO_init();
	static bool_e XBEE_FIFO_isFull();
	static bool_e XBEE_FIFO_isEmpty();
	static bool_e XBEE_FIFO_insertMsg(networkMessageReceive_t * Data);
	static void XBEE_FIFO_incWriteIndex();
	static void XBEE_FIFO_incReadIndex();


	static volatile XBEE_FIFO_ReceivedFrame_t XBEE_FIFO_ReceivedFrame;

	void NETWORK_RECEIVER_init(){
		XBEE_FIFO_init();
		NETWORK_SPREADER_networkListen((UART_dataReceivedFunctionPtr)&NETWORK_RECEIVER_APIParser);
	}

	bool_e NETWORK_RECEIVER_msgReady(){
		return !XBEE_FIFO_isEmpty();
	}

	bool_e NETWORK_RECEIVER_getMsg(networkMessageReceive_t * Data){
		if(XBEE_FIFO_isEmpty())
			return FALSE;

		Uint8 i;

		for(i=0; i < XBEE_HEADER_SIZE; i++)
			Data->header.rawData[i] = XBEE_FIFO_ReceivedFrame.buffer[XBEE_FIFO_ReceivedFrame.readIndex].header.rawData[i];

		for(i=0; i < Data->header.formated.lenght - 1; i++)
			Data->frame.rawData[i] = XBEE_FIFO_ReceivedFrame.buffer[XBEE_FIFO_ReceivedFrame.readIndex].frame.rawData[i];

		XBEE_FIFO_incReadIndex();

		return TRUE;
	}

	static void NETWORK_RECEIVER_APIParser(char ByteRead){

		static xbeeAPIParserState_e state = XBEE_PARSER_DELIMITER;
		static xbeeAPIParserState_e last_state = XBEE_PARSER_DELIMITER;

		static networkMessageReceive_t xbeeAPIFrame;
		static Uint8 checksum;
		static bool_e xbeeFrameOverflow;

		static Uint16 byteToRead;
		static Uint16 byteCounter;


		bool_e entrance = last_state != state;
		last_state = state;

		switch (state)
		{
			case XBEE_PARSER_DELIMITER:

				checksum = 0;
				xbeeFrameOverflow = FALSE;

				if(ByteRead != XBEE_START_DELIMITER)
					error_printf("Invalid start 0x%02x != 0x7E\n", ByteRead);
				else{
					state = XBEE_PARSER_LENGHT;
				}
				break;

			case XBEE_PARSER_LENGHT:

				if(entrance){
					byteToRead = 2;
					byteCounter = 0;
				}

				if(byteCounter < XBEE_HEADER_SIZE)
					xbeeAPIFrame.header.rawData[byteCounter++] = ByteRead;

				byteToRead--;

				if(byteToRead == 0){
					xbeeAPIFrame.header.formated.lenght = __REV16(xbeeAPIFrame.header.formated.lenght);

					if(xbeeAPIFrame.header.formated.lenght == 0)
						state = XBEE_PARSER_DELIMITER;
					else
						state = XBEE_PARSER_FRAME_TYPE;
				}
				break;

			case XBEE_PARSER_FRAME_TYPE:

				if(entrance){
					byteToRead = 1;
				}

				if(byteCounter < XBEE_HEADER_SIZE)
					xbeeAPIFrame.header.rawData[byteCounter++] = ByteRead;

				checksum += ByteRead;

				byteToRead--;

				if(byteToRead == 0){
					state = XBEE_PARSER_FRAME;
				}
				break;

			case XBEE_PARSER_FRAME:

				if(entrance){
					byteToRead = xbeeAPIFrame.header.formated.lenght - 1;
					byteCounter = 0;
				}

				if(byteCounter < XBEE_MAX_SIZE_FRAME)
					xbeeAPIFrame.frame.rawData[byteCounter++] = ByteRead;
				else
					xbeeFrameOverflow = TRUE;

				checksum += ByteRead;

				byteToRead--;

				if(byteToRead == 0){
					state = XBEE_PARSER_CHECKSUM;
				}
				break;

			case XBEE_PARSER_CHECKSUM:

				if(checksum + ByteRead != XBEE_CHECKSUM_BASE_VALUE){
					error_printf("Invalid checksum 0x%02x + 0x%02x != 0x%02x\n", checksum, ByteRead, XBEE_CHECKSUM_BASE_VALUE);
				}else if(xbeeFrameOverflow){
					error_printf("Invalid frame overflow > %d\n", XBEE_MAX_SIZE_FRAME);
				}else if(XBEE_FIFO_insertMsg(&xbeeAPIFrame) == FALSE){
					error_printf("Fifo full\n");
				}

				state = XBEE_PARSER_DELIMITER;
				break;

			default:
				error_printf("ERROR : XBEE Low Level parser default case !\n");
				state = XBEE_PARSER_DELIMITER;
				break;
		}
	}

	static void NETWORK_RECEIVER_convertEndian(networkMessageReceive_t *Msg){
		switch(Msg->header.formated.frameType){
			case FRAME_TYPE_AT_COMMAND_RESPONSE:
				break;

			case FRAME_TYPE_EXPLICIT_RX_INDICATOR:
				Msg->frame.explicitRxIndicator.clusterID = __REV16(Msg->frame.explicitRxIndicator.clusterID);
				Msg->frame.explicitRxIndicator.destinationAddress16bit = __REV16(Msg->frame.explicitRxIndicator.destinationAddress16bit);
				Msg->frame.explicitRxIndicator.destinationAddress64bit = __REV64(Msg->frame.explicitRxIndicator.destinationAddress64bit);
				Msg->frame.explicitRxIndicator.profileId = __REV16(Msg->frame.explicitRxIndicator.profileId);
				break;

			case FRAME_TYPE_EXTENDED_MODEM_STATUS:
				break;

			case FRAME_TYPE_IO_DATA_SAMPLE_RX_INDICATOR:
				Msg->frame.IODataSampleRxIndicator.destinationAddress16bit = __REV16(Msg->frame.IODataSampleRxIndicator.destinationAddress16bit);
				Msg->frame.IODataSampleRxIndicator.destinationAddress64bit = __REV64(Msg->frame.IODataSampleRxIndicator.destinationAddress64bit);
				Msg->frame.IODataSampleRxIndicator.digitalChannelMask = __REV16(Msg->frame.IODataSampleRxIndicator.digitalChannelMask);
				break;

			case FRAME_TYPE_JOIN_NOTIFICATION_STATUS:
				Msg->frame.joinNotificationStatus.newAddress16bit = __REV16(Msg->frame.joinNotificationStatus.newAddress16bit);
				Msg->frame.joinNotificationStatus.newAddress64bit = __REV64(Msg->frame.joinNotificationStatus.newAddress64bit);
				Msg->frame.joinNotificationStatus.parentAddress16bit = __REV16(Msg->frame.joinNotificationStatus.parentAddress16bit);
				break;

			case FRAME_TYPE_MANY_TO_ONE_ROUTE_REQUEST_INDICATOR:
				Msg->frame.manyToOneRouteRequestIndicator.destinationAddress16bit = __REV16(Msg->frame.manyToOneRouteRequestIndicator.destinationAddress16bit);
				Msg->frame.manyToOneRouteRequestIndicator.destinationAddress64bit = __REV64(Msg->frame.manyToOneRouteRequestIndicator.destinationAddress64bit);
				break;

			case FRAME_TYPE_MODEM_STATUS:
				break;

			case FRAME_TYPE_NODE_IDENTIFICATION_INDICATOR:
				Msg->frame.nodeIdentificationIndicator.destinationAddress16bit = __REV16(Msg->frame.nodeIdentificationIndicator.destinationAddress16bit);
				Msg->frame.nodeIdentificationIndicator.destinationAddress64bit = __REV64(Msg->frame.nodeIdentificationIndicator.destinationAddress64bit);
				Msg->frame.nodeIdentificationIndicator.deviceTypeIdentifier = __REV(Msg->frame.nodeIdentificationIndicator.deviceTypeIdentifier);
				Msg->frame.nodeIdentificationIndicator.digiProfileId = __REV16(Msg->frame.nodeIdentificationIndicator.digiProfileId);
				Msg->frame.nodeIdentificationIndicator.manufacturerId = __REV16(Msg->frame.nodeIdentificationIndicator.manufacturerId);
				Msg->frame.nodeIdentificationIndicator.parentAddress16bit = __REV16(Msg->frame.nodeIdentificationIndicator.parentAddress16bit);
				Msg->frame.nodeIdentificationIndicator.remoteAddress16bit = __REV16(Msg->frame.nodeIdentificationIndicator.remoteAddress16bit);
				Msg->frame.nodeIdentificationIndicator.remoteAddress64bit = __REV64(Msg->frame.nodeIdentificationIndicator.remoteAddress64bit);
				break;

			case FRAME_TYPE_OVER_THE_AIR_FIRMWARE_UPDATE_STATUS:
				Msg->frame.overTheAirFirmwareUpdateStatus.destinationAddress16bit = __REV16(Msg->frame.overTheAirFirmwareUpdateStatus.destinationAddress16bit);
				Msg->frame.overTheAirFirmwareUpdateStatus.destinationAddress64bit = __REV64(Msg->frame.overTheAirFirmwareUpdateStatus.destinationAddress64bit);
				Msg->frame.overTheAirFirmwareUpdateStatus.targetAddress64bit = __REV64(Msg->frame.overTheAirFirmwareUpdateStatus.targetAddress64bit);
				break;

			case FRAME_TYPE_RECEIVE_PACKET:
				Msg->frame.receivePacket.destinationAddress16bit = __REV16(Msg->frame.receivePacket.destinationAddress16bit);
				Msg->frame.receivePacket.destinationAddress64bit = __REV64(Msg->frame.receivePacket.destinationAddress64bit);
				break;

			case FRAME_TYPE_REMOTE_AT_COMMAND_RESPONSE:
				Msg->frame.remoteAtCommandResponse.destinationAddress16bit = __REV16(Msg->frame.remoteAtCommandResponse.destinationAddress16bit);
				Msg->frame.remoteAtCommandResponse.destinationAddress64bit = __REV64(Msg->frame.remoteAtCommandResponse.destinationAddress64bit);
				break;

			case FRAME_TYPE_ROUTE_RECORD_INDICATOR:
				Msg->frame.routeRecordIndicator.destinationAddress16bit = __REV16(Msg->frame.routeRecordIndicator.destinationAddress16bit);
				Msg->frame.routeRecordIndicator.destinationAddress64bit = __REV64(Msg->frame.routeRecordIndicator.destinationAddress64bit);
				Msg->frame.routeRecordIndicator.intermediateAddress16bit = __REV16(Msg->frame.routeRecordIndicator.intermediateAddress16bit);
				break;

			case FRAME_TYPE_TRANSMIT_STATUS:
				Msg->frame.transmitStatus.destinationAddress16bit = __REV16(Msg->frame.transmitStatus.destinationAddress16bit);
				break;

			case FRAME_TYPE_XBEE_SENSOR_READ_INDICATOR:
				Msg->frame.xbeeSensorReadIndicator.destinationAddress16bit = __REV16(Msg->frame.xbeeSensorReadIndicator.destinationAddress16bit);
				Msg->frame.xbeeSensorReadIndicator.destinationAddress64bit = __REV64(Msg->frame.xbeeSensorReadIndicator.destinationAddress64bit);
				Msg->frame.xbeeSensorReadIndicator.temperatureRead = __REV16(Msg->frame.xbeeSensorReadIndicator.temperatureRead);
				break;

			default:
				break;
		}
	}

	///-------------------------- FIFO function

	static void XBEE_FIFO_init(){
		XBEE_FIFO_ReceivedFrame.readIndex = 0;
		XBEE_FIFO_ReceivedFrame.writeIndex = 0;
	}

	static bool_e XBEE_FIFO_isFull(){
		Uint8 writeIndex = XBEE_FIFO_ReceivedFrame.writeIndex + 1;

		if(writeIndex >= XBEE_RECEIVED_BUFFER_SIZE)
			writeIndex = 0;

		return (writeIndex == XBEE_FIFO_ReceivedFrame.readIndex);
	}

	static bool_e XBEE_FIFO_isEmpty(){
		return (XBEE_FIFO_ReceivedFrame.readIndex == XBEE_FIFO_ReceivedFrame.writeIndex);
	}

	static bool_e XBEE_FIFO_insertMsg(networkMessageReceive_t *Data){
		Uint16 i;

		if(XBEE_FIFO_isFull())
			return FALSE;

		NETWORK_RECEIVER_convertEndian(Data);

		for(i=0; i < XBEE_HEADER_SIZE; i++)
			XBEE_FIFO_ReceivedFrame.buffer[XBEE_FIFO_ReceivedFrame.writeIndex].header.rawData[i] = Data->header.rawData[i];

		for(i=0; i < Data->header.formated.lenght - 1; i++)
			XBEE_FIFO_ReceivedFrame.buffer[XBEE_FIFO_ReceivedFrame.writeIndex].frame.rawData[i] = Data->frame.rawData[i];

		XBEE_FIFO_incWriteIndex();

		assert(!XBEE_FIFO_isFull());	// Présence d'une préemption

		return TRUE;
	}

	static void XBEE_FIFO_incWriteIndex(){
		Uint8 writeIndex = XBEE_FIFO_ReceivedFrame.writeIndex + 1;

		if(writeIndex >= XBEE_RECEIVED_BUFFER_SIZE)
			XBEE_FIFO_ReceivedFrame.writeIndex = 0;
		else
			XBEE_FIFO_ReceivedFrame.writeIndex = writeIndex;
	}

	static void XBEE_FIFO_incReadIndex(){
		__disable_irq();
		Uint8 readIndex = XBEE_FIFO_ReceivedFrame.readIndex + 1;

		if(readIndex >= XBEE_RECEIVED_BUFFER_SIZE)
			XBEE_FIFO_ReceivedFrame.readIndex = 0;
		else
			XBEE_FIFO_ReceivedFrame.readIndex = readIndex;
		__enable_irq();
	}

#endif
