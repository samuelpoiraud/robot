/*
 * gyroscope.h
 *
 *  Created on: 12 juin. 2014
 *      Author: Anthony
 */

#ifndef GYROSCOPE_H_
#define GYROSCOPE_H_

#include "QS/QS_all.h"

	#ifdef USE_GYROSCOPE
		void GYRO_test();
		void GYRO_init(void);
		Sint16 GYRO_GetSensorData(bool_e verbose, bool_e * valid);
		Sint16 ADXRS453_GetTemperature(void);
		Sint32 GYRO_get_speed_rotation(bool_e * valid);
	#endif


#endif /* GYROSCOPE_H_ */
