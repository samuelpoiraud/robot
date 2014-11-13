/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : switch.c
 *	Package : Carte IHM
 *	Description : Contrôle les switchs
 *	Auteur : Anthony
 *	Version 2012/01/14
 */

#include "switch.h"

#include "config/config_global.h"
#include "QS/QS_types.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_outputlog.h"

#define SWITCH_ID 0b01111111

// Il y a deux types de switch ceux dont l'information sera envoyé vers l'extérieure
static bool_e switchs[SWITCHS_NUMBER_IHM];

// Au démarrage, n'envoyera pas de messsage CAN sur l'état des switchs
static bool_e initialized = FALSE;

void SWITCHS_init(){
	if(initialized)
		return;

	SWITCHS_update();

	initialized = TRUE;
}

void SWITCHS_update(){
	static Uint32 switchs_were_pressed = 0x00000000; //état des switchs dans le passage précédent
	Uint32 switchs_pressed = 0x00000000; //état des switchs
	Uint32 switchs_rising_edge; //état des switchs
	Uint8 i;


	// Gère tous les switchs qui envoient un message vers le monde extérieure
	switchs_pressed = 0;
#ifdef SWITCH_COLOR
	switchs_pressed = (SWITCH_COLOR)? switchs_pressed|1 : switchs_pressed;
#endif
#ifdef SWITCH_LCD_PORT
	switchs_pressed = (SWITCH_LCD_PORT)? switchs_pressed|(1<<1) : switchs_pressed;
#endif
#ifdef SWITCH0_PORT
	switchs_pressed = (SWITCH0_PORT)? switchs_pressed|(1<<2) : switchs_pressed;
#endif
#ifdef SWITCH1_PORT
	switchs_pressed = (SWITCH1_PORT)? switchs_pressed|(1<<3) : switchs_pressed;
#endif
#ifdef SWITCH2_PORT
	switchs_pressed = (SWITCH2_PORT)? switchs_pressed|(1<<4) : switchs_pressed;
#endif
#ifdef SWITCH3_PORT
	switchs_pressed = (SWITCH3_PORT)? switchs_pressed|(1<<5) : switchs_pressed;
#endif
#ifdef SWITCH4_PORT
	switchs_pressed = (SWITCH4_PORT)? switchs_pressed|(1<<6) : switchs_pressed;
#endif
#ifdef SWITCH5_PORT
	switchs_pressed = (SWITCH5_PORT)? switchs_pressed|(1<<7) : switchs_pressed;
#endif
#ifdef SWITCH6_PORT
	switchs_pressed = (SWITCH6_PORT)? switchs_pressed|(1<<8) : switchs_pressed;
#endif
#ifdef SWITCH7_PORT
	switchs_pressed = (SWITCH7_PORT)? switchs_pressed|(1<<9) : switchs_pressed;
#endif
#ifdef SWITCH8_PORT
	switchs_pressed = (SWITCH8_PORT)? switchs_pressed|(1<<10) : switchs_pressed;
#endif
#ifdef SWITCH9_PORT
	switchs_pressed = (SWITCH9_PORT)? switchs_pressed|(1<<11) : switchs_pressed;
#endif
#ifdef SWITCH10_PORT
	switchs_pressed = (SWITCH10_PORT)? switchs_pressed|(1<<12) : switchs_pressed;
#endif
#ifdef SWITCH11_PORT
	switchs_pressed = (SWITCH11_PORT)? switchs_pressed|(1<<13) : switchs_pressed;
#endif
#ifdef SWITCH12_PORT
	switchs_pressed = (SWITCH12_PORT)? switchs_pressed|(1<<14) : switchs_pressed;
#endif
#ifdef SWITCH13_PORT
	switchs_pressed = (SWITCH13_PORT)? switchs_pressed|(1<<15) : switchs_pressed;
#endif
#ifdef SWITCH14_PORT
	switchs_pressed = (SWITCH14_PORT)? switchs_pressed|(1<<16) : switchs_pressed;
#endif
#ifdef SWITCH15_PORT
	switchs_pressed = (SWITCH15_PORT)? switchs_pressed|(1<<17) : switchs_pressed;
#endif
#ifdef SWITCH16_PORT
	switchs_pressed = (SWITCH16_PORT)? switchs_pressed|(1<<18) : switchs_pressed;
#endif
#ifdef SWITCH17_PORT
	switchs_pressed = (SWITCH17_PORT)? switchs_pressed|(1<<19) : switchs_pressed;
#endif
#ifdef SWITCH18_PORT
	switchs_pressed = (SWITCH18_PORT)? switchs_pressed|(1<<20) : switchs_pressed;
#endif

	//détection des fronts montant
	switchs_rising_edge = (~switchs_were_pressed) & switchs_pressed;

	for(i=0;i<SWITCHS_NUMBER_IHM;i++){
		// Mets à jour le tableau pour une éventuelle demande extérieure
		switchs[i] = switchs_rising_edge & (1<<i);

		if((switchs_rising_edge & (1<<i)) && initialized){// N'envoie pas, l'information si n'est pas encore init
			SWITCHS_send_msg(i);

#ifdef VERBOSE_ELEMENT
//			if(BIROUTE_IHM == i)	{debug_printf("sw_color = %s\n",	(BIROUTE_PORT)?	"ON":"OFF");}
//			else if(SWITCH_COLOR_IHM == i)	{debug_printf("sw_color = %s\n",	(SWITCH_COLOR_PORT)?	"ON":"OFF");}
//			else if(SWITCH_LCD_IHM 	== i)	{debug_printf("sw_lcd = %s\n",	 (SWITCH_LCD_PORT)?	"ON":"OFF");}
//			else debug_printf("SWITCH %d : State %s\n",i,(switchs[i])?"ON":"OFF");
#endif
		}
	}

	switchs_were_pressed = switchs_pressed;
}

void SWITCHS_send_msg(switch_ihm_e switch_id){
	CAN_msg_t msg;
	msg.size = 1;
	msg.sid = IHM_SWITCH;
	msg.data[0] = (((Uint8)(switchs[switch_id])<< 7) | ((Uint8)switch_id & SWITCH_ID));
	CAN_send(&msg);
}

bool_e SWITCHS_get(switch_ihm_e switch_id){
	return switchs[switch_id];
}

void SWITCHS_answer(CAN_msg_t *send){
	CAN_msg_t msg;
	Uint8 i;

	for(i = 0; i < send->size; ++i) {
		msg.data[i] = (((Uint8)(switchs[send->data[i]])<< 7) | (send->data[i] & SWITCH_ID));
	}

	msg.sid = IHM_SWITCH;
	msg.size = send->size;
	CAN_send(&msg);
}

void SWITCHS_send_all(){
	Uint32 data = 0x00000000;
	Uint8 i;

	for (i = 0; i < SWITCHS_NUMBER_IHM; ++i)
		data |= ((switchs[i] & 1) << i);

	CAN_msg_t msg;
	msg.sid = IHM_SWITCH_ALL;
	msg.size = 4;
	msg.data[0] = (Uint8)(data >> 24);
	msg.data[1] = (Uint8)(data >> 16);
	msg.data[2] = (Uint8)(data >> 8);
	msg.data[3] = (Uint8)(data);
	CAN_send(&msg);
}
