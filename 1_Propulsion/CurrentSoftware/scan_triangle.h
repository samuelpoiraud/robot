/*
 * scan_triangle.h
 *
 *  Created on: 11 oct. 2013
 *      Author: Arnaud
 */

#ifndef SCAN_TRIANGLE_H_
#define SCAN_TRIANGLE_H_

	#include "QS/QS_adc.h"
	#include "QS/QS_all.h"
	#include "QS/QS_outputlog.h"
	#include "supervisor.h"
	#include "corrector.h"
	#include "roadmap.h"
	#include "cos_sin.h"

	void SCAN_TRIANGLE_init(void);
	void SCAN_TRIANGLE_process_it(void);
	void SCAN_TRIANGLE_canMsg(void);
	void SCAN_TRIANGLE_calculate(void);


#endif /* SCAN_TRIANGLE_H_ */
