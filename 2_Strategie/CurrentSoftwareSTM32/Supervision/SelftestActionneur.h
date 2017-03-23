/*
 * SelftestActionneur.h
 *
 *  Created on: Mar 11, 2017
 *      Author: thoma
 */
#include "../QS/QS_all.h"

#ifndef SUPERVISION_SELFTESTACTIONNEUR_H_
	#define SUPERVISION_SELFTESTACTIONNEUR_H_

	#include "../QS/QS_types.h"
	#include "../actuator/queue.h"

	typedef struct{
			Uint8 numero_etape;
			ACT_sid_e actionneur;
			ACT_order_e position;
			queue_id_e queue_id;
			SELFTEST_error_code_e error_code;
	}struct_selftest_t;


#endif /* SUPERVISION_SELFTESTACTIONNEUR_H_ */
