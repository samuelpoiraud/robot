
#ifndef _VL53L0X_H_
	#define _VL53L0X_H_

	#include "../QS_all.h"

	#ifdef USE_VL53L0X

		typedef enum{
			DISTANCE_SENSOR_FORWARD,
			DISTANCE_SENSOR_BACKWARD,
			DISTANCE_SENSOR_NB_SENSOR
		}VL53L0X_id_e;
		#define IS_DISTANCE_SENSOR_ID(x)		(									\
												((x) == DISTANCE_SENSOR_FORWARD)	\
												((x) == DISTANCE_SENSOR_BACKWARD)	\
												)

		typedef Uint16 VL53L0X_distanceMeasure_t;		// [mm]

		bool_e VL53L0X_init(void);
		void VL53L0X_processMain(void);
		VL53L0X_distanceMeasure_t VL53L0X_getDistance(VL53L0X_id_e id);

	#endif

#endif /* _VL53L0X_H_ */
