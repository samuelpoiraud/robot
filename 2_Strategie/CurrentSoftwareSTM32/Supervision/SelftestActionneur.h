/*
 * SelftestActionneur.h
 *
 *  Created on: Mar 11, 2017
 *      Author: thoma
 */
#include "../QS/QS_all.h"

#ifndef SUPERVISION_SELFTESTACTIONNEUR_H_
	#define SUPERVISION_SELFTESTACTIONNEUR_H_

	typedef struct{
			Uint8 numero_etape;
			ACT_sid_e actionneur;
			ACT_order_e position;
	}struct_selftest_t;


	#define LAST_ETAPE = 4


#endif /* SUPERVISION_SELFTESTACTIONNEUR_H_ */
