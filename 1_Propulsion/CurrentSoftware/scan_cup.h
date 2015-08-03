#define SCAN_CUP

#ifdef SCAN_CUP
	#include "QS/QS_all.h"
	#include "QS/QS_can.h"

	void SCAN_CUP_init(void);
	void SCAN_CUP_process_it();
	void SCAN_CUP_canMsg(CAN_msg_t *msg);
	void SCAN_CUP_calculate(void);

#endif
