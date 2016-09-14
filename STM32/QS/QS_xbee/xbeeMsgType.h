#ifndef XBEE_MSG_TYPE_H
	#define	XBEE_MSG_TYPE_H
	#include "../QS_all.h"

	#ifdef USE_XBEE

		#define XBEE_START_DELIMITER								0x7E
		#define XBEE_CHECKSUM_BASE_VALUE		0xFF

		#define XBEE_MAX_DATA_REQUEST 								64
		#define XBEE_MAX_AT_PARAM									64
		#define XBEE_MAX_SIZE_FRAME									128
		#define XBEE_HEADER_SIZE									3

		#define BROADCAST_RADIUS_DEFAULT_VALUE						10

		typedef enum{
			// SPECIAL
			WRITE 											= 0X5752,	// WR
			RESTORE_DEFAULTS 								= 0X5245,	// RE
			SOFTWAR_ERESET 									= 0X4652,	// FR
			NETWORK_RESET 									= 0X4E52,	// NR
			// ADDRESSING
			ADDRESS 										= 0X4D59,	// MY
			PARENT_ADDRESS									= 0X4D50,	// MP
			NUMBER_OF_REMAINING_CHILDREAN					= 0X4E43,	// NC
			SERIAL_NUMBERHIGH								= 0X5348,	// SH
			SERIAL_NUMBERLOW								= 0X534C,	// SL
			NODE_IDENTIFIER									= 0X4E49,	// NI
			DEVICE_TYPE_IDENTIFIER							= 0X4444,	// DD
			MAXIMUM_RFPAY_LOAD_BYTES						= 0X4E50,	// NP
			DESTINATION_ADDRESSLOW							= 0X444C,	// DL
			DESTINATION_ADDRESSHIGH							= 0X4448,	// DH
			MANY_TOONE_ROUTE_BROADCAST_TIME					= 0X4152,	// AR
			// NETWORKING
			OPERATING_CHANNEL								= 0X4348,	// CH
			EXTENDED_PANID									= 0X4944,	// ID
			OPERATING_EXTENDED_PANID						= 0X4F50,	// OP
			MAXIMUM_UNICAST_HOPS							= 0X4E48,	// NH
			BROADCAST_HOPS									= 0X4248,	// BH
			NODE_DISCOVER_TIMEOUT							= 0X4E54,	// NT
			OPERATING_PANID									= 0X4F49,	// OI
			NETWORK_DISCOVERY_OPTIONS						= 0X4E4F,	// NO
			NODE_DISCOVER									= 0X4E44,	// ND
			DESTINATION_NODE								= 0X444E,	// DN
			SCAN_CHANNELS									= 0X5343,	// SC
			SCAN_DURATION									= 0X5344,	// SD
			ZIGBEE_STACK_PROFILE							= 0X5A53,	// ZS
			NODE_JOIN_TIME									= 0X4E4A,	// NJ
			CHANNEL_VERIFICATION							= 0X4A56,	// JV
			JOIN_NOTIFICATION								= 0X4A4E,	// JN
			AGGREGATE_ROUTING_NOTIFICATION					= 0X4152,	// AR
			ASSOCIATION_INDICATION							= 0X4149,	// AI
			NODE_DISCOVERY_OPTIONS							= 0X4E4F,	// NO
			MAXIMUM_NUMBER_OF_API_TRANSMISSION_BYTES		= 0X4E50,	// NP
			PAN_CONFLICT_THRESHOLD							= 0X4352,	// CR
			// SECURITY
			ENCRYPTION_ENABLE								= 0X4545,	// EE
			ENCRYPTION_OPTIONS								= 0X454F,	// EO
			ENCRYPTION_KEY									= 0X4E4B,	// NK
			LINK_KEY										= 0X4B59,	// KY
			// RF INTERFACING
			POWER_LEVEL										= 0X504C,	// PL
			POWER_MODE										= 0X504D,	// PM
			POWER_PL4										= 0X5050,	// PP
			RECEIVED_SIGNAL_STRENGTH						= 0X4442,	// DB
			// SERIAL INTERFACING (I/O)
			API_ENABLE										= 0X4150,	// AP
			API_OPTIONS										= 0X414F,	// AO
			INTERFACE_DATA_RATE								= 0X4244,	// BD
			SERIAL_PARITY									= 0X4E42,	// NB
			PACKETIZATION_TIMEOUT							= 0X524F,	// RO
			DIO7_CONFIGURATION								= 0X4437,	// D7
			DIO6_CONFIGURATION								= 0X4436,	// D6
			// I/O COMMANDS
			FORCE_SAMPLE									= 0X4953,	// IS
			XBEE_SENSOR_SAMPLE								= 0X3153,	// 1S
			IO_SAMPLE_RATE									= 0X4952,	// IR
			IO_DIGITAL_CHANGE_DIRECTION						= 0X4943,	// IC
			PWM0_CONFIGURATION								= 0X4C30,	// P0
			DIO11_CONFIGURATION								= 0X4C31,	// P1
			DIO12_CONFIGURATION								= 0X4C32,	// P2
			DIO13_CONFIGURATION								= 0X4C33,	// P3
			ADIO0_CONFIGURATION								= 0X4430,	// D0
			ADIO1_CONFIGURATION								= 0X4431,	// D1
			ADIO2_CONFIGURATION								= 0X4432,	// D2
			ADIO3_CONFIGURATION								= 0X4433,	// D3
			DIO4_CONFIGURATION								= 0X4434,	// D4
			DIO5_CONFIGURATION								= 0X4435,	// D5
			ASSOCIATE_LED_BLINK_TIME						= 0X4C54,	// LT
			DIO8_CONFIGURATION								= 0X4438,	// D8
			PULL_UP_REGISTER								= 0X4C52,	// PR
			RSSI_PWM_TIMER									= 0X524C,	// RP
			COMMISSIONING_PUSH_BUTTON						= 0X4342,	// CB
			// DIAGNOSTICS
			FIRMWARE_VERSION								= 0X5652,	// VR
			HARDWARE_VERSION								= 0X4856,	// HV
			SUPPLY_VOLTAGE									= 0X2556,	// %V
			TEMPERATURE										= 0X5450,	// TP
			// SLEEP COMMANDS
			SLEEP_MODE										= 0X534D,	// SM
			NUMBER_OF_SLEEP_PERIODS							= 0X534E,	// SN
			SLEEP_PERIOD									= 0X5350,	// SP
			TIME_BEFORE_SLEEP								= 0X5354,	// ST
			SLEEP_OPTIONS									= 0X534F	// SO
		}atCommandType_e;

		typedef enum{
			AT_CMD_STATUS_OK									= 0x00,
			AT_CMD_STATUS_ERROR									= 0x01,
			AT_CMD_STATUS_INVALID_COMMAND						= 0x03,
			AT_CMD_STATUS_TX_FAILURE							= 0x04
		}atCommandStatus_e;

		typedef enum{
			MODEM_STATUS_HARDWARE_RESET							= 0x00,
			MODEM_STATUS_WATCHDOG_TIMER_RESET					= 0x01,
			MODEM_STATUS_JOINED_NETWORK							= 0x02,
			MODEM_STATUS_DIASSOCIATED							= 0x03,
			MODEM_STATUS_CONFIG_ERROR_SYNC_LOST					= 0x04,
			MODEM_STATUS_COORDINATOR_REALIGNEMENT				= 0x05,
			MODEM_STATUS_COORDINATOR_STARTED					= 0x06,
			MODEM_STATUS_NETWORK_SECURITY_KEY_UPDATED 			= 0x07,
			MODEM_STATUS_NETWORK_WOKE_UP						= 0x0B,
			MODEM_STATUS_NETWORK_WENT_TO_SLEEP					= 0x0C,
			MODEM_STATUS_VOLTAGE_SUPPLY_LIMIT_EXCEED 			= 0x0D,
			MODEM_STATUS_MODEM_CONFIG_CHANGED_WHILE_JOIN_IN_PROGRESS = 0x11,
			MODEM_STATUS_STACK_ERROR							= 0x80,
			MODEM_STATUS_SEND_JOIN_COMMAND_ISSUED_WITHOUT_CONNECTING_FROM_AP = 0x82,
			MODEM_STATUS_ACCESS_POINT_NOT_FOUND					= 0x83,
			MODEM_STATUS_PSK_NOT_CONFIGURED						= 0x84,
			MODEM_STATUS_SSID_NOT_FOUND							= 0x87,
			MODEM_STATUS_FAILED_TO_JOIN_WITH_SECURITY_ENABLED 	= 0x88,
			MODEM_STATUS_INVALID_CHANNEL						= 0x8A,
			MODEM_STATUS_FAILED_TO_JOIN_ACCESS_POINT			= 0x8E
		}modemStatus_e;

		typedef enum{
			FRAME_TYPE_AT_COMMAND								= 0x08,
			FRAME_TYPE_AT_COMMAND_QUEUE_REGISTER_VALUE			= 0x09,
			FRAME_TYPE_TRANSMIT_REQUEST							= 0x10,
			FRAME_TYPE_EXPLICIT_ADDRESSING_COMMAND_FRAME		= 0x11,
			FRAME_TYPE_REMOTE_AT_COMMAND						= 0x17,
			FRAME_TYPE_CREATE_SOURCE_ROUTE						= 0x21,
			FRAME_TYPE_REGISTER_JOINING_DEVICE					= 0x24,
			FRAME_TYPE_AT_COMMAND_RESPONSE						= 0x88,
			FRAME_TYPE_MODEM_STATUS								= 0x8A,
			FRAME_TYPE_TRANSMIT_STATUS							= 0x8B,
			FRAME_TYPE_RECEIVE_PACKET							= 0x90,
			FRAME_TYPE_EXPLICIT_RX_INDICATOR					= 0x91,
			FRAME_TYPE_IO_DATA_SAMPLE_RX_INDICATOR				= 0x92,
			FRAME_TYPE_XBEE_SENSOR_READ_INDICATOR				= 0x94,
			FRAME_TYPE_NODE_IDENTIFICATION_INDICATOR			= 0x95,
			FRAME_TYPE_REMOTE_AT_COMMAND_RESPONSE				= 0x97,
			FRAME_TYPE_EXTENDED_MODEM_STATUS					= 0x98,
			FRAME_TYPE_OVER_THE_AIR_FIRMWARE_UPDATE_STATUS		= 0xA0,
			FRAME_TYPE_ROUTE_RECORD_INDICATOR					= 0xA1,
			FRAME_TYPE_MANY_TO_ONE_ROUTE_REQUEST_INDICATOR		= 0xA3,
			FRAME_TYPE_JOIN_NOTIFICATION_STATUS					= 0xA5
		}frameType_e;

		typedef enum{
			DELIVERY_STATUS_SUCCESS								= 0x00,
			DELIVERY_STATUS_MAC_ACKNOWLEDGE_NEVER_OCCURED		= 0x01,
			DELIVERY_STATUS_CCA_FAILURE							= 0x02,
			DELIVERY_STATUS_PACKET_PURGE_WITHOUT_TRANSMISSION	= 0x03,
			DELIVERY_STATUS_PHYSICAL_ERROR_INTERFACE			= 0x04,
			DELIVERY_STATUS_NO_BUFFER							= 0x18,
			DELIVERY_STATUS_NETWORK_ACKNOWLEDGE_NEVER_OCCURED	= 0x21,
			DELIVERY_STATUS_NOT_JOINED_TO_NETWORK				= 0x22,
			DELIVERY_STATUS_SELF_ADDRESSED						= 0x23,
			DELIVERY_STATUS_ADDRESS_NOT_FOUND					= 0x24,
			DELIVERY_STATUS_ROUTE_NOT_FOUND						= 0x25,
			DELIVERY_STATUS_BROADCAST_RELAY_NOT_HEARD			= 0x26,
			DELIVERY_STATUS_INVALID_BINDING_TABLE_INDEX			= 0x2B,
			DELIVERY_STATUS_INVALID_ENDPOINT					= 0x2C,
			DELIVERY_STATUS_SOFTWARE_ERROR						= 0x31,
			DELIVERY_STATUS_RESSOURCE_ERROR						= 0x32,
			DELIVERY_STATUS_DATA_PAYLOAD_TOO_LARGE				= 0x74,
			DELIVERY_STATUS_CLIENT_SOCKET_CREATION_FAILED		= 0x76,
			DELIVERY_STATUS_KEY_NOT_AUTHORIZED					= 0xBB
		}deliveryStatus_e;

		typedef enum{
			DELIVERY_STATUS_NO_DISCOVERY_OVERHEAD				= 0x00,
			DELIVERY_STATUS_ADDRESS_DISCOVERY					= 0x01,
			DELIVERY_STATUS_ROUTE_DISCOVERY						= 0x02,
			DELIVERY_STATUS_ADDRESS_AND_ROUTE					= 0x03,
			DELIVERY_STATUS_EXTENDED_TIMEOUT_DISCOVERY			= 0x40
		}discoveryStatus_e;

		typedef enum{
			ONE_WIRE_SENSORS_A_D_SENSOR_READ					= 0x01,
			ONE_WIRE_SENSORS_TEMPERATURE_SENSOR_READ			= 0x02,
			ONE_WIRE_SENSORS_WATER_PRESENT						= 0x03
		}oneWireSensors_e;

		typedef enum{
			DEVICE_TYPE_COORDINATOR								= 0x00,
			DEVICE_TYPE_ROUTER									= 0x01,
			DEVICE_TYPE_END_DEVICE								= 0x02
		}deviceType_e;

		typedef enum{
			SOURCE_EVENT_NI_PUSHBUTTON_EVENT					= 0x01,
			SOURCE_EVENT_JOINING_EVENT							= 0x02,
			SOURCE_EVENT_POWER_CYCLE_EVENT						= 0x03
		}sourceEvent_e;

		typedef enum{
			STATUS_CODE_REJOIN									= 0x00,
			STATUS_CODE_STACK_STATUS							= 0x01,
			STATUS_CODE_JOINING									= 0x02,
			STATUS_CODE_NETWORK_FORMED__NETWORK_JOINED			= 0x03,
			STATUS_CODE_BEACON_REPONSE							= 0x04,
			STATUS_CODE_REJECT_ZS								= 0x05,
			STATUS_CODE_REJECT_ID								= 0x06,
			STATUS_CODE_REJECT_NJ								= 0x07,
			STATUS_CODE_PAN_ID_MATCH							= 0x08,
			STATUS_CODE_REJECT_LQIRSSI							= 0x09,
			STATUS_CODE_BEACON_SAVED							= 0x0A,
			STATUS_CODE_AI_VALUE_CHANGED						= 0x0B,
			STATUS_CODE_PERMIT_JOIN_DURATION_CHANGED			= 0x0C,
			STATUS_CODE_ACTIVE_SCANNING_HAS_BEGUN				= 0x0D,
			STATUS_CODE_ERROR_DURING_ACTIVE_SCAN				= 0x0E
		}statusCode_e;

		typedef enum{
			BOOTLOADER_MSG_TYPE_ACK								= 0x06,
			BOOTLOADER_MSG_TYPE_NACK							= 0x15,
			BOOTLOADER_MSG_TYPE_NO_MAC_ACK						= 0x40,
			BOOTLOADER_MSG_TYPE_QUERY							= 0x51,
			BOOTLOADER_MSG_TYPE_QUERY_RESPONSE					= 0x52
		}bootloaderMsgType_e;

		typedef enum{
			JOIN_STATUS_STANDARD_SECURITED_REJOIN				= 0x00,
			JOIN_STATUS_STANDARD_UNSECURITED_JOIN				= 0x01,
			JOIN_STATUS_DEVICE_LEFT								= 0x02,
			JOIN_STATUS_STANDARD_UNSECURITED_REJOIN				= 0x03,
			JOIN_STATUS_HIGH_SECURITED_REJOIN					= 0x04,
			JOIN_STATUS_HIGH_UNSECURITED_JOIN					= 0x05,
			JOIN_STATUS_HIGH_UNSECURITED_REJOIN					= 0x07
		}joinStatus_e;

		typedef union{

			Uint8 rawData[XBEE_MAX_SIZE_FRAME];

			struct{
				Uint8 frameID;
				char command[2];
				char parameter[XBEE_MAX_AT_PARAM];
			}atCommand;

			struct{
				Uint8 frameID;
				char command[2];
				char parameter[XBEE_MAX_AT_PARAM];
			}atCommandQueueRegisterValue;

			#define TRANSMIT_REQUEST_SIZE								14
			struct{
				Uint8 frameID;
				Uint64 destinationAddress64bit;
				Uint16 destinationAddress16bit;
				Uint8 broadcastRadius;
				Uint8 options;
				Uint8 dataRF[XBEE_MAX_DATA_REQUEST];
			}transmitRequest;

			struct{
				Uint8 frameID;
				Uint8 destinationAddress64bit_0;
				Uint8 destinationAddress64bit_1;
				Uint8 destinationAddress64bit_2;
				Uint8 destinationAddress64bit_3;
				Uint8 destinationAddress64bit_4;
				Uint8 destinationAddress64bit_5;
				Uint8 destinationAddress64bit_6;
				Uint8 destinationAddress64bit_7;
				Uint8 destinationAddress16bit_0;
				Uint8 destinationAddress16bit_1;
				Uint8 broadcastRadius;
				Uint8 options;
				Uint8 dataRF[XBEE_MAX_DATA_REQUEST];
			}__transmitRequest;

			struct{
				Uint8 frameID;
				Uint64 destinationAddress64bit;
				Uint16 destinationAddress16bit;
				Uint8 sourceEndPoint;
				Uint8 destinationEndPoint;
				Uint16 clusterID;
				Uint16 profileId;
				Uint8 broadcastRadius;
				Uint8 options;
				Uint8 dataPayload[XBEE_MAX_DATA_REQUEST];
			}explicitAddressingCommandFrame;

			struct{
				Uint8 frameID;
				Uint64 destinationAddress64bit;
				Uint16 destinationAddress16bit;
				Uint8 remoteCommandOptions;
				char command[2];
				char parameter[XBEE_MAX_AT_PARAM];
			}remoteATCommand;

			struct{
				Uint8 frameID;
				Uint64 destinationAddress64bit;
				Uint16 destinationAddress16bit;
				Uint8 routeCommandOptions;
				Uint8 numberOfAddresses;
				Uint16 addresses;
			}createSourceRoute;

			struct{
				Uint8 frameID;
				Uint64 destinationAddress64bit;
				Uint16 destinationAddress16bit;
				Uint8 options;
				Uint8 key;
			}registerJoiningDevice;

		}xbeeTransmitFrame_u;

		typedef union{

			Uint8 rawData[XBEE_MAX_SIZE_FRAME];

			struct{
				Uint8 frameID;
				char command[2];
				atCommandStatus_e commandStatus;
				char parameter[XBEE_MAX_AT_PARAM];
			}atCommandResponse;

			struct{
				modemStatus_e modemStatus;
			}modemStatus;

			struct{
				Uint8 frameID;
				Uint16 destinationAddress16bit;
				Uint8 transmitRetryCount;
				deliveryStatus_e deliveryStatus;
				discoveryStatus_e discoveryStatus;
			}transmitStatus;

			#define RECEIVE_PACKET_SIZE								12
			struct{
				Uint64 destinationAddress64bit;
				Uint16 destinationAddress16bit;
				Uint8 receiveOptions;
				Uint8 receivedData[XBEE_MAX_DATA_REQUEST];
			}receivePacket;

			struct{
				Uint64 destinationAddress64bit;
				Uint16 destinationAddress16bit;
				Uint8 sourceEndPoint;
				Uint8 destinationEndPoint;
				Uint16 clusterID;
				Uint16 profileId;
				Uint8 receiveOptions;
				Uint8 receivedData[XBEE_MAX_DATA_REQUEST];
			}explicitRxIndicator;

			struct{
				Uint64 destinationAddress64bit;
				Uint16 destinationAddress16bit;
				Uint8 receiveOptions;
				Uint8 numberOfSample;
				Uint16 digitalChannelMask;
				Uint8 analogChannelMask;
				Uint8 digitalSample;
				Uint8 analogSample;
			}IODataSampleRxIndicator;

			struct{
				Uint64 destinationAddress64bit;
				Uint16 destinationAddress16bit;
				Uint8 receiveOptions;
				oneWireSensors_e oneWireSensors;
				Uint8 analogDigitalValue[8];
				Uint16 temperatureRead;
			}xbeeSensorReadIndicator;

			struct{
				Uint64 destinationAddress64bit;
				Uint16 destinationAddress16bit;
				Uint8 receiveOptions;
				Uint16 remoteAddress16bit;
				Uint64 remoteAddress64bit;
				Uint8 niString[24];
				Uint16 parentAddress16bit;
				deviceType_e deviceType;
				sourceEvent_e sourceEvent;
				Uint16 digiProfileId;
				Uint16 manufacturerId;
				Uint32 deviceTypeIdentifier;
			}nodeIdentificationIndicator;

			struct{
				Uint8 frameID;
				Uint64 destinationAddress64bit;
				Uint16 destinationAddress16bit;
				char command[2];
				atCommandStatus_e commandStatus;
				Uint8 commandData[256];
			}remoteAtCommandResponse;

			struct{
				statusCode_e statusCode;
				Uint8 statusData[10];
			}extendedModemStatus;

			struct{
				Uint64 destinationAddress64bit;
				Uint16 destinationAddress16bit;
				Uint8 receiveOptions;
				bootloaderMsgType_e bootloaderMsgType;
				Uint8 blockNumber;
				Uint64 targetAddress64bit;
			}overTheAirFirmwareUpdateStatus;

			struct{
				Uint64 destinationAddress64bit;
				Uint16 destinationAddress16bit;
				Uint8 receiveOptions;
				Uint8 numberOfAddress;
				Uint16 intermediateAddress16bit;
			}routeRecordIndicator;

			struct{
				Uint64 destinationAddress64bit;
				Uint16 destinationAddress16bit;
				Uint8 reserved;
			}manyToOneRouteRequestIndicator;

			struct{
				Uint16 parentAddress16bit;
				Uint16 newAddress16bit;
				Uint64 newAddress64bit;
				joinStatus_e joinStatus;
			}joinNotificationStatus;

		}xbeeReceivedFrame_u;

		typedef union{
			Uint8 rawData[3];

			struct{
				Uint16 lenght;
				frameType_e frameType;
			}formated;

		}xbeeHeader_u;

		typedef struct{
			xbeeHeader_u header;
			xbeeReceivedFrame_u frame;
		}networkMessageReceive_t;

		typedef struct{
			xbeeHeader_u header;
			xbeeTransmitFrame_u frame;
		}networkMessageTransmit_t;

	#endif // def USE_XBEE

#endif /* ifndef XBEE_MSG_TYPE_H */
