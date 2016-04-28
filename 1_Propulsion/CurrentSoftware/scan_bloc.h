#ifndef SCAN_BLOC_H
#define SCAN_BLOC_H

	#include "QS/QS_all.h"
	#include "QS/QS_can.h"

	void SCAN_BLOC_init(void);
	void SCAN_BLOC_process_it();
	void SCAN_BLOC_canMsg(CAN_msg_t *msg);
	void SCAN_BLOC_calculate();

#endif
