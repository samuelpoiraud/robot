/*
 * cmd_MS.c
 *
 *  Created on: 15 févr. 2017
 *      Author: guill
 */


#include "cmd_MS.h"
#include "../QS_hokuyo.h"
#include <string.h>
#include "../../QS/QS_outputlog.h"
#include "../usb_vcp_cdc.h"

#define NB_BYTES_FROM_HOKUYO_CMD_MS		2500
#define ECHO_MS_SIZE_MIN				21

static Uint8 CMD_MS_datas[NB_BYTES_FROM_HOKUYO_CMD_MS];			//Données brutes issues du capteur HOKUYO
static Uint32 CMD_MS_index = 0;									//Index pour ces données

static commandEchoStatus_e CMD_MS_readBufferEcho(hokuyoSendCommand_s *cmd);
static void CMD_MS_generateString(hokuyoSendCommand_s *cmd);
static void CMD_MS_setNumberOfScans(hokuyoSendCommand_s *cmd, Uint8 numberOfScans);

hokuyoSendCommand_s CMD_MS_create(Uint16 startingStep,
		                          Uint16 endStep,
								  Uint8 clusterCount,
								  Uint8 scanInterval,
								  Uint8 numberOfScans,
								  char *stringCharacters)
{

	hokuyoSendCommand_s cmd;

	cmd.command.formated.commandSymbol = CMD_MS;
	cmd.command.formated.parameters.MD_MS.startingStep = startingStep;
	cmd.command.formated.parameters.MD_MS.endStep = endStep;
	cmd.command.formated.parameters.MD_MS.clusterCount = clusterCount;
	cmd.command.formated.parameters.MD_MS.scanInterval = scanInterval;
	cmd.command.formated.parameters.MD_MS.numberOfScans = numberOfScans;
	strcpy((char*)cmd.command.formated.stringCharacters, ";");
	strncpy((char*)cmd.command.formated.stringCharacters + 1, stringCharacters, STRING_CHARACTERS_SIZE - 1);

	CMD_MS_generateString(&cmd);

	debug_printf("MS command generated : %s\n", cmd.cmdStr);

	return cmd;
}

HOKUYO_commandStatus_e CMD_MS_sendCommand(hokuyoSendCommand_s *cmd) {

	typedef enum
	{
		INIT = 0,
		SEND_COMMAND,
		WAIT_ECHO,
		READ_DATA,
		COMPUTE_DATA,
		DETECT_ROBOT,
		NOTIFY,
		ERROR,
		TIMEOUT,
		DONE
	}state_e;

	static state_e state = INIT, last_state = INIT;
	bool_e entrance;
	static time32_t buffer_read_time_begin = 0;

	if((state == INIT && last_state == INIT) || state != last_state)
		entrance = TRUE;
	else
		entrance = FALSE;

	last_state = state;

	switch(state) {

		case INIT:
			state = SEND_COMMAND;
			break;

		case SEND_COMMAND:
			HOKUYO_writeCommand(cmd->cmdStr);
			state = WAIT_ECHO;
			break;

		case WAIT_ECHO:

			switch(CMD_MS_readBufferEcho(cmd)) {
				case CMD_ECHO_DONE:
					state = READ_DATA;
					break;
				case CMD_ECHO_TIMEOUT:
					state = TIMEOUT;
					break;
				case CMD_ECHO_ERROR:
					state = ERROR;
					break;
				case CMD_ECHO_IN_PROGRESS:
				default:
					break;
			}

			break;

		case READ_DATA:
			return HOKUYO_CMD_DONE;
			break;

		case TIMEOUT:
			return HOKUYO_CMD_TIMEOUT;
			break;

		case ERROR:
			return HOKUYO_CMD_ERROR;
			break;

		default:
			break;

	}

	return HOKUYO_CMD_IN_PROGRESS;

}

