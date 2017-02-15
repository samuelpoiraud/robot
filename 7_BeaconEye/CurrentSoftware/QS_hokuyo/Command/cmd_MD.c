/*
 * cmdScan.c
 *
 *  Created on: 15 févr. 2017
 *      Author: guill
 */

#include "cmd_MD.h"

#include <string.h>
#include "../../QS/QS_outputlog.h"



hokuyoSendCommand_s CMD_MD_create(Uint16 startingStep,
		                          Uint16 endStep,
								  Uint8 clusterCount,
								  Uint8 scanInterval,
								  Uint8 numberOfScans,
								  char *stringCharacters)
{

	hokuyoSendCommand_s cmd;

	cmd.command.formated.commandSymbol = CMD_MD;
	cmd.command.formated.parameters.MD_MS.startingStep = startingStep;
	cmd.command.formated.parameters.MD_MS.endStep = endStep;
	cmd.command.formated.parameters.MD_MS.clusterCount = clusterCount;
	cmd.command.formated.parameters.MD_MS.scanInterval = scanInterval;
	cmd.command.formated.parameters.MD_MS.numberOfScans = numberOfScans;
	strncpy((char*)cmd.command.formated.stringCharacters, stringCharacters, STRING_CHARACTERS_SIZE - 1);

	Uint8 index = 0;
	strcpy((char*)(cmd.cmdStr + index), "MD");
	index += COMMAND_SYMBOL_SIZE;
	sprintf((char*)(cmd.cmdStr + index), "%04d", startingStep);
	index += STARTING_STEP_SIZE;
	sprintf((char*)(cmd.cmdStr + index), "%04d", endStep);
	index += END_STEP_SIZE;
	sprintf((char*)(cmd.cmdStr + index), "%02d", clusterCount);
	index += CLUSTER_COUNT_SIZE;
	sprintf((char*)(cmd.cmdStr + index), "%01d", scanInterval);
	index += SCAN_INTERVAL_SIZE;
	sprintf((char*)(cmd.cmdStr + index), "%02d", numberOfScans);
	index += NUMBER_OF_SCANS;
	strcpy((char*)(cmd.cmdStr + index), (char*)cmd.command.formated.stringCharacters);

	debug_printf("MD command generated : %s\n", cmd.cmdStr);

	return cmd;
}




