/*
 * gyroscope.h
 *
 *  Created on: 12 juin. 2014
 *      Author: Anthony
 */

#ifndef GYROSCOPE_H_
#define GYROSCOPE_H_

#include "_Propulsion_config.h"
#include "QS/QS_types.h"

	#ifdef USE_GYROSCOPE
		void GYRO_test();
		void GYRO_init(void);
		Uint32 GYRO_GetSensorData(void);
		Uint16 GYRO_GetRegisterValue(Uint8 registerAddress);
	#endif


#endif /* GYROSCOPE_H_ */
