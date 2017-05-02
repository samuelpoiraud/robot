/*
 * gyroscope.h
 *
 *  Created on: 12 juin. 2014
 *      Author: Anthony
 */

#ifndef GYROSCOPE_H_
#define GYROSCOPE_H_

#include "QS/QS_all.h"
#include "QS/QS_CANmsgList.h"

	#ifdef USE_GYROSCOPE
		void GYRO_test();
		void GYRO_init(void);
		Sint16 GYRO_GetSensorData(bool_e verbose, bool_e * valid);
		Sint16 ADXRS453_GetTemperature(void);
		Sint32 GYRO_get_speed_rotation(bool_e * valid, bool_e reset);
	#endif

		void GYRO_set_coef(PROPULSION_coef_e coef, Sint32 value);
		void GYRO_reset_coef(PROPULSION_coef_e coef);

#endif /* GYROSCOPE_H_ */
