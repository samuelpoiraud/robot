#include "networkTransmitter.h"
#include "../../../QS_uart.h"
#include "../../../QS_outputlog.h"

#ifdef USE_XBEE

	#define XBee_putc_checksum(x)	{XBee_putc(x); checksum+=x;}

	static void NETWORK_TRANSMITTER_convertEndian(networkMessageTransmit_t * Msg);
	static void NETWORK_TRANSMITTER_correctBitFieldAlignment(networkMessageTransmit_t * Msg);

	void NETWORK_TRANSMITTER_send(networkMessageTransmit_t *NetworkMessage){
		Uint8 checksum = 0;
		Uint8 i;

		info_printf("O - 0x%2x\n", NetworkMessage->header.formated.frameType);

		Uint16 nbData = NetworkMessage->header.formated.lenght - 1;

		NETWORK_TRANSMITTER_convertEndian(NetworkMessage);
		NETWORK_TRANSMITTER_correctBitFieldAlignment(NetworkMessage);

		XBee_putc(XBEE_START_DELIMITER);
		XBee_putc(NetworkMessage->header.rawData[0]);		// Size MSB
		XBee_putc(NetworkMessage->header.rawData[1]);		// Size LSB
		XBee_putc_checksum(NetworkMessage->header.rawData[2]);

		for(i=0; i < nbData; i++){
			XBee_putc_checksum(NetworkMessage->frame.rawData[i]);
		}

		XBee_putc(XBEE_CHECKSUM_BASE_VALUE - checksum);
	}

	static void NETWORK_TRANSMITTER_correctBitFieldAlignment(networkMessageTransmit_t * Msg){
		networkMessageTransmit_t temp = *Msg;
		Uint8 i;
		switch(Msg->header.formated.frameType){
			case FRAME_TYPE_TRANSMIT_REQUEST:
				Msg->frame.__transmitRequest.frameID = temp.frame.transmitRequest.frameID;
				Msg->frame.__transmitRequest.destinationAddress64bit_7 = (temp.frame.transmitRequest.destinationAddress64bit & 0xFF00000000000000ULL) >> 56;
				Msg->frame.__transmitRequest.destinationAddress64bit_6 = (temp.frame.transmitRequest.destinationAddress64bit & 0x00FF000000000000ULL) >> 48;
				Msg->frame.__transmitRequest.destinationAddress64bit_5 = (temp.frame.transmitRequest.destinationAddress64bit & 0x0000FF0000000000ULL) >> 40;
				Msg->frame.__transmitRequest.destinationAddress64bit_4 = (temp.frame.transmitRequest.destinationAddress64bit & 0x000000FF00000000ULL) >> 32;
				Msg->frame.__transmitRequest.destinationAddress64bit_3 = (temp.frame.transmitRequest.destinationAddress64bit & 0x00000000FF000000ULL) >> 24;
				Msg->frame.__transmitRequest.destinationAddress64bit_2 = (temp.frame.transmitRequest.destinationAddress64bit & 0x0000000000FF0000ULL) >> 16;
				Msg->frame.__transmitRequest.destinationAddress64bit_1 = (temp.frame.transmitRequest.destinationAddress64bit & 0x000000000000FF00ULL) >> 8;
				Msg->frame.__transmitRequest.destinationAddress64bit_0 = (temp.frame.transmitRequest.destinationAddress64bit & 0x00000000000000FFULL) >> 0;
				Msg->frame.__transmitRequest.destinationAddress16bit_1 = (temp.frame.transmitRequest.destinationAddress16bit & 0xFF00) >> 8;
				Msg->frame.__transmitRequest.destinationAddress16bit_0 = (temp.frame.transmitRequest.destinationAddress16bit & 0x00FF) >> 0;
				Msg->frame.__transmitRequest.broadcastRadius = temp.frame.transmitRequest.broadcastRadius;
				Msg->frame.__transmitRequest.options = temp.frame.transmitRequest.options;
				for(i=0; i < __REV16(Msg->header.formated.lenght) - TRANSMIT_REQUEST_SIZE; i++)
					Msg->frame.__transmitRequest.dataRF[i] = temp.frame.transmitRequest.dataRF[i];
				break;

			default:
				break;
		}

	}

	static void NETWORK_TRANSMITTER_convertEndian(networkMessageTransmit_t * Msg){

		Msg->header.formated.lenght = __REV16(Msg->header.formated.lenght);

		switch(Msg->header.formated.frameType){
			case FRAME_TYPE_AT_COMMAND:
				break;

			case FRAME_TYPE_AT_COMMAND_QUEUE_REGISTER_VALUE:
				break;

			case FRAME_TYPE_CREATE_SOURCE_ROUTE:
				Msg->frame.createSourceRoute.addresses = __REV16(Msg->frame.createSourceRoute.addresses);
				Msg->frame.createSourceRoute.destinationAddress16bit = __REV16(Msg->frame.createSourceRoute.destinationAddress16bit);
				Msg->frame.createSourceRoute.destinationAddress64bit = __REV64(Msg->frame.createSourceRoute.destinationAddress64bit);
				break;

			case FRAME_TYPE_EXPLICIT_ADDRESSING_COMMAND_FRAME:
				Msg->frame.explicitAddressingCommandFrame.clusterID = __REV16(Msg->frame.explicitAddressingCommandFrame.clusterID);
				Msg->frame.explicitAddressingCommandFrame.destinationAddress16bit = __REV16(Msg->frame.explicitAddressingCommandFrame.destinationAddress16bit);
				Msg->frame.explicitAddressingCommandFrame.destinationAddress64bit = __REV64(Msg->frame.explicitAddressingCommandFrame.destinationAddress64bit);
				Msg->frame.explicitAddressingCommandFrame.profileId = __REV16(Msg->frame.explicitAddressingCommandFrame.profileId);
				break;

			case FRAME_TYPE_REGISTER_JOINING_DEVICE:
				Msg->frame.registerJoiningDevice.destinationAddress16bit = __REV16(Msg->frame.registerJoiningDevice.destinationAddress16bit);
				Msg->frame.registerJoiningDevice.destinationAddress64bit = __REV64(Msg->frame.registerJoiningDevice.destinationAddress64bit);
				break;

			case FRAME_TYPE_REMOTE_AT_COMMAND:
				Msg->frame.remoteATCommand.destinationAddress16bit = __REV16(Msg->frame.remoteATCommand.destinationAddress16bit);
				Msg->frame.remoteATCommand.destinationAddress64bit = __REV64(Msg->frame.remoteATCommand.destinationAddress64bit);
				break;

			case FRAME_TYPE_TRANSMIT_REQUEST:
				Msg->frame.transmitRequest.destinationAddress16bit = __REV16(Msg->frame.transmitRequest.destinationAddress16bit);
				Msg->frame.transmitRequest.destinationAddress64bit = __REV64(Msg->frame.transmitRequest.destinationAddress64bit);
				break;

			default:
				break;
		}

	}

#endif
