/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : voltage_measure.c
 *	Package : Carte IHM
 *	Description : Contr�le de la tension des batteries
 *	Auteur : Anthony / Arnaud
 *	Version 2012/01/14
 */

#include "voltage_measure.h"
#include "QS/QS_adc.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_IHM.h"
#include "QS/QS_can.h"
#include "QS/QS_types.h"
#include "QS/QS_outputlog.h"

#define PERCENTAGE_FILTER			10
#define THRESHOLD_BATTERY_OFF		18000	//[mV] En dessous cette valeur, on consid�re que la puissance est absente
#define THRESHOLD_BATTERY_LOW		21300	//[mV] R�glage du seuil de batterie faible
#define THRESHOLD_12V_HOKUYO_MIN	10000	//[mV] R�glage du seuil de la tension minimum hokuyo
#define THRESHOLD_12V_HOKUYO_MAX	14000	//[mV] R�glage du seuil de la tension maximum hokuyo
#define GAP_BETWEEN_ARU				1500	//[mV] Ecart entre le 24V permanent et la puissance(d�rri�re l'ARU)
#define ACQUISITION					200		//[ms] Faire une acquisition de la batterie tous les..

static Uint32 valuePerm;
static alim_state_e aru_state, battery_state, hokuyo_state;
volatile bool_e flag_200ms = FALSE;

static void send_msgCAN(Uint8 state);

void VOLTAGE_MEASURE_init(){
	ADC_init();
	valuePerm = THRESHOLD_BATTERY_LOW + 1000;

	battery_state = BATTERY_ENABLE;
	aru_state = ARU_DISABLE;  //ARU non enclench� => disable
	hokuyo_state = HOKUYO_ENABLE;

	valuePerm = valuePerm*(100-PERCENTAGE_FILTER)/100 + VOLTAGE_MEASURE_measure24_mV(ADC_24_PERMANENCE)*PERCENTAGE_FILTER/100;
	send_msgCAN(BATTERY_ENABLE | ARU_DISABLE | HOKUYO_ENABLE);
}

void VOLTAGE_MEASURE_process_main(void){
	Uint32 valuePcse;
	Uint32 valueHokuyo;

	if(flag_200ms){
		flag_200ms = FALSE;
		valuePcse = VOLTAGE_MEASURE_measure24_mV(ADC_24_PUISSANCE);
		valueHokuyo = VOLTAGE_MEASURE_measure24_mV(ADC_12_HOKUYO);
		valuePerm = valuePerm*(100-PERCENTAGE_FILTER)/100 + VOLTAGE_MEASURE_measure24_mV(ADC_24_PERMANENCE)*PERCENTAGE_FILTER/100;

		if(valuePerm < THRESHOLD_BATTERY_OFF && battery_state != BATTERY_DISABLE){
			send_msgCAN(BATTERY_DISABLE);
			battery_state = BATTERY_DISABLE;
		}

		if(valuePerm < THRESHOLD_BATTERY_LOW && battery_state != BATTERY_LOW){
			send_msgCAN(BATTERY_LOW);
			battery_state = BATTERY_LOW;
		}

		if(valuePcse < valuePerm - GAP_BETWEEN_ARU && aru_state != ARU_ENABLE){ // L'ARU vient d'�tre enfonc�, plus de puissance
			send_msgCAN(ARU_ENABLE);
			aru_state = ARU_ENABLE;
		}else if(valuePcse > valuePerm - GAP_BETWEEN_ARU && aru_state != ARU_DISABLE){ // L'ARU vient d'�tre rel�ch�, retour de la puissance
			send_msgCAN(ARU_DISABLE);
			aru_state = ARU_DISABLE;
		}

		if((valueHokuyo < THRESHOLD_12V_HOKUYO_MIN || valueHokuyo > THRESHOLD_12V_HOKUYO_MAX) && hokuyo_state != HOKUYO_DISABLE){
			send_msgCAN(HOKUYO_DISABLE);
			hokuyo_state = HOKUYO_DISABLE;
		}else if((valueHokuyo > THRESHOLD_12V_HOKUYO_MIN && valueHokuyo < THRESHOLD_12V_HOKUYO_MAX) && hokuyo_state != HOKUYO_ENABLE){
			send_msgCAN(HOKUYO_ENABLE);
			hokuyo_state = HOKUYO_ENABLE;
		}
	}
}


void VOLTAGE_MEASURE_process_it(Uint8 ms){
	static Uint16 time = ACQUISITION;
	// Regarde, s'il doit faire une acquisition ou bien passer son chemin
	if(time > ms){
		time -= (Uint8)(ms);
	}
	else{
		time = ACQUISITION;
		flag_200ms = TRUE;
	}
}

Uint16 VOLTAGE_MEASURE_measure24_mV(adc_id_e id){
	Uint32 measure = (Uint32)ADC_getValue(id);
	return (Uint16)((measure * 21500)/600);	//21500 [mV] correspond � 600 [ADC]
}

static void send_msgCAN(Uint8 state){
	CAN_msg_t msg;
	msg.sid = BROADCAST_ALIM;
	msg.size = SIZE_BROADCAST_ALIM;
	msg.data.broadcast_alim.battery_value = valuePerm;
	msg.data.broadcast_alim.state = state;
	CAN_send(&msg);
}

bool_e VOLTAGE_MEASURE_get_state_hokuyo(void){
	return (hokuyo_state == HOKUYO_ENABLE);
}

bool_e VOLTAGE_MEASURE_get_state_battery(void){
	return (battery_state != BATTERY_LOW);
}

bool_e VOLTAGE_MEASURE_get_state_ARU(void){
	return (aru_state == ARU_ENABLE);
}
