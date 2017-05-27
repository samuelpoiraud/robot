#include "scan.h"

#include "QS/QS_vl53l0x/QS_vl53l0x.h"
#include "QS/QS_outputlog.h"

#ifdef USE_SCAN

	typedef enum{
		SCAN_THRESHOLD_FALSE_ON_LOWER,
		SCAN_THRESHOLD_FALSE_ON_UPPER
	}SCAN_on_shot_threshold_way_e;

	typedef struct{
		Uint16 min;
		Uint16 max;
		Uint16 xForMin;
	}SCAN_data_s;

	typedef struct{
		bool_e SCAN_asked;
		bool_e SCAN_readyToTransmit;
		Uint8 nbTry;
		VL53L0X_distanceMeasure_t distance;
		bool_e presence;
		VL53L0X_distanceMeasure_t threshold;
		SCAN_on_shot_threshold_way_e thresholdWay;

		VL53L0X_id_e idVL53L0X;
	}SCAN_on_shot_data_s;

	static volatile bool_e SCAN_activate = FALSE;
	static volatile SCAN_I2C_side_e SCAN_side;
	static volatile SCAN_data_s data;

	static volatile SCAN_on_shot_data_s dataOnShot[SCAN_SENSOR_ID_NB] = {
			(SCAN_on_shot_data_s){.threshold = 40, .thresholdWay = SCAN_THRESHOLD_FALSE_ON_UPPER, .idVL53L0X = DISTANCE_SENSOR_SMALL_ELEVATOR},		// SCAN_SENSOR_ID_SMALL_ELEVATOR
			(SCAN_on_shot_data_s){.threshold = 30, .thresholdWay = SCAN_THRESHOLD_FALSE_ON_UPPER, .idVL53L0X = DISTANCE_SENSOR_SMALL_SLIDER}		// DISTANCE_SENSOR_SMALL_SLIDER
	};

	static void SCAN_launchScan();
	static void SCAN_computeScan();

	static void SCAN_on_shot_launch();
	static void SCAN_on_shot_compute();
	static void SCAN_on_shot_sendMsg();

	void SCAN_init(void){
		VL53L0X_init();

		Uint8 i;
		for(i=0; i<SCAN_SENSOR_ID_NB; i++){
			dataOnShot[i].SCAN_asked = FALSE;
			dataOnShot[i].SCAN_readyToTransmit = FALSE;
			dataOnShot[i].distance = 0;
			dataOnShot[i].nbTry = 0;
			dataOnShot[i].presence = FALSE;
		}

	}

	void SCAN_processMain(void){
		if(SCAN_activate && global.pos.updated){
			SCAN_launchScan();
		}

		SCAN_on_shot_launch();

		VL53L0X_processMain();

		SCAN_on_shot_compute();

		SCAN_on_shot_sendMsg();

		if(SCAN_activate && global.pos.updated){
			SCAN_computeScan();
		}
	}

	void SCAN_processMsg(CAN_msg_t * msg){
		switch(msg->sid){

			case ACT_START_SCAN:

				SCAN_side = msg->data.act_start_scan.side;
				SCAN_activate = TRUE;

				data.min = 0xFFFF;
				data.max = 0;

				break;

			case ACT_STOP_SCAN:{

				SCAN_activate = FALSE;

				CAN_msg_t msgSend;
				msgSend.sid = ACT_RESULT_SCAN;
				msgSend.size = SIZE_ACT_RESULT_SCAN;
				msgSend.data.act_result_scan.max = data.max;
				msgSend.data.act_result_scan.min = data.min;
				msgSend.data.act_result_scan.xForMin = data.xForMin;
				CAN_send(&msgSend);

				}break;

			case ACT_SCAN_DISTANCE:
				if(msg->data.act_scan_distance.idSensor < SCAN_SENSOR_ID_NB){
					dataOnShot[msg->data.act_scan_distance.idSensor].nbTry = 0;
					dataOnShot[msg->data.act_scan_distance.idSensor].SCAN_readyToTransmit = 0;
					dataOnShot[msg->data.act_scan_distance.idSensor].SCAN_asked = TRUE;
				}
				break;

			default:
				error_printf("default switch SCAN_processMsg\n");
				break;
		}
	}

	static void SCAN_launchScan(){
		switch(SCAN_side){
			case SCAN_I2C_LEFT:
				VL53L0X_askMeasure(DISTANCE_SENSOR_SMALL_LEFT);
				break;

			case SCAN_I2C_RIGHT:
				VL53L0X_askMeasure(DISTANCE_SENSOR_SMALL_SLIDER);
				break;

			default:
				error_printf("default switch SCAN_launchScan\n");
				break;
		}
	}

	static void SCAN_on_shot_launch(){
		Uint8 i;
		for(i=0; i<SCAN_SENSOR_ID_NB; i++){
			if(dataOnShot[i].SCAN_asked){
				VL53L0X_askMeasure(dataOnShot[i].idVL53L0X);
			}
		}
	}

	static void SCAN_on_shot_compute(){
		Uint8 i;
		for(i=0; i<SCAN_SENSOR_ID_NB; i++){
			if(dataOnShot[i].SCAN_asked){

				VL53L0X_distanceMeasure_t dist = VL53L0X_getMeasure(dataOnShot[i].idVL53L0X);

				if(dist != 0){

					dataOnShot[i].distance = dist;

					if(dataOnShot[i].thresholdWay == SCAN_THRESHOLD_FALSE_ON_LOWER){
						if(dataOnShot[i].distance > dataOnShot[i].threshold){
							dataOnShot[i].presence = TRUE;
						}else{
							dataOnShot[i].presence = FALSE;
						}
					}else{
						if(dataOnShot[i].distance < dataOnShot[i].threshold){
							dataOnShot[i].presence = TRUE;
						}else{
							dataOnShot[i].presence = FALSE;
						}
					}

					dataOnShot[i].SCAN_asked = FALSE;
					dataOnShot[i].SCAN_readyToTransmit = TRUE;

				}else if(dataOnShot[i].nbTry < 3){
					error_printf("Erreur lecture distance %s\n", VL53L0X_getNameSensor(dataOnShot[i].idVL53L0X));
					dataOnShot[i].nbTry++;
				}else{
					dataOnShot[i].distance = 0;
					dataOnShot[i].presence = FALSE;
					dataOnShot[i].SCAN_asked = FALSE;
					dataOnShot[i].SCAN_readyToTransmit = TRUE;
				}
			}
		}
	}

	static void SCAN_on_shot_sendMsg(){
		Uint8 i;
		for(i=0; i<SCAN_SENSOR_ID_NB; i++){
			if(dataOnShot[i].SCAN_readyToTransmit){

				dataOnShot[i].SCAN_readyToTransmit = FALSE;

				CAN_msg_t msg;

				msg.sid = ACT_SCAN_DISTANCE_RESULT;
				msg.size = SIZE_ACT_SCAN_DISTANCE_RESULT;
				msg.data.act_scan_distance_result.idSensor = i;
				msg.data.act_scan_distance_result.distance = dataOnShot[i].distance;
				msg.data.act_scan_distance_result.present = dataOnShot[i].presence;

				CAN_send(&msg);

			}
		}
	}

	static void SCAN_computeScan(){
		bool_e scanDone;
		VL53L0X_distanceMeasure_t scanMeasure;

		switch(SCAN_side){
			case SCAN_I2C_LEFT:
				scanDone = VL53L0X_measureReady(DISTANCE_SENSOR_SMALL_LEFT);
				scanMeasure = VL53L0X_getMeasure(DISTANCE_SENSOR_SMALL_LEFT);
				break;

			case SCAN_I2C_RIGHT:
				scanDone = VL53L0X_measureReady(DISTANCE_SENSOR_SMALL_SLIDER);
				scanMeasure = VL53L0X_getMeasure(DISTANCE_SENSOR_SMALL_SLIDER);
				break;

			default:
				scanDone = FALSE;
				error_printf("default switch SCAN_computeScan\n");
				break;
		}

		if(scanDone == FALSE)
			return;

		if(scanMeasure > data.max)
			data.max = scanMeasure;

		if(scanMeasure < data.min){
			data.min = scanMeasure;
			data.xForMin = global.pos.x;
		}
	}
#endif
