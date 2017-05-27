
/** ----------------  Defines possibles  --------------------
 *	USE_VL53L0X					: Activation du VL53L0X
 *	VL53L0X_I2C					: I2C utilisé pour le capteur de distance
 */

#ifndef _VL53L0X_H_
	#define _VL53L0X_H_

	#include "../QS_all.h"

	#ifdef USE_VL53L0X

		/* Bus I2C utilisé */
		#ifndef VL53L0X_I2C
			#error I2C VL53L0X non défini
		#endif

		typedef enum{
			DISTANCE_SENSOR_SMALL_LEFT,
			DISTANCE_SENSOR_SMALL_SLIDER,
			DISTANCE_SENSOR_SMALL_ELEVATOR,
			DISTANCE_SENSOR_NB_SENSOR
		}VL53L0X_id_e;
		#define IS_VL53L0X_ID(x)	(										\
									((x) == DISTANCE_SENSOR_SMALL_LEFT)		\
									((x) == DISTANCE_SENSOR_SMALL_SLIDER)	\
									((x) == DISTANCE_SENSOR_SMALL_ELEVATOR)	\
									)

		typedef Uint16 VL53L0X_distanceMeasure_t;		// [mm]

		bool_e VL53L0X_init(void);

		void VL53L0X_processMain(void);

		void VL53L0X_askMeasure(VL53L0X_id_e id);

		bool_e VL53L0X_measureReady(VL53L0X_id_e id);

		VL53L0X_distanceMeasure_t VL53L0X_getMeasure(VL53L0X_id_e id);

		const char * VL53L0X_getNameSensor(VL53L0X_id_e id);

	#endif

#endif /* _VL53L0X_H_ */
