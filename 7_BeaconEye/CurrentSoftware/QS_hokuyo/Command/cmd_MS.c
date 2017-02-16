/*
 * cmd_MS.c
 *
 *  Created on: 15 févr. 2017
 *      Author: guill
 */

//TODO : Vérifier si l'on enlève les 5 premiers degrés et derniers

#include "cmd_MS.h"
#include "../QS_hokuyo.h"
#include <string.h>
#include "../../QS/QS_outputlog.h"
#include "../usb_vcp_cdc.h"
#include "../../QS/QS_maths.h"
#include "../../environment.h"
#ifdef USE_BEACON_EYE
	#include "../IHM/terminal.h"
	#define Terminal_debug(x)	 TERMINAL_puts(x)
#else
	#define Terminal_debug(x)	 (void)0
#endif

#define NB_BYTES_FROM_HOKUYO_CMD_MS		2500
#define ECHO_MS_SIZE_MIN				21

static Uint8 CMD_MS_datas[NB_BYTES_FROM_HOKUYO_CMD_MS];			// Données brutes issues du capteur HOKUYO
static Uint32 CMD_MS_index = 0;									// Index pour ces données
static HOKUYO_adversary_position detectedPoints[NB_STEP_MAX];	// Points détectés par l'hokuyo
static Uint16 nbDetectedPoints = 0;

static commandEchoStatus_e CMD_MS_readBufferEcho(hokuyoSendCommand_s *cmd);
static commandEchoStatus_e CMD_MS_readBufferData(hokuyoSendCommand_s *cmd);
static void CMD_MS_generateString(hokuyoSendCommand_s *cmd);
static void CMD_MS_setNumberOfScans(hokuyoSendCommand_s *cmd, Uint8 numberOfScans);
static void CMD_MS_computeDataBlock2C(hokuyoSendCommand_s *cmd, Uint8 *dataBlock, Uint8 length, Sint16 angle);

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
			debug_printf("Commande envoyée\n");
			state = WAIT_ECHO;
			break;

		case WAIT_ECHO:

			switch(CMD_MS_readBufferEcho(cmd)) {
				case CMD_ECHO_DONE:
					debug_printf("Echo recu\n");
					state = READ_DATA;
					break;
				case CMD_ECHO_TIMEOUT:
					debug_printf("Timeout echo\n");
					Terminal_debug("ReadEcho : timeout");
					state = TIMEOUT;
					break;
				case CMD_ECHO_ERROR:
					debug_printf("Error echo\n");
					Terminal_debug("ReadEcho : error");
					state = ERROR;
					break;
				case CMD_ECHO_IN_PROGRESS:
				default:
					break;
			}

			break;

		case READ_DATA:

			switch(CMD_MS_readBufferData(cmd)) {
				case CMD_ECHO_DONE:{
					char strNb[255];
					sprintf(strNb, "Points detectes : %4d", nbDetectedPoints);
					Terminal_debug(strNb);
					state = DONE;
				}break;
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

		case TIMEOUT:
			state = INIT;
			return HOKUYO_CMD_TIMEOUT;
			break;

		case ERROR:
			state = INIT;
			return HOKUYO_CMD_ERROR;
			break;

		case DONE:
			state = INIT;
			return HOKUYO_CMD_DONE;
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

	static state_e state = CMD_ACK, last_state = CMD_ACK;
	bool_e entrance;

	while(!VCP_isRxEmpty()) {



		if((state == CMD_ACK && last_state == CMD_ACK) || state != last_state)
			entrance = TRUE;
		else
			entrance = FALSE;

		last_state = state;

		data = VCP_read();

		debug_printf("%c", data);

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
						state = CMD_ACK;
						return CMD_ECHO_DONE;
					}
				}
				break;

			case ERROR:
				if(data == LINE_FEED && previous_data == LINE_FEED) {
					state = CMD_ACK;
					return CMD_ECHO_ERROR;
				}
				break;

		}

		if(global.absolute_time - buffer_read_time_begin > HOKUYO_ECHO_READ_TIMEOUT) {
			error_printf("Hokuyo Erreur : TimeOut echo\n");
			state = CMD_ACK;
			isInit = FALSE;
			return CMD_ECHO_TIMEOUT;
		}

		pre_previous_data = previous_data;
		previous_data = data;

	}

	return CMD_ECHO_IN_PROGRESS;

}

