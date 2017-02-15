/*
 * cmdScan.h
 *
 *  Created on: 15 févr. 2017
 *      Author: guill
 */

#ifndef QS_HOKUYO_COMMAND_CMD_MD_H_
#define QS_HOKUYO_COMMAND_CMD_MD_H_

#include "../../QS/QS_all.h"
#include "hokuyoMsgType.h"

hokuyoSendCommand_s CMD_MD_create(Uint16 startingStep,
		                          Uint16 endStep,
								  Uint8 clusterCount,
								  Uint8 scanInterval,
								  Uint8 numberOfScans,
								  char *stringCharacters);

#endif /* QS_HOKUYO_COMMAND_CMD_MD_H_ */
