/*
 *	Club Robot ESEO 2012-2015
 *
 *	Fichier : supervision.c
 *	Package : Balise émettrice
 *	Description : Supervision de l'état de la balise
 *	Auteur : Arnaud
 *	Version 201203
 */

#include "supervision.h"

#include "QS/QS_adc.h"
#include "QS/QS_rf.h"

#define THRESHOLD_VOLTAGE_BATTERY		3500			// [mV]
#define TIME_REPEAT_LOW_BATTERY			4000			// [mS]

void SUPERVISION_init(){
	ADC_init();
}

void SUPERVISION_process_main(){
	static Uint16 average_value = 0;
	static bool_e first_loop = TRUE;
	static bool_e warn_send = FALSE;
	static time32_t last_time_send = 0;

	if(first_loop)
		average_value = ADC_getValue(ADC_CHANNEL_MEASURE_BAT);
	else
		average_value = average_value*15/16 + ADC_getValue(ADC_CHANNEL_MEASURE_BAT)/16;

	if(average_value < THRESHOLD_VOLTAGE_BATTERY && warn_send == FALSE){
		CAN_msg_t msg;
		msg.sid = BEACON_WARNING_LOW_BATTERY;
		msg.size = SIZE_BEACON_WARNING_LOW_BATTERY;
		msg.data.beacon_warning_low_battery.battery_voltage = average_value;
		RF_can_send(RF_BROADCAST, &msg);

		last_time_send = global.absolute_time;
		warn_send = TRUE;
	}

	if(global.absolute_time - last_time_send > TIME_REPEAT_LOW_BATTERY)
		warn_send = FALSE;
}