static commandEchoStatus_e CMD_MS_readBufferData(hokuyoSendCommand_s *cmd) {

	static Sint32 resolution = ANGLE_RESOLUTION;
	static Sint32 blockSize = DATA_BLOCK_SIZE;
	Sint32 cluster = cmd->command.formated.parameters.MD_MS.clusterCount;

	static Uint8 previous_data = 0;						// Octet précédent
	static Uint8 checksum = 0;							// Checksum courant
	static Uint8 expectedChecksum = 0;					// Checksum attendu
	static time32_t buffer_read_time_begin = 0;			// Temps de début de traitement de la trame
	static bool_e isNewBloc = TRUE;						// Entrance pour le première état de la MAE
	static bool_e resetTimeout = TRUE;					// Indique la remise à zéro du timeout
	static Uint8 dataBlock[DATA_BLOCK_SIZE] = "";		// Block de données courant
	static Uint8 indexDataBlock = 0;					// Index pour le tableau de données
	static Sint32 angle = 0;							// Angle courant pour le premier point du bloc de donnée
	static bool_e isInTreatement = FALSE;				// En cours de traitement d'un bloc de données
	Uint8 data;											// Octet courant

	while(!VCP_isRxEmpty()) {
		data = VCP_read();

		/* Si nous avons un nouveau bloc de données */
		if(isNewBloc) {
			checksum = 0;
			indexDataBlock = 0;
			isNewBloc = FALSE;
		}

		if(data == LINE_FEED && previous_data == LINE_FEED) {
			/* Fin de la trame Hokuyo */
			isNewBloc = TRUE;
			return CMD_ECHO_DONE;

		} else if(data == LINE_FEED) {

			/* Fin d'un bloc de données */
			if(indexDataBlock < DATA_BLOCK_SIZE) {
				/* Pour les blocs de données non complet */
				checksum -= previous_data;
				indexDataBlock--;
			}
			expectedChecksum = previous_data;

			/* Vérification du checksum */
			Uint8 calculateChecksum = HOKUYO_convertChecksum(checksum);
			if(calculateChecksum == expectedChecksum) {

				/* Traitement des données */
				debug_printf("Compute data : %d\n", angle);
				CMD_MS_computeDataBlock2C(cmd, dataBlock, indexDataBlock, angle);
				angle += ANGLE_RESOLUTION * DATA_BLOCK_SIZE * cmd->command.formated.parameters.MD_MS.clusterCount;
				isNewBloc = TRUE;

			} else {

				/* Le checksum est faux */
				error_printf("Hokuyo Erreur : checksum data block (angle = %d)\n", angle);
				angle += ANGLE_RESOLUTION * DATA_BLOCK_SIZE * cmd->command.formated.parameters.MD_MS.clusterCount;
				isNewBloc = TRUE;

			}

		} else {

			/* Dans le bloc de données */
			if(indexDataBlock < DATA_BLOCK_SIZE) {
				/* On ne veut pas enregistrer le checksum dans les données */
				dataBlock[indexDataBlock++] = data;
				checksum += data;
			} else if (indexDataBlock == DATA_BLOCK_SIZE){
				/* C'est le checksum */
			} else {
				/* Le bloc de données est trop long */
				error_printf("Hokuyo Erreur : data block too long\n");
				isNewBloc = TRUE;
				return CMD_ECHO_ERROR;
			}
		}

		previous_data = data;

	}

	return CMD_ECHO_IN_PROGRESS;

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

static void CMD_MS_computeDataBlock2C(hokuyoSendCommand_s *cmd, Uint8 *dataBlock, Uint8 length, Sint16 angle) {

	Sint16 cos;				// Cosinus de l'angle en cours
	Sint16 sin;				// Sinus de l'angle en cours
	Sint32 x_absolute;		// Position en x sur le terrain du point en cours
	Sint32 y_absolute;		// Position en Y sur le terrain du point en cours
	bool_e point_filtered;	// Le point courant est-il pris en compte

	Sint16 angle_offset_rad = ((Sint32)(FIRST_STEP_DEG)) +
							  ((Sint32)(cmd->command.formated.parameters.MD_MS.startingStep * ANGLE_RESOLUTION));
	angle_offset_rad = DEG100_TO_PI4096(angle_offset_rad);


	/* Le nombre de données du bloc doit être pair */
	if((length % 2) == 0) {

		Uint8 i = 0;
		for(i = 1; i < length; i+=2) {
			Sint32 distance = (((Uint16)dataBlock[i-1]-0x30)<<6)+(((Uint16)dataBlock[i]-0x30)&0x3f);

			/* On élimine les points trop près ou trop loin */
			if(distance > TO_CLOSE_DISTANCE && distance < TO_FAR_DISTANCE) {

				/* Angle relatif au robot */
				Sint16 teta_relative_rad = DEG100_TO_PI4096(angle) - angle_offset_rad;
				/* Angle absolu par rapport au terrain */
				Sint16 teta_absolute_rad = GEOMETRY_modulo_angle(teta_relative_rad);
				/* Calcul du cosinus et du sinus pour l'angle précédent */
				COS_SIN_4096_get(teta_absolute_rad, &cos, &sin);

				/* Calcul de la position sur la terrain suivant la couleur */
				if(ENVIRONMENT_getColor() == BOT_COLOR) {
					x_absolute = (distance*(Sint32)(cos))/4096;
					y_absolute = (distance*(Sint32)(sin))/4096;
				}
				else
				{
					x_absolute = -(distance*(Sint32)(cos))/4096;
					y_absolute = -(distance*(Sint32)(sin))/4096;
				}

				/* On garde les points qui sont sur le terrain */
				if(	x_absolute < FIELD_SIZE_X - HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					x_absolute > HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					y_absolute < FIELD_SIZE_Y - HOKUYO_MARGIN_FIELD_SIDE_IGNORE &&
					y_absolute > HOKUYO_MARGIN_FIELD_SIDE_IGNORE)
				{
					point_filtered = FALSE;

					/* Les 4 coins et deux balises fixes */
					if( x_absolute > FIELD_SIZE_X - MARGIN_BEACON ||
						x_absolute < MARGIN_BEACON ||
						absolute(x_absolute - FIELD_SIZE_X/2) < MARGIN_BEACON)
					{
						if(y_absolute < MARGIN_BEACON || y_absolute > FIELD_SIZE_Y - MARGIN_BEACON) {
							point_filtered = TRUE;
						}

					}

				}

				if(point_filtered == FALSE && nbDetectedPoints < NB_STEP_MAX) {
					detectedPoints[nbDetectedPoints].teta = teta_relative_rad; // Pour l'évitement
					detectedPoints[nbDetectedPoints].coordX = x_absolute;
					detectedPoints[nbDetectedPoints].coordY = y_absolute;
					nbDetectedPoints++;
				}
			}

			/* On passe au prochain point */
			angle += ANGLE_RESOLUTION * cmd->command.formated.parameters.MD_MS.clusterCount;

		}

	} else {
		debug_printf("Hokuyo Error : modulo error\n");
	}
}
