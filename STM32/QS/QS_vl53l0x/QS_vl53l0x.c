#include "QS_vl53l0x.h"

#ifdef USE_VL53L0X

	#include "low_layer/QS_vl53l0x_api.h"
	#include "../QS_ports.h"
	#include "../QS_i2c.h"

	#include "../QS_outputlog.h"

	#define DEBUG_DISPLAY_SENSOR	// Activer pour avoir l'affichage des données

	/**************************************
	 * Définition des types
	 **************************************/

	typedef Uint8 VL53L0X_address_t;

	typedef enum{
		VL53L0X_LONG_RANGE 		= 0, /*!< Long range mode */
		VL53L0X_HIGH_SPEED 		= 1, /*!< High speed mode */
		VL53L0X_HIGH_ACCURACY	= 2, /*!< High accuracy mode */
	}VL53L0X_rangingConfig_e;

	typedef enum{
		VL53L0X_MEASUREMENT_ONE_SHOT,
		VL53L0X_MEASUREMENT_CONTINUE
	}VL53L0X_measurementConfig;

	typedef struct{
		bool_e initialised;
		bool_e newAddressAffected;
		VL53L0X_address_t setAddress;
		VL53L0X_Dev_t sensor;
		VL53L0X_rangingConfig_e rangeConfig;
		VL53L0X_DeviceModes deviceMode;
		GPIO_TypeDef* GPIOx;
		uint16_t GPIO_Pin;

		bool_e measureAsked;
		bool_e measureDone;
		VL53L0X_distanceMeasure_t measure;
	}VL53L0X_sensor_t;

	/**************************************
	 * Déclaration des variables locales
	 **************************************/

	static void VL53L0X_initStructSensor(void);
	static void VL53L0X_initAddressSensor(void);
	static void VL53L0X_initConfigSensor(void);
	static void VL53L0X_performMeasure(VL53L0X_id_e id);

	/**************************************
	 * Déclaration des variables locales
	 **************************************/

	static volatile VL53L0X_sensor_t sensors[DISTANCE_SENSOR_NB_SENSOR] = {
			{	.setAddress = 0x54, .rangeConfig = VL53L0X_HIGH_ACCURACY, .deviceMode = VL53L0X_DEVICEMODE_CONTINUOUS_RANGING, .GPIOx = CS_LASER_LEFT_PORT, .GPIO_Pin = CS_LASER_LEFT_PIN},	// DISTANCE_SENSOR_SMALL_LEFT
			{	.setAddress = 0x56, .rangeConfig = VL53L0X_HIGH_ACCURACY, .deviceMode = VL53L0X_DEVICEMODE_CONTINUOUS_RANGING, .GPIOx = CS_LASER_RIGHT_PORT, .GPIO_Pin = CS_LASER_RIGHT_PIN}	// DISTANCE_SENSOR_SMALL_RIGHT
	};


	static volatile int LeakyFactorFix8 = (int)( 0.6 *256);

	/**************************************
	 * Déclaration des fonctions publiques
	 **************************************/

	bool_e VL53L0X_init(void){
		I2C_init();

		VL53L0X_initStructSensor();

		VL53L0X_initAddressSensor();

		VL53L0X_initConfigSensor();

		return FALSE;
	}

	void VL53L0X_processMain(void){
		Uint8 i;
		for(i=0; i<DISTANCE_SENSOR_NB_SENSOR; i++){
			if(sensors[i].sensor.Present && sensors[i].measureAsked){
				VL53L0X_performMeasure(i);
			}
		}
	}

	void VL53L0X_askMeasure(VL53L0X_id_e id){
		//assert(IS_VL53L0X_ID(id));

		if(sensors[id].sensor.Present){
			sensors[id].measureDone = FALSE;
			sensors[id].measureAsked = TRUE;
		}else{
			sensors[id].measure = 0;
			sensors[id].measureAsked = FALSE;
			sensors[id].measureDone = TRUE;
		}
	}

	bool_e VL53L0X_measureReady(VL53L0X_id_e id){
		//assert(IS_VL53L0X_ID(id));

		return sensors[id].measureDone;
	}

	VL53L0X_distanceMeasure_t VL53L0X_getMeasure(VL53L0X_id_e id){
		//assert(IS_VL53L0X_ID(id));

		return sensors[id].measure;
	}

	/**************************************
	 * Déclaration des fonctions privées
	 **************************************/

	static void VL53L0X_initStructSensor(void){
		Uint8 i;

		for(i=0; i<DISTANCE_SENSOR_NB_SENSOR; i++){
			sensors[i].initialised = FALSE;
			sensors[i].newAddressAffected = FALSE;
			sensors[i].sensor.Id = i;
			sensors[i].sensor.Enabled = TRUE;
			sensors[i].sensor.I2cDevAddr = 0x52;
			sensors[i].sensor.I2cHandle = VL53L0X_I2C;
			sensors[i].sensor.Present = FALSE;
			sensors[i].measureAsked = FALSE;
			sensors[i].measureDone = FALSE;
			sensors[i].measure = 0;
		}
	}

	static void VL53L0X_initAddressSensor(void){
		Uint8 i;

		// Désactivation de l'ensemble des capteurs
		for(i=0; i<DISTANCE_SENSOR_NB_SENSOR; i++){
			GPIO_ResetBits(sensors[i].GPIOx, sensors[i].GPIO_Pin);
		}

		time32_t beginTimeBoot;
		Uint16 idSensor;
		VL53L0X_Error status;

		// Initialisation de chaque capteur un par un
		for(i=0; i<DISTANCE_SENSOR_NB_SENSOR; i++){
			GPIO_SetBits(sensors[i].GPIOx, sensors[i].GPIO_Pin);

			beginTimeBoot = global.absolute_time;
			while(global.absolute_time - beginTimeBoot < 2);	// Temps d'init réel d'après la doc : 1.2ms

			status = VL53L0X_RdWord((VL53L0X_DEV)&(sensors[i].sensor), VL53L0X_REG_IDENTIFICATION_MODEL_ID, &idSensor);

			if(status){
				debug_printf("VL53L0X Read id register (first time) : %d fail (%d)\n", i, status);
				continue;
			}

			if(idSensor != 0xEEAA){
				debug_printf("VL53L0X Read id register (first time) : %d id unknown (%d)\n", i, status);
				continue;
			}

			status = VL53L0X_SetDeviceAddress((VL53L0X_DEV)&(sensors[i].sensor), sensors[i].setAddress); // Changement de l'adresse I2C du capteur

			if(status){
				debug_printf("VL53L0X_SetDeviceAddress %d fail (%d)\n", i, status);
				continue;
			}

			sensors[i].sensor.I2cDevAddr = sensors[i].setAddress;

			status = VL53L0X_RdWord((VL53L0X_DEV)&(sensors[i].sensor), VL53L0X_REG_IDENTIFICATION_MODEL_ID, &idSensor); // Vérification de la communication avec la lecture de l'id du capteur

			if(status){
				debug_printf("VL53L0X Read id register (second time) : %d  fail (%d)\n", i, status);
				continue;
			}

			if(idSensor != 0xEEAA){
				debug_printf("VL53L0X Read id register (second time) : %d id unknown (%d)\n", i, status);
				continue;
			}

			status = VL53L0X_DataInit((VL53L0X_DEV)&(sensors[i].sensor));

			if(status){
				debug_printf("VL53L0X_DataInit %d fail (%d)\n", i, status);
				continue;
			}

			sensors[i].sensor.Present = 1;

			debug_printf("VL53L0X %d Present and initiated to final 0x%x\n", sensors[i].sensor.Id, sensors[i].sensor.I2cDevAddr);
		}
	}

	static void VL53L0X_initConfigSensor(void){
		Uint8 i;
		Uint8 VhvSettings;
		Uint8 PhaseCal;
		Uint32 refSpadCount;
		Uint8 isApertureSpads;
		FixPoint1616_t signalLimit = (FixPoint1616_t)(0.25*65536);
		FixPoint1616_t sigmaLimit = (FixPoint1616_t)(18*65536);
		Uint32 timingBudget = 33000;
		Uint8 preRangeVcselPeriod = 14;
		Uint8 finalRangeVcselPeriod = 10;
		VL53L0X_Error status;

		for(i=0; i<DISTANCE_SENSOR_NB_SENSOR; i++){

			if(sensors[i].sensor.Present){

				sensors[i].sensor.Present = FALSE; // On pose le capteur étant non accessible

				status = VL53L0X_StaticInit((VL53L0X_DEV)&(sensors[i].sensor));
				if(status){
					debug_printf("VL53L0X_StaticInit %d failed (%d) \n", i, status);
					continue;
				}

				status = VL53L0X_PerformRefCalibration((VL53L0X_DEV)&(sensors[i].sensor), &VhvSettings, &PhaseCal);
				if(status){
					debug_printf("VL53L0X_PerformRefCalibration %d failed (%d) \n", i, status);
					continue;
				}

				status = VL53L0X_PerformRefSpadManagement((VL53L0X_DEV)&(sensors[i].sensor), &refSpadCount, &isApertureSpads);
				if(status){
					debug_printf("VL53L0X_PerformRefSpadManagement %d failed (%d) \n", i, status);
					continue;
				}

				status = VL53L0X_SetDeviceMode((VL53L0X_DEV)&(sensors[i].sensor), sensors[i].deviceMode);
				if(status){
					debug_printf("VL53L0X_SetDeviceMode %d failed (%d) \n", i, status);
					continue;
				}

				status = VL53L0X_SetLimitCheckEnable((VL53L0X_DEV)&(sensors[i].sensor), VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1); // Enable Sigma limit
				if(status){
					debug_printf("VL53L0X_SetLimitCheckEnable %d failed (%d) \n", i, status);
					continue;
				}

				status = VL53L0X_SetLimitCheckEnable((VL53L0X_DEV)&(sensors[i].sensor), VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1); // Enable Signa limit
				if(status){
					debug_printf("VL53L0X_SetLimitCheckEnable %d failed (%d) \n", i, status);
					continue;
				}

				switch(sensors[i].rangeConfig){
					case VL53L0X_LONG_RANGE:
						signalLimit = (FixPoint1616_t)(0.1*65536);
						sigmaLimit = (FixPoint1616_t)(60*65536);
						timingBudget = 33000;
						preRangeVcselPeriod = 18;
						finalRangeVcselPeriod = 14;
						break;

					case VL53L0X_HIGH_ACCURACY:
						signalLimit = (FixPoint1616_t)(0.25*65536);
						sigmaLimit = (FixPoint1616_t)(18*65536);
						timingBudget = 200000;
						preRangeVcselPeriod = 14;
						finalRangeVcselPeriod = 10;
						break;

					case VL53L0X_HIGH_SPEED:
						signalLimit = (FixPoint1616_t)(0.25*65536);
						sigmaLimit = (FixPoint1616_t)(32*65536);
						timingBudget = 20000;
						preRangeVcselPeriod = 14;
						finalRangeVcselPeriod = 10;
						break;

					default:
						debug_printf("VL53L0X Range Not Supported %d\n", i);
						continue;
				}

				status = VL53L0X_SetLimitCheckValue((VL53L0X_DEV)&(sensors[i].sensor), VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, signalLimit);
				if(status){
					debug_printf("VL53L0X_SetLimitCheckValue %d failed (%d) \n", i, status);
					continue;
				}

				status = VL53L0X_SetLimitCheckValue((VL53L0X_DEV)&(sensors[i].sensor), VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, sigmaLimit);
				if(status){
					debug_printf("VL53L0X_SetLimitCheckValue %d failed (%d) \n", i, status);
					continue;
				}

				status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds((VL53L0X_DEV)&(sensors[i].sensor),  timingBudget);
				if(status){
					debug_printf("VL53L0X_SetMeasurementTimingBudgetMicroSeconds %d failed (%d) \n", i, status);
					continue;
				}

				status = VL53L0X_SetVcselPulsePeriod((VL53L0X_DEV)&(sensors[i].sensor), VL53L0X_VCSEL_PERIOD_PRE_RANGE, preRangeVcselPeriod);
				if(status){
					debug_printf("VL53L0X_SetVcselPulsePeriod %d failed (%d) \n", i, status);
					continue;
				}

				status = VL53L0X_SetVcselPulsePeriod((VL53L0X_DEV)&(sensors[i].sensor), VL53L0X_VCSEL_PERIOD_FINAL_RANGE, finalRangeVcselPeriod);
				if(status){
					debug_printf("VL53L0X_SetVcselPulsePeriod %d failed (%d) \n", i, status);
					continue;
				}

				status = VL53L0X_PerformRefCalibration((VL53L0X_DEV)&(sensors[i].sensor), &VhvSettings, &PhaseCal);
				if(status){
					debug_printf("VL53L0X_PerformRefCalibration %d failed (%d) \n", i, status);
					continue;
				}

				status = VL53L0X_StartMeasurement((VL53L0X_DEV)&(sensors[i].sensor));
				if(status){
					debug_printf("VL53L0X_StartMeasurement %d failed (%d) \n", i, status);
					continue;
				}

				sensors[i].sensor.Present = TRUE;
				sensors[i].sensor.LeakyFirst = 1;
			}
		}
	}

	static void VL53L0X_performMeasure(VL53L0X_id_e id){
		//assert(IS_VL53L0X_ID(id));
		assert(sensors[id].sensor.Present);

		VL53L0X_Error status;
		VL53L0X_RangingMeasurementData_t rangingMeasurementData;

		status = VL53L0X_GetRangingMeasurementData((VL53L0X_DEV)&(sensors[id].sensor), &rangingMeasurementData);
		if(status){
			debug_printf("VL53L0X_GetRangingMeasurementData %d failed\n", id);
			sensors[id].measureDone = TRUE;
			sensors[id].measureAsked = FALSE;
			sensors[id].measure = 0;
		}

		if(rangingMeasurementData.RangeStatus == 0){

			if(sensors[id].sensor.LeakyFirst){
				sensors[id].sensor.LeakyFirst = 0;
				sensors[id].sensor.LeakyRange = rangingMeasurementData.RangeMilliMeter;
			}else{
				sensors[id].sensor.LeakyRange = (sensors[id].sensor.LeakyRange * LeakyFactorFix8 + (256 - LeakyFactorFix8) * rangingMeasurementData.RangeMilliMeter) >> 8;
			}

		}else{
			sensors[id].sensor.LeakyFirst = 1;
		}

		if(rangingMeasurementData.RangeStatus == 0){
			sensors[id].measure = sensors[id].sensor.LeakyRange;
			#ifdef DEBUG_DISPLAY_SENSOR
				debug_printf("VL53L0X %d : %4d\n", id, sensors[id].sensor.LeakyRange);
			#endif
		}else{
			sensors[id].measure = 0;
			#ifdef DEBUG_DISPLAY_SENSOR
				debug_printf("VL53L0X %d : ----\n", id, sensors[id].sensor.LeakyRange);
			#endif
		}

		sensors[id].measureDone = TRUE;
		sensors[id].measureAsked = FALSE;
	}
#endif
