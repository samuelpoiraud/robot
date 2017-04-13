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
#include "QS/QS_types.h"
#include "QS/QS_outputlog.h"

#define PERCENTAGE_FILTER				10
#define THRESHOLD_STARTUP_RESISTOR_OFF	16000	//[mV]
#define THRESHOLD_STARTUP_RESISTOR_ON	18000	//[mV]
#define THRESHOLD_BATTERY_OFF			18000	//[mV] En dessous cette valeur, on considère que la puissance est absente
#define THRESHOLD_BATTERY_LOW			21300	//[mV] Réglage du seuil de batterie faible
#define THRESHOLD_12V_HOKUYO_MIN		10000	//[mV] Réglage du seuil de la tension minimum hokuyo
#define THRESHOLD_12V_HOKUYO_MAX		14000	//[mV] Réglage du seuil de la tension maximum hokuyo
#define GAP_BETWEEN_ARU					1500	//[mV] Ecart entre le 24V permanent et la puissance(dérrière l'ARU)
#define ACQUISITION						200		//[ms] Faire une acquisition de la batterie tous les..

static Uint32 valuePerm;
static alim_state_e aru_state, battery_state, hokuyo_state, power_state;
static alim_state_e last_aru_state, last_battery_state, last_hokuyo_state, last_power_state;
volatile bool_e flag_200ms = FALSE;

static void send_msgCAN(Uint8 state);
static void VOLTAGE_MEASURE_setStartupResistorState(bool_e state);

void VOLTAGE_MEASURE_init(){
	ADC_init();
	valuePerm = THRESHOLD_BATTERY_LOW + 1000;

	battery_state = BATTERY_DISABLE;
	last_battery_state = BATTERY_DISABLE;

	aru_state = ARU_ENABLE;  //ARU enclenché => enable
	last_aru_state = ARU_ENABLE;  //ARU enclenché => enable

	hokuyo_state = HOKUYO_DISABLE;
	last_hokuyo_state = HOKUYO_DISABLE;

	valuePerm = valuePerm*(100-PERCENTAGE_FILTER)/100 + VOLTAGE_MEASURE_measure24_mV(ADC_24_PERMANENCE)*PERCENTAGE_FILTER/100;
	send_msgCAN(battery_state | aru_state | hokuyo_state);

	VOLTAGE_MEASURE_setStartupResistorState(TRUE);
}

void VOLTAGE_MEASURE_process_main(void){
	Uint32 valuePcse;
	Uint32 valueHokuyo;

	if(flag_200ms){
		flag_200ms = FALSE;

		global.voltage.Vpuissance = valuePcse = VOLTAGE_MEASURE_measure24_mV(ADC_24_PUISSANCE);
		global.voltage.Vhokuyo = valueHokuyo = VOLTAGE_MEASURE_measure24_mV(ADC_12_HOKUYO);
		global.voltage.Vpermanent = valuePerm = valuePerm*(100-PERCENTAGE_FILTER)/100 + VOLTAGE_MEASURE_measure24_mV(ADC_24_PERMANENCE)*PERCENTAGE_FILTER/100;

		if(valuePerm < THRESHOLD_BATTERY_OFF)
			battery_state = BATTERY_DISABLE;
		else if(valuePerm < THRESHOLD_BATTERY_LOW)
			battery_state = BATTERY_LOW;
		else
			battery_state = BATTERY_ENABLE;


		if(battery_state != last_battery_state){
			last_battery_state = battery_state;
			send_msgCAN(battery_state);
		}

		if(valuePcse < valuePerm - GAP_BETWEEN_ARU) // L'ARU vient d'être enfoncé, plus de puissance
			aru_state = ARU_ENABLE;
		else // L'ARU vient d'être relâché, retour de la puissance
			aru_state = ARU_DISABLE;

		if(aru_state != last_aru_state){
			last_aru_state = aru_state;
			send_msgCAN(aru_state);
		}

		if(valuePcse > THRESHOLD_STARTUP_RESISTOR_ON){
			power_state = POWER_AVAILABLE;
			VOLTAGE_MEASURE_setStartupResistorState(FALSE);
		}else if(valuePcse < THRESHOLD_STARTUP_RESISTOR_OFF){
			power_state = POWER_NO_AVAILABLE;
			VOLTAGE_MEASURE_setStartupResistorState(TRUE);
		}

		if(power_state != last_power_state){
			last_power_state = power_state;
			send_msgCAN(power_state);
		}

		if(valueHokuyo > THRESHOLD_12V_HOKUYO_MIN && valueHokuyo < THRESHOLD_12V_HOKUYO_MAX)
			hokuyo_state = HOKUYO_ENABLE;
		else
			hokuyo_state = HOKUYO_DISABLE;

		if(hokuyo_state != last_hokuyo_state){
			last_hokuyo_state = hokuyo_state;
			send_msgCAN(hokuyo_state);
		}
	}
}
static void VOLTAGE_MEASURE_setStartupResistorState(bool_e state){
	static bool_e lastStartupResistorState = FALSE;

	if(lastStartupResistorState != state){
		lastStartupResistorState = state;
		if(state)
			GPIO_ResetBits(RELAY_RESISTOR);
		else
			GPIO_SetBits(RELAY_RESISTOR);
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
	return (Uint16)((measure * 21500)/600);	//21500 [mV] correspond à 600 [ADC]
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
