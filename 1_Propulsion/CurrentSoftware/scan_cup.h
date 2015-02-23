#define SCAN_CUP

#ifdef SCAN_CUP
	#include "QS/QS_all.h"

	#define SCAN_CUP_SENSOR			ADC_11

	void SCAN_CUP_init(void);
	void SCAN_CUP_process_it();
	void SCAN_CUP_canMsg(CAN_msg_t *msg);
	void SCAN_CUP_calculate(void);

#endif
