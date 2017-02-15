/*
 * hokuyoMsgType.h
 *
 *  Created on: 15 févr. 2017
 *      Author: guill
 */

#ifndef QS_HOKUYO_HOKUYOMSGTYPE_H_
#define QS_HOKUYO_HOKUYOMSGTYPE_H_

#define COMMAND_SYMBOL_SIZE		2
#define PARAMETERS_MAX_SIZE		10
#define STRING_CHARACTERS_SIZE  18
#define SEND_COMMAND_SIZE		35

typedef enum{
	CMD_MD		= 0x4D44,
	CMD_MS		= 0x4D53,
	CMD_GD		= 0x4744,
	CMD_GS		= 0x4753,
	CMD_BM		= 0x424D,
	CMD_QT		= 0x5154,
	CMD_RS		= 0x5253,
	CMD_TM		= 0x544D,
	CMD_HS		= 0x4853,
	CMD_VV		= 0x5656,
	CMD_PP		= 0x5050,
	CMD_II		= 0x4949
}commandSymbol_e;

typedef enum{
	CONTROL_CODE_ADJUST_MODE_ON		= 0x00,
	CONTROL_CODE_TIME_REQUEST		= 0x01,
	CONTROL_CODE_ADJUST_MODE_OFF	= 0x02
}controlCode_e;

typedef enum{
	SENSITIVITY_NORMAL				= 0x00,
	SENSITIVITY_HIGH				= 0x01
}sensitivity_e;

typedef struct {

	Uint16 length;
	Uint8 cmdStr[SEND_COMMAND_SIZE];

	union {

		Uint8 rawData[COMMAND_SYMBOL_SIZE + PARAMETERS_MAX_SIZE + STRING_CHARACTERS_SIZE];

		struct{

			commandSymbol_e commandSymbol;

			union{

				#define STARTING_STEP_SIZE	4
				#define END_STEP_SIZE		4
				#define CLUSTER_COUNT_SIZE	2
				#define SCAN_INTERVAL_SIZE	1
				#define NUMBER_OF_SCANS		2

				struct{
					Uint16 startingStep;
					Uint16 endStep;
					Uint8 clusterCount;
					Uint8 scanInterval;
					Uint8 numberOfScans;
				}MD_MS;

				struct{
					Uint8 startingStep[STARTING_STEP_SIZE];
					Uint8 endStep[END_STEP_SIZE];
					Uint8 clusterCount[CLUSTER_COUNT_SIZE];
				}GD_GS;

				struct{
					controlCode_e controlCode;
				}TM;

				struct{
					sensitivity_e sensitivity;
				}HS;

			}parameters;

			Uint8 stringCharacters[STRING_CHARACTERS_SIZE];

		}formated;

	}command;

}hokuyoSendCommand_s;

#endif /* QS_HOKUYO_HOKUYOMSGTYPE_H_ */
