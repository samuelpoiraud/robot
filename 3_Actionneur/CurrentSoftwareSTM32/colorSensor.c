/*
 *	Club Robot ESEO 2016 - 2017
 *	Harry & Anne
 *
 *	Fichier : mosfetBoard.c
 *	Package : Actionneur
 *	Description : Communication avec la carte mosfet
 *  Auteurs : Arnaud
 *  Version 20110225
 */

#include "colorSensor.h"
#include "QS/QS_apds9960.h"
#include "QS/QS_ports.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"

#ifdef USE_I2C_COLOR_SENSOR

	//#define SENSOR_DEBUG_MODE

	#define COLOR_SENSOR_REFRESH	200		// [ms]

	#define THRESHOLD_BLUE			10
	#define THRESHOLD_YELLOW		10
	#define THRESHOLD_WHITE			10

	static volatile COLOR_SENSOR_color_e actualColor = COLOR_SENSOR_NONE, lastColor = COLOR_SENSOR_NONE;
	static volatile COLOR_SENSOR_color_e actualPresence = FALSE, lastPresence = FALSE;

	static void COLOR_SENSOR_computeColor(Uint32 ambiant, Uint32 blue, Uint32 white, Uint32 yellow);

	void COLOR_SENSOR_init(){
		GPIO_SetBits(SMALL_COLOR_I2C_ACTIVATE);
		Uint32 i;
		for(i=0;i<100000;i++);
		APDS9960_init();
		APDS9960_enableLightSensor(FALSE);
	}

	void COLOR_SENSOR_processMain(){
		static time32_t lastProcess;

		#ifdef SENSOR_DEBUG_MODE
			static time32_t lastDisplay;
		#endif

		if(global.absolute_time - lastProcess > COLOR_SENSOR_REFRESH){
			Uint16 ambiant, red, blue, green;
			Uint32 blue_found, yellow_found, white_found;

			APDS9960_readAmbientLight(&ambiant);
			APDS9960_readRedLight(&red);
			APDS9960_readBlueLight(&blue);

			APDS9960_readGreenLight(&green);

			if(ambiant)
			{
				//Mesure des composants RGB, ramenéesen pourcentage de la mesure de lumière ambiante
				blue = (Uint16)(((Uint32)blue*100)/ambiant);
				red = (Uint16)(((Uint32)red*100)/ambiant);
				green = (Uint16)(((Uint32)green*100)/ambiant);

				//Estimation de l'écart aux couleurs possible du modules. Plus ces grandeur sont faibles, plus la probabilité de la couleur est élevée !
				blue_found = absolute(blue-45)+absolute(green-50)+absolute(red-23);
				yellow_found = absolute(blue-16)+absolute(green-44)+absolute(red-51);
				white_found = absolute(blue-25)+absolute(green-43)+absolute(red-44);

			}else{

				blue_found = 0xFFFFFFFF;
				yellow_found = 0xFFFFFFFF;
				white_found = 0xFFFFFFFF;

			}

			#ifdef SENSOR_DEBUG_MODE
				if(global.absolute_time - lastDisplay > 1000){
					debug_printf("%d; %d; %d; %d; %d; %d; %d\n", ambiant, red, green, blue, blue_found, white_found, yellow_found);
					lastDisplay = global.absolute_time;
				}
			#endif

			COLOR_SENSOR_computeColor(ambiant, blue_found, white_found, yellow_found);

			lastProcess = global.absolute_time;
		}
	}

	bool_e COLOR_SENSOR_getPresence(){
		return actualPresence;
	}

	COLOR_SENSOR_color_e COLOR_SENSOR_getColor(){
		return actualColor;
	}

	static void COLOR_SENSOR_computeColor(Uint32 ambiant, Uint32 blue, Uint32 white, Uint32 yellow){

		Uint32 color[3] = {blue, white, yellow};

		Uint8 i, indiceMin = 0;
		for(i=0; i<3; i++){
			if(color[i] < color[indiceMin]){
				indiceMin = i;
			}
		}

		if(indiceMin == 1){ // Si on détecte du blanc
			if(ambiant > 1000)
				actualColor = indiceMin + 1;
			else
				actualColor = 0;
		}else{
			actualColor = indiceMin + 1;
		}

		if(lastColor != actualColor){
			COLOR_SENSOR_sendColor();
			lastColor = actualColor;
		}
	}

	void COLOR_SENSOR_sendColor(){
		CAN_msg_t msg;

		msg.sid = ACT_TELL_COLOR_SENSOR_I2C;
		msg.size = SIZE_ACT_TELL_COLOR_SENSOR_I2C;
		msg.data.act_tell_color_sensor_i2c.color = actualColor;
		CAN_send(&msg);

		#ifdef SENSOR_DEBUG_MODE
			/*switch(actualColor){
				case COLOR_SENSOR_NONE :
					debug_printf("%s\n", "COLOR_SENSOR_NONE");
					break;

				case COLOR_SENSOR_BLUE :
					debug_printf("%s\n", "COLOR_SENSOR_BLUE");
					break;

				case COLOR_SENSOR_WHITE :
					debug_printf("%s\n", "COLOR_SENSOR_WHITE");
					break;

				case COLOR_SENSOR_YELLOW :
					debug_printf("%s\n", "COLOR_SENSOR_YELLOW");
					break;

				default:
					debug_printf("%s\n", COLOR_SENSOR_NONE);
					break;
			}*/
		#endif
	}

#endif
