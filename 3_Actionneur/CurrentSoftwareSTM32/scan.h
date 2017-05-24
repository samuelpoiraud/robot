
#ifndef SCAN_H
	#define SCAN_H

	#include "QS/QS_all.h"

	#ifdef USE_SCAN

		#include "QS/QS_can.h"

		void SCAN_init(void);

		void SCAN_processMain(void);

		void SCAN_processMsg(CAN_msg_t * msg);

	#endif

#endif // SCAN_H
