/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : voltage_measure.c
 *	Package : Carte IHM
 *	Description : Contrôle de la tension des batteries
 *	Auteur : Anthony / Arnaud
 *	Version 2012/01/14
 */

#include "voltage_measure.h"
#include "QS/QS_adc.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_IHM.h"
#include "QS/QS_can.h"

#define PERCENTAGE_FILTER			10
#define THRESHOLD_BATTERY_OFF		18000	//[mV] En dessous cette valeur, on considère que la puissance est absente
#define THRESHOLD_BATTERY_LOW		21300	//[mV] Réglage du seuil de batterie faible
#define THRESHOLD_12V_HOKUYO_MIN	10000	//[mV] Réglage du seuil de la tension minimum hokuyo
#define THRESHOLD_12V_HOKUYO_MAX	14000	//[mV] Réglage du seuil de la tension maximum hokuyo
#define GAP_BETWEEN_ARU				1000	//[mV] Ecart entre le 24V permanent et la puissance(dérrière l'ARU)
#define ACQUISITION					200		//[ms] Faire une acquisition de la batterie tous les..

static Uint32 valuePerm;
static bool_e ARU_enable,BATTERY_Low,HOKUYO_enable;


static void send_msgCAN(IHM_power_e state);

void VOLTAGE_MEASURE_init(){
	ADC_init();
	Uint32 valuePcse = VOLTAGE_MEASURE_measure24_mV(ADC_24_PUISSANCE);
	Uint32 valueHokuyo = VOLTAGE_MEASURE_measure24_mV(ADC_12_HOKUYO);
	valuePerm = VOLTAGE_MEASURE_measure24_mV(ADC_24_PERMANENCE);


	if(valuePerm < THRESHOLD_BATTERY_OFF){
		send_msgCAN(BATTERY_OFF);
		BATTERY_Low = TRUE;
	}
	else if(valuePerm < THRESHOLD_BATTERY_LOW){
		send_msgCAN(BATTERY_LOW);
		BATTERY_Low = TRUE;
	}else
		BATTERY_Low = FALSE;


	if(valuePcse < valuePerm - GAP_BETWEEN_ARU){ // ARU, enfoncé
		send_msgCAN(ARU_ENABLE);
		ARU_enable = TRUE;
	}else{
		send_msgCAN(ARU_DISABLE);
		ARU_enable = FALSE;
	}

	if(valueHokuyo < THRESHOLD_12V_HOKUYO_MIN || valueHokuyo > THRESHOLD_12V_HOKUYO_MAX){
		send_msgCAN(HOKUYO_POWER_FAIL);
		HOKUYO_enable = FALSE;
	}else
		HOKUYO_enable = TRUE;
}

void VOLTAGE_MEASURE_process_it(Uint8 ms){
	static Uint8 time = ACQUISITION;

	// Regarde, s'il doit faire une acquisition ou bien passer son chemin
	if(time > ms){
		time -= (Uint8)(ms);
		return;
	}

	time = ACQUISITION;

	Uint32 valuePcse = VOLTAGE_MEASURE_measure24_mV(ADC_24_PUISSANCE);
	Uint32 valueHokuyo = VOLTAGE_MEASURE_measure24_mV(ADC_12_HOKUYO);

	valuePerm = valuePerm*(100-PERCENTAGE_FILTER)/100 + VOLTAGE_MEASURE_measure24_mV(ADC_24_PERMANENCE)*PERCENTAGE_FILTER/100;


	if(valuePerm < THRESHOLD_BATTERY_OFF && !BATTERY_Low){
		send_msgCAN(BATTERY_OFF);
		BATTERY_Low = TRUE;
	}else if(valuePerm < THRESHOLD_BATTERY_LOW && !BATTERY_Low){
		send_msgCAN(BATTERY_LOW);
		BATTERY_Low = TRUE;
	}


	if(valuePcse < valuePerm - GAP_BETWEEN_ARU && !ARU_enable){ // L'ARU vient d'être enfoncé, plus de puissance
		send_msgCAN(ARU_ENABLE);
		ARU_enable = TRUE;
	}else if(valuePcse > valuePerm - GAP_BETWEEN_ARU && ARU_enable){ // L'ARU vient d'être relâché, retour de la puissance
		send_msgCAN(ARU_DISABLE);
		ARU_enable = FALSE;
	}


	if((valueHokuyo < THRESHOLD_12V_HOKUYO_MIN || valueHokuyo > THRESHOLD_12V_HOKUYO_MAX)){
		if(HOKUYO_enable)
			send_msgCAN(HOKUYO_POWER_FAIL);
		HOKUYO_enable = FALSE;
	}else
		HOKUYO_enable = TRUE;
}

Uint16 VOLTAGE_MEASURE_measure24_mV(adc_id_e id){
	Uint32 measure = (Uint32)ADC_getValue(id);
	return (Uint16)((measure * 3000*110/10)/1024);	//3000 [mV] correspond à 4096 [ADC]
}

static void send_msgCAN(IHM_power_e state){
	CAN_msg_t msg;
	msg.sid = IHM_POWER;
	msg.data[0] = state;
	msg.size = 1;
	CAN_send(&msg);
}
