#ifndef SYNCHRO_H
	#define SYNCHRO_H

	#include "QS/QS_all.h"
	#include "QS/QS_rf.h"

	void SYNCHRO_init();
	void SYNCHRO_process_main();

	bool_e SYNCHRO_get_warner_foe1_is_rf_unreacheable(void);
	bool_e SYNCHRO_get_warner_foe2_is_rf_unreacheable(void);
	bool_e SYNCHRO_is_synchronized(void);
	Uint8 SYNCHRO_getSynchronisedTime(void);

#endif // SYNCHRO_H
