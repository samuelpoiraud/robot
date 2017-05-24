#include "scan.h"

#include "QS/QS_vl53l0x/QS_vl53l0x.h"
#include "QS/QS_outputlog.h"

#ifdef USE_SCAN

	typedef struct{
		Uint16 min;
		Uint16 max;
		Uint16 xForMin;
	}SCAN_data_s;

	static volatile bool_e SCAN_activate = FALSE;
	static volatile SCAN_I2C_side_e SCAN_side;
	static volatile SCAN_data_s data;

	static void SCAN_launchScan();
	static void SCAN_computeScan();

	void SCAN_init(void){
		VL53L0X_init();
	}

	void SCAN_processMain(void){
		if(SCAN_activate && global.pos.updated){
			SCAN_launchScan();
		}

		 VL53L0X_processMain();

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

				CAN_msg_t msg;
				msg.sid = ACT_RESULT_SCAN;
				msg.size = SIZE_ACT_RESULT_SCAN;
				msg.data.act_result_scan.max = data.max;
				msg.data.act_result_scan.min = data.min;
				msg.data.act_result_scan.xForMin = data.xForMin;
				CAN_send(&msg);

				}break;

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
				VL53L0X_askMeasure(DISTANCE_SENSOR_SMALL_RIGHT);
				break;

			default:
				error_printf("default switch SCAN_launchScan\n");
				break;
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
				scanDone = VL53L0X_measureReady(DISTANCE_SENSOR_SMALL_RIGHT);
				scanMeasure = VL53L0X_getMeasure(DISTANCE_SENSOR_SMALL_RIGHT);
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
