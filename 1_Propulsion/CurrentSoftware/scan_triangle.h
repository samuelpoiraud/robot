/*
 * scan_triangle.h
 *
 *  Created on: 11 oct. 2013
 *      Author: Arnaud
 */

#ifndef SCAN_TRIANGLE_H_
#define SCAN_TRIANGLE_H_


#include "QS/QS_all.h"

#define ADC_SENSOR_DT10_FLOOR	ADC_11
#define ADC_SENSOR_DT10_NV1		ADC_12
#define ADC_SENSOR_DT10_NV2		ADC_14

#ifdef SCAN_TRIANGLE
	void SCAN_TRIANGLE_init(void);
	void SCAN_TRIANGLE_process_it(void);
	void SCAN_TRIANGLE_canMsg(CAN_msg_t *msg);
	void SCAN_TRIANGLE_WARNER_canMsg(CAN_msg_t *msg);
	void SCAN_TRIANGLE_calculate(void);
#endif

#endif /* SCAN_TRIANGLE_H_ */
