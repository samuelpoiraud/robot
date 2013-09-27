/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : QS_CapteurCouleurCW.c
 *	Package : Qualité Soft
 *	Description : Driver du capteur couleur Tri-tronics ColorWise
 *  Auteurs : Alexis
 *  Version 20130207
 */

#include "QS_CapteurCouleurCW.h"
#include "QS_adc.h"

#ifdef USE_CW_SENSOR

#define CW_PORTBIT_SET(p) BIT_SET(*(p).port, (p).bit_number)
#define CW_PORTBIT_CLR(p) BIT_CLR(*(p).port, (p).bit_number)
#define CW_PORTBIT_TST(p) BIT_TEST(*(p).port, (p).bit_number)

typedef struct {
	//Uint8 current_remote_capture_channel;	// >0 quand on est en train d'envoyer un remote capture; non utilisé/implémenté
	CW_config_t config;
} CW_sensor_data_t;

CW_sensor_data_t CW_sensors[CW_SENSOR_NUMBER];

static bool_e CW_PORT_get(CW_port_t port);
static void CW_PORT_set(CW_port_t port, bool_e state);

void CW_init() {
	static bool_e initialized = FALSE;
	int i, j;

	if(initialized)
		return;
	initialized = TRUE;

	ADC_init();

	for(i=0; i < CW_SENSOR_NUMBER; i++) {
		CW_sensors[i].config.analog_X = CW_UNUSED_ANALOG_PORT;
		CW_sensors[i].config.analog_Y = CW_UNUSED_ANALOG_PORT;
		CW_sensors[i].config.analog_Z = CW_UNUSED_ANALOG_PORT;
		for(j=0; j < CW_PP_MAXPORTNUM; j++) {
			CW_sensors[i].config.digital_ports[j].port = CW_UNUSED_PORT;
			CW_sensors[i].config.digital_ports[j].bit_number = 0;
		}
	}
}

void CW_config_sensor(Uint8 id_sensor, CW_config_t* config) {
	assert(id_sensor < CW_PP_MAXPORTNUM);
	CW_sensors[id_sensor].config = *config;
}

bool_e CW_is_color_detected(Uint8 id_sensor, Uint8 canal) {
	bool_e value;
	int i;

	assert(id_sensor < CW_PP_MAXPORTNUM);
	assert(canal < 4);

	if(CW_sensors[id_sensor].config.digital_ports[canal].port == CW_UNUSED_PORT)
		return FALSE;

	//CW_PORTBIT_SET(CW_sensors[id_sensor].config.digital_ports[CW_PP_Gate]);
	CW_PORT_set(CW_sensors[id_sensor].config.digital_ports[CW_PP_Gate], TRUE);
	for(i=10000;i>0;i--);	//attente temps de réponse de 1ms~ du capteur
	//value = CW_PORTBIT_TST(CW_sensors[id_sensor].config.digital_ports[canal]);
	value = CW_PORT_get(CW_sensors[id_sensor].config.digital_ports[canal]);
	//CW_PORTBIT_CLR(CW_sensors[id_sensor].config.digital_ports[CW_PP_Gate]);
	CW_PORT_set(CW_sensors[id_sensor].config.digital_ports[CW_PP_Gate], FALSE);

	return value;
}

Uint16 CW_get_color_intensity(Uint8 id_sensor, CW_analog_color_e composante) {
	assert(id_sensor < CW_PP_MAXPORTNUM);

	switch(composante) {
		case CW_AC_XYZ_X:
			if(CW_sensors[id_sensor].config.analog_X != CW_UNUSED_ANALOG_PORT)
				return ADC_getValue(CW_sensors[id_sensor].config.analog_X);
			break;

		case CW_AC_XYZ_Y:
			if(CW_sensors[id_sensor].config.analog_Y != CW_UNUSED_ANALOG_PORT)
				 return ADC_getValue(CW_sensors[id_sensor].config.analog_Y);
			break;

		case CW_AC_XYZ_Z:
			if(CW_sensors[id_sensor].config.analog_Z != CW_UNUSED_ANALOG_PORT)
				 return ADC_getValue(CW_sensors[id_sensor].config.analog_Z);
			break;
	}

	debug_printf("CW_get_color_intensity: No ADC\n");
	return 0;
}

#ifdef CW_REMOTE_CONTROL_TIMER
void CW_set_channel_color(Uint8 id_sensor, Uint8 canal) {
	//Non implémenté
	#error "Le support de CW_REMOTE_CONTROL_TIMER n'est pas implémenté."
}
#endif

static bool_e CW_PORT_get(CW_port_t port) {
	if(port.port == CW_UNUSED_PORT)
		return FALSE;
	return CW_PORTBIT_TST(port) ^ port.is_inverted_logic;
}

static void CW_PORT_set(CW_port_t port, bool_e state) {
	if(port.port != CW_UNUSED_PORT) {
		if(state ^ port.is_inverted_logic)
			CW_PORTBIT_SET(port);
		else CW_PORTBIT_CLR(port);
	}
}

#endif //USE_CW_SENSOR