static commandEchoStatus_e CMD_MS_readBufferEcho(hokuyoSendCommand_s *cmd) {

	typedef enum {
		CMD_ACK,
		STATUS_ACK,
		CMD_ECHO,
		STATUS_ECHO,
		TIME_STAMP,
		ERROR
	} state_e;


	static Uint8 previous_data = 0;
	static Uint8 pre_previous_data = 0;
	static Uint8 str[STRING_CHARACTERS_SIZE] = "";
	static Uint8 index = 0;
	static Uint8 checksum = 0;
	static time32_t buffer_read_time_begin = 0;
	static bool_e isInit = FALSE;
	Uint8 data;

	while(!VCP_isRxEmpty()) {

		static state_e state = CMD_ACK, last_state = ERROR;
		bool_e entrance;

		if((state == CMD_ACK && last_state == CMD_ACK) || state != last_state)
			entrance = TRUE;
		else
			entrance = FALSE;

		last_state = state;

		data = VCP_read();

		CMD_MS_datas[CMD_MS_index++] = data;

		switch(state) {

			case CMD_ACK:
				if(isInit == FALSE) {
					buffer_read_time_begin = global.absolute_time;
					index = 0;
					isInit = TRUE;
				}
				if(data == LINE_FEED) {
					state = STATUS_ACK;
					isInit = FALSE;
				} else {
					if(data != cmd->cmdStr[index++]) {
						error_printf("Hokuyo Erreur Acknowledge : command");
						state = ERROR;
						isInit = FALSE;
					}
				}
				break;

			case STATUS_ACK:
				if(entrance) {
					index = 0;
					checksum = 0;
				}
				if(index < 2) {
					str[index++] = data;
					checksum += data;
				} else {
					if(data == LINE_FEED && previous_data == LINE_FEED) {
						if(pre_previous_data == HOKUYO_convertChecksum(checksum)){
							/* On regarde si le statut indique une erreur (not "00") */
							if(str[0] == '0' && str [1] == '0')
							{
								/* On diminue le nombre de scan */
								CMD_MS_setNumberOfScans(cmd, 0);
								state = CMD_ECHO;
							} else {
								error_printf("Hokuyo Erreur Acknowledge : %c%c\n", str[0], str[1]);
								state = ERROR;
							}
						} else {
							error_printf("Hokuyo Erreur Acknowledge : checksum");
						}

					}
				}
				break;

			case CMD_ECHO:
				if(entrance) {
					index = 0;
				}
				if(data == LINE_FEED) {
					state = STATUS_ECHO;
					isInit = FALSE;
				} else {
					if(data != cmd->cmdStr[index++]) {
						error_printf("Hokuyo Erreur Echo : command");
						state = ERROR;
					}
				}
				break;

			case STATUS_ECHO:
				if(entrance) {
					index = 0;
					checksum = 0;
				}
				if(index < 2) {
					str[index++] = data;
					checksum += data;
				} else {
					if(data == LINE_FEED) {
						if(previous_data == HOKUYO_convertChecksum(checksum)){
							/* On regarde si le statut indique une erreur (not "99") */
							if(str[0] == '9' && str [1] == '9')
							{
								state = TIME_STAMP;
							} else {
								error_printf("Hokuyo Erreur Echo : %c%c\n", str[0], str[1]);
								state = ERROR;
							}
						} else {
							error_printf("Hokuyo Erreur Echo : checksum");
						}

					}
				}
				break;

			case TIME_STAMP:
				if(entrance) {
					index = 0;
					checksum = 0;
				}
				if(index < 4) {
					str[index++] = data;
					checksum += data;
				} else {
					if(data == LINE_FEED) {
						if(previous_data != HOKUYO_convertChecksum(checksum)) {
							error_printf("Hokuyo Erreur : checksum timeStamp\n");
						}
						return CMD_ECHO_DONE;
					}
				}
				break;

			case ERROR:
				if(data == LINE_FEED && previous_data == LINE_FEED) {
					return CMD_ECHO_ERROR;
				}
				break;

		}

		if(global.absolute_time - buffer_read_time_begin > HOKUYO_ECHO_READ_TIMEOUT) {
			error_printf("Hokuyo Erreur : TimeOut echo\n");
			return CMD_ECHO_TIMEOUT;
		}

		pre_previous_data = previous_data;
		previous_data = data;

	}

	return CMD_ECHO_IN_PROGRESS;

}


static bool_e CMD_MS_readBuffer(bool_e isEcho) {

	static Uint8 previous_data = 0;
	Uint8 data;

	while(!VCP_isRxEmpty()) {
		data = VCP_read();
		CMD_MS_datas[CMD_MS_index] = data;
		debug_printf("%c", data);
		if(CMD_MS_index < NB_BYTES_FROM_HOKUYO_CMD_MS) {
			CMD_MS_index++;
		} else {
			fatal_printf("Overflow in hokuyo_read_buffer !\n");
		}

		/* On interrompt la réception des octets lorsque l'on reçoit deux LF consécutifs */
		if(data == LINE_FEED && previous_data == LINE_FEED) {
			return TRUE;
		}

		previous_data = data;
	}
	return FALSE;
}

static void CMD_MS_generateString(hokuyoSendCommand_s *cmd) {
	Uint8 index = 0;
	strcpy((char*)(cmd->cmdStr + index), "MS");
	index += COMMAND_SYMBOL_SIZE;
	sprintf((char*)(cmd->cmdStr + index), "%04d", cmd->command.formated.parameters.MD_MS.startingStep);
	index += STARTING_STEP_SIZE;
	sprintf((char*)(cmd->cmdStr + index), "%04d", cmd->command.formated.parameters.MD_MS.endStep);
	index += END_STEP_SIZE;
	sprintf((char*)(cmd->cmdStr + index), "%02d", cmd->command.formated.parameters.MD_MS.clusterCount);
	index += CLUSTER_COUNT_SIZE;
	sprintf((char*)(cmd->cmdStr + index), "%01d", cmd->command.formated.parameters.MD_MS.scanInterval);
	index += SCAN_INTERVAL_SIZE;
	sprintf((char*)(cmd->cmdStr + index), "%02d", cmd->command.formated.parameters.MD_MS.numberOfScans);
	index += NUMBER_OF_SCANS;
	strcpy((char*)(cmd->cmdStr + index), (char*)cmd->command.formated.stringCharacters);
}


static void CMD_MS_setNumberOfScans(hokuyoSendCommand_s *cmd, Uint8 numberOfScans) {
	cmd->command.formated.parameters.MD_MS.numberOfScans = numberOfScans;
	CMD_MS_generateString(cmd);
}
