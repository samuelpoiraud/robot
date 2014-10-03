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

#define ADC_24_PUISSANCE	ADC_6
#define ADC_24_PERMANENCE	ADC_10

#define PERCENTAGE_FILTER 10
#define THRESHOLD_BATTERY_OFF	18000	//[mV] En dessous cette valeur, on considère que la puissance est absente
#define THRESHOLD_BATTERY_LOW	21300	//[mV] Réglage du seuil de batterie faible
#define GAP_BETWEEN_ARU			1000	//[mV] Ecart entre le 24V permanent et la puissance(dérrière l'ARU)
#define ACQUISITION				200		//[ms] Faire une acquisition de la batterie tous les..

static Uint32 valuePerm;
static bool_e ARU_enable;


void send_msgCAN(IHM_power_e state);

void VOLTAGE_MEASURE_init(){
	ADC_init();

	valuePerm = VOLTAGE_MEASURE_measure24_mV(ADC_24_PERMANENCE);

	if(VOLTAGE_MEASURE_measure24_mV(ADC_24_PUISSANCE) < valuePerm - GAP_BETWEEN_ARU){ // ARU, enfoncé
		send_msgCAN(ARU_ENABLE);
		ARU_enable = TRUE;
	}else{
		send_msgCAN(ARU_DISABLE);
		ARU_enable = FALSE;
	}
}

void VOLTAGE_MEASURE_process_it(Uint8 ms){
	static Uint8 time = ACQUISITION;

	// Regarde si il doit faire une acquisition ou bien passer son chemin
	if(time < ms){
		time = ACQUISITION;
		return;
	}
	time -= (Uint8)(ms);

	Uint32 valuePcse = VOLTAGE_MEASURE_measure24_mV(ADC_24_PUISSANCE);
	valuePerm = valuePerm*(100-PERCENTAGE_FILTER) + VOLTAGE_MEASURE_measure24_mV(ADC_24_PERMANENCE)*PERCENTAGE_FILTER;


	if(valuePerm < THRESHOLD_BATTERY_OFF)
		send_msgCAN(BATTERY_OFF);
	else if(valuePerm < THRESHOLD_BATTERY_LOW)
		send_msgCAN(BATTERY_LOW);

	if(valuePcse < valuePerm - GAP_BETWEEN_ARU && !ARU_enable){ // L'ARU vient d'être enfoncé, plus de puissance
		send_msgCAN(ARU_ENABLE);
		ARU_enable = TRUE;
	}else if(valuePcse > valuePerm - GAP_BETWEEN_ARU && ARU_enable){ // L'ARU vient d'être relâché, retour de la puissance
		send_msgCAN(ARU_DISABLE);
		ARU_enable = FALSE;
	}
}

Uint16 VOLTAGE_MEASURE_measure24_mV(adc_id_e id){
	Uint32 measure = (Uint32)ADC_getValue(id);
	return (Uint16)((measure * 3000*110/10)/1024);	//3000 [mV] correspond à 4096 [ADC]
}

void send_msgCAN(IHM_power_e state){
	CAN_msg_t msg;
	msg.sid = IHM_POWER;
	msg.data[0] = state;
	msg.size = 1;
	CAN_send(&msg);
}
