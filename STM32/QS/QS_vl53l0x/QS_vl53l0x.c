#include "QS_vl53l0x.h"

#ifdef USE_VL53L0X

	#include "low_layer/QS_vl53l0x_api.h"
	#include "../QS_ports.h"
	#include "../QS_i2c.h"
	#include "../QS_watchdog.h"

	#include "../QS_outputlog.h"

	/**************************************
	 * Définition des types
	 **************************************/

	typedef Uint8 VL53L0X_address_t;

	typedef struct{
		bool_e initialised;
		bool_e newAddressAffected;
		VL53L0X_address_t address;
		VL53L0X_Dev_t sensor;

	}VL53L0X_sensor_t;

	typedef enum {
		LONG_RANGE 		= 0, /*!< Long range mode */
		HIGH_SPEED 		= 1, /*!< High speed mode */
		HIGH_ACCURACY	= 2, /*!< High accuracy mode */
	} RangingConfig_e;

	/**************************************
	 * Déclaration des variables locales
	 **************************************/

	static void VL53L0X_initSensor(void);
	static bool_e VL53L0X_chipSetAddress(VL53L0X_id_e id, VL53L0X_address_t address);
	static void VL53L0X_chipSelect(VL53L0X_id_e id, bool_e state);
	void Sensor_SetNewRange(VL53L0X_Dev_t *pDev, VL53L0X_RangingMeasurementData_t *pRange);


	/**************************************
	 * Déclaration des variables locales
	 **************************************/

	static const VL53L0X_address_t sensorAddress[DISTANCE_SENSOR_NB_SENSOR] = {
			0x54,
			0x56
	};

	static volatile VL53L0X_sensor_t sensors[DISTANCE_SENSOR_NB_SENSOR];


	int LeakyFactorFix8 = (int)( 0.6 *256);

	/**************************************
	 * Déclaration des fonctions publiques
	 **************************************/

	bool_e VL53L0X_init(void){

		VL53L0X_Error status;

		VL53L0X_RangingMeasurementData_t RangingMeasurementData;

		WATCHDOG_init();

		I2C_init();

		VL53L0X_initSensor();

		VL53L0X_chipSelect(DISTANCE_SENSOR_FORWARD, FALSE);

		GPIO_SetBits(DIST_CS);

		bool_e timeBoot;
		if(WATCHDOG_create_flag(2, &timeBoot) == WATCHDOG_ERROR){
			error_printf("Problème d'initialisation du watchdog d'attente capteur distance\n");
			Uint16 var = 60000;
			while(var--);
			timeBoot = TRUE;
		}

		while(timeBoot == FALSE);

		Uint16 id;
		status = VL53L0X_RdWord((VL53L0X_DEV)&(sensors[0].sensor), VL53L0X_REG_IDENTIFICATION_MODEL_ID, &id);

		if (status) {
			debug_printf("Read id fail\n");
		}

		if(id == 0xEEAA){
			/* Sensor is found => Change its I2C address to final one */
			status = VL53L0X_SetDeviceAddress((VL53L0X_DEV)&(sensors[0].sensor), sensorAddress[0]);
			if(status){
				debug_printf("VL53L0X_SetDeviceAddress fail\n");
			}
			sensors[0].sensor.I2cDevAddr = sensorAddress[0];

			/* Check all is OK with the new I2C address and initialize the sensor */
			status = VL53L0X_RdWord((VL53L0X_DEV)&(sensors[0].sensor), VL53L0X_REG_IDENTIFICATION_MODEL_ID, &id);
			if(status){
				debug_printf("VL53L0X_RdWord fail\n");
			}

			status = VL53L0X_DataInit((VL53L0X_DEV)&(sensors[0].sensor));
			if(status == 0){
				sensors[0].sensor.Present = 1;
			}else{
				debug_printf("VL53L0X_DataInit %d fail\n");
			}
			debug_printf("VL53L0X %d Present and initiated to final 0x%x\n", sensors[0].sensor.Id, sensors[0].sensor.I2cDevAddr);

		}else{
			debug_printf("Unknown ID %x\n", id);
			status = VL53L0X_ERROR_UNDEFINED;
		}


		SetupSingleShot(HIGH_ACCURACY);

		while(1){
			/* only one sensor */
			/* Call All-In-One blocking API function */
			status = VL53L0X_PerformSingleRangingMeasurement((VL53L0X_DEV)&(sensors[0].sensor),&RangingMeasurementData);
			if( status ==0 ){
				/* Push data logging to UART */
				trace_printf("%d,%u,%d,%d,%d\n", sensors[0].sensor.Id, global.absolute_time, RangingMeasurementData.RangeStatus, RangingMeasurementData.RangeMilliMeter, RangingMeasurementData.SignalRateRtnMegaCps);
				Sensor_SetNewRange((VL53L0X_DEV)&(sensors[0].sensor),&RangingMeasurementData);
				/* Display distance in cm */
				if( RangingMeasurementData.RangeStatus == 0 ){
					debug_printf("%4d\n", sensors[0].sensor.LeakyRange);
				}
				else{
					debug_printf("----\n", sensors[0].sensor.LeakyRange);
				}
			}
			else{
				debug_printf("Erreur range\n", sensors[0].sensor.LeakyRange);
			}
		}

		return FALSE;
	}

	/* Store new ranging data into the device structure, apply leaky integrator if needed */
	void Sensor_SetNewRange(VL53L0X_Dev_t *pDev, VL53L0X_RangingMeasurementData_t *pRange){
	    if( pRange->RangeStatus == 0 ){
	        if( pDev->LeakyFirst ){
	            pDev->LeakyFirst = 0;
	            pDev->LeakyRange = pRange->RangeMilliMeter;
	        }
	        else{
	            pDev->LeakyRange = (pDev->LeakyRange*LeakyFactorFix8 + (256-LeakyFactorFix8)*pRange->RangeMilliMeter)>>8;
	        }
	    }
	    else{
	        pDev->LeakyFirst = 1;
	    }
	}

	static void VL53L0X_initSensor(void){
		Uint8 i;

		for(i=0; i<DISTANCE_SENSOR_NB_SENSOR; i++){
			sensors[0].initialised = FALSE;
			sensors[0].newAddressAffected = FALSE;
			sensors[0].address = 0x52;
			sensors[0].sensor.Enabled = TRUE;
			sensors[0].sensor.I2cDevAddr = 0x52;
			sensors[0].sensor.I2cHandle = I2C2;
			sensors[0].sensor.Present = FALSE;
		}
	}

	void VL53L0X_processMain(void){

	}


	VL53L0X_distanceMeasure_t VL53L0X_getDistance(VL53L0X_id_e id){

		return 0;
	}

	/**************************************
	 * Déclaration des fonctions privées
	 **************************************/

	static bool_e VL53L0X_chipSetAddress(VL53L0X_id_e id, VL53L0X_address_t address){

		return FALSE;
	}

	static void VL53L0X_chipSelect(VL53L0X_id_e id, bool_e state){
		//assert(IS_DISTANCE_SENSOR_ID(id));

		/*if(sensors[id].newAddressAffected)
			return;

		switch(id){
			case DISTANCE_SENSOR_FORWARD :
				if(state)
					GPIO_ResetBits(CS_CHIP_DIST_SENSOR_1);
				else
					GPIO_SetBits(CS_CHIP_DIST_SENSOR_1);
				break;

			case DISTANCE_SENSOR_BACKWARD :
				if(state)
					GPIO_ResetBits(CS_CHIP_DIST_SENSOR_2);
				else
					GPIO_SetBits(CS_CHIP_DIST_SENSOR_2);
				break;

			default:
				break;
		}*/
	}

	/**
	 *  Setup all detected sensors for single shot mode and setup ranging configuration
	 */
	void SetupSingleShot(RangingConfig_e rangingConfig){
	    int i;
	    int status;
	    uint8_t VhvSettings;
	    uint8_t PhaseCal;
	    uint32_t refSpadCount;
		uint8_t isApertureSpads;
		FixPoint1616_t signalLimit = (FixPoint1616_t)(0.25*65536);
		FixPoint1616_t sigmaLimit = (FixPoint1616_t)(18*65536);
		uint32_t timingBudget = 33000;
		uint8_t preRangeVcselPeriod = 14;
		uint8_t finalRangeVcselPeriod = 10;

	    for(i=0; i<1; i++){
	        if(sensors[i].sensor.Present){
	            status=VL53L0X_StaticInit(&(sensors[i].sensor));
	            if( status ){
	                debug_printf("VL53L0X_StaticInit %d failed\n",i);
	            }

	            status = VL53L0X_PerformRefCalibration(&(sensors[i].sensor), &VhvSettings, &PhaseCal);
				if( status ){
				   debug_printf("VL53L0X_PerformRefCalibration failed\n");
				}

				status = VL53L0X_PerformRefSpadManagement(&(sensors[i].sensor), &refSpadCount, &isApertureSpads);
				if( status ){
				   debug_printf("VL53L0X_PerformRefSpadManagement failed\n");
				}

	            status = VL53L0X_SetDeviceMode(&(sensors[i].sensor), VL53L0X_DEVICEMODE_SINGLE_RANGING); // Setup in single ranging mode
	            if( status ){
	               debug_printf("VL53L0X_SetDeviceMode failed\n");
	            }

	            status = VL53L0X_SetLimitCheckEnable(&(sensors[i].sensor), VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1); // Enable Sigma limit
				if( status ){
				   debug_printf("VL53L0X_SetLimitCheckEnable failed\n");
				}

				status = VL53L0X_SetLimitCheckEnable(&(sensors[i].sensor), VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1); // Enable Signa limit
				if( status ){
				   debug_printf("VL53L0X_SetLimitCheckEnable failed\n");
				}
				/* Ranging configuration */
	            switch(rangingConfig) {
	            case LONG_RANGE:
	            	signalLimit = (FixPoint1616_t)(0.1*65536);
	            	sigmaLimit = (FixPoint1616_t)(60*65536);
	            	timingBudget = 33000;
	            	preRangeVcselPeriod = 18;
	            	finalRangeVcselPeriod = 14;
	            	break;
	            case HIGH_ACCURACY:
					signalLimit = (FixPoint1616_t)(0.25*65536);
					sigmaLimit = (FixPoint1616_t)(18*65536);
					timingBudget = 200000;
					preRangeVcselPeriod = 14;
					finalRangeVcselPeriod = 10;
					break;
	            case HIGH_SPEED:
					signalLimit = (FixPoint1616_t)(0.25*65536);
					sigmaLimit = (FixPoint1616_t)(32*65536);
					timingBudget = 20000;
					preRangeVcselPeriod = 14;
					finalRangeVcselPeriod = 10;
					break;
	            default:
	            	debug_printf("Not Supported");
	            }

	            status = VL53L0X_SetLimitCheckValue(&(sensors[i].sensor),  VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, signalLimit);
				if( status ){
				   debug_printf("VL53L0X_SetLimitCheckValue failed\n");
				}

				status = VL53L0X_SetLimitCheckValue(&(sensors[i].sensor),  VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, sigmaLimit);
				if( status ){
				   debug_printf("VL53L0X_SetLimitCheckValue failed\n");
				}

	            status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&(sensors[i].sensor),  timingBudget);
	            if( status ){
	               debug_printf("VL53L0X_SetMeasurementTimingBudgetMicroSeconds failed\n");
	            }

	            status = VL53L0X_SetVcselPulsePeriod(&(sensors[i].sensor),  VL53L0X_VCSEL_PERIOD_PRE_RANGE, preRangeVcselPeriod);
				if( status ){
				   debug_printf("VL53L0X_SetVcselPulsePeriod failed\n");
				}

	            status = VL53L0X_SetVcselPulsePeriod(&(sensors[i].sensor),  VL53L0X_VCSEL_PERIOD_FINAL_RANGE, finalRangeVcselPeriod);
				if( status ){
				   debug_printf("VL53L0X_SetVcselPulsePeriod failed\n");
				}

				status = VL53L0X_PerformRefCalibration(&(sensors[i].sensor), &VhvSettings, &PhaseCal);
				if( status ){
				   debug_printf("VL53L0X_PerformRefCalibration failed\n");
				}

				sensors[i].sensor.LeakyFirst=1;
	        }
	    }
	}

#endif
