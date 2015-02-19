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

#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_outputlog.h"

#define SWITCH_ID 0x7F

// Il y a deux types de switch ceux dont l'information sera envoyé vers l'extérieure
static Uint32 switchs;

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
	Uint32 switchs_edge; //état des switchs
	Uint8 i;


	// Gère tous les switchs qui envoient un message vers le monde extérieure
	switchs_pressed = 0;
#ifdef BIROUTE_PORT
	switchs_pressed |= (!BIROUTE_PORT)? (1<<BIROUTE_IHM) : 0; // Le laisse là, afin de le mettre à jour dans la variable switchs, si on vient à la demander plus tard (IHM_get..)
#endif
#ifdef SWITCH_COLOR_PORT
	switchs_pressed |= (SWITCH_COLOR_PORT)?  (1<<SWITCH_COLOR_IHM) : 0;
#endif
#ifdef SWITCH_LCD_PORT
	switchs_pressed |= (SWITCH_LCD_PORT)?  (1<<SWITCH_LCD_IHM) : 0;
#endif
#ifdef SWITCH0_PORT
	switchs_pressed |= (SWITCH0_PORT)?  (1<<SWITCH0_IHM) : 0;
#endif
#ifdef SWITCH1_PORT
	switchs_pressed |= (SWITCH1_PORT)?  (1<<SWITCH1_IHM) : 0;
#endif
#ifdef SWITCH2_PORT
	switchs_pressed |= (SWITCH2_PORT)?  (1<<SWITCH2_IHM) : 0;
#endif
#ifdef SWITCH3_PORT
	switchs_pressed |= (SWITCH3_PORT)?  (1<<SWITCH3_IHM) : 0;
#endif
#ifdef SWITCH4_PORT
	switchs_pressed |= (SWITCH4_PORT)?  (1<<SWITCH4_IHM) : 0;
#endif
#ifdef SWITCH5_PORT
	switchs_pressed |= (SWITCH5_PORT)?  (1<<SWITCH5_IHM) : 0;
#endif
#ifdef SWITCH6_PORT
	switchs_pressed |= (SWITCH6_PORT)?  (1<<SWITCH6_IHM) : 0;
#endif
#ifdef SWITCH7_PORT
	switchs_pressed |= (SWITCH7_PORT)?  (1<<SWITCH7_IHM) : 0;
#endif
#ifdef SWITCH8_PORT
	switchs_pressed |= (SWITCH8_PORT)?  (1<<SWITCH8_IHM) : 0;
#endif
#ifdef SWITCH9_PORT
	switchs_pressed |= (SWITCH9_PORT)?  (1<<SWITCH9_IHM) : 0;
#endif
#ifdef SWITCH10_PORT
	switchs_pressed |= (SWITCH10_PORT)?  (1<<SWITCH10_IHM) : 0;
#endif
#ifdef SWITCH11_PORT
	switchs_pressed |= (SWITCH11_PORT)?  (1<<SWITCH11_IHM) : 0;
#endif
#ifdef SWITCH12_PORT
	switchs_pressed |= (SWITCH12_PORT)?  (1<<SWITCH12_IHM) : 0;
#endif
#ifdef SWITCH13_PORT
	switchs_pressed |= (SWITCH13_PORT)?  (1<<SWITCH13_IHM) : 0;
#endif
#ifdef SWITCH14_PORT
	switchs_pressed |= (SWITCH14_PORT)?  (1<<SWITCH14_IHM) : 0;
#endif
#ifdef SWITCH15_PORT
	switchs_pressed |= (SWITCH15_PORT)?  (1<<SWITCH15_IHM) : 0;
#endif
#ifdef SWITCH16_PORT
	switchs_pressed |= (SWITCH16_PORT)?  (1<<SWITCH16_IHM) : 0;
#endif
#ifdef SWITCH17_PORT
	switchs_pressed |= (SWITCH17_PORT)?  (1<<SWITCH17_IHM) : 0;
#endif
#ifdef SWITCH18_PORT
	switchs_pressed |= (SWITCH18_PORT)?  (1<<SWITCH18_IHM) : 0;
#endif

	//détection des fronts
	switchs_edge = switchs_were_pressed ^ switchs_pressed;
	switchs_were_pressed = switchs = switchs_pressed;

	for(i=0;i<SWITCHS_NUMBER_IHM;i++){
		// Mets à jour le tableau pour une éventuelle demande extérieure

		if((switchs_edge & (1<<i)) && initialized && i != BIROUTE_IHM)// N'envoie pas, l'information si n'est pas encore init ou si c'est la biroute
			SWITCHS_send_msg(i);
	}
}

void SWITCHS_biroute_update(){
#ifdef BIROUTE_PORT
	static Uint8 current_biroute = 0;

	current_biroute = (current_biroute << 1) | BIROUTE_PORT;

	if((current_biroute & 0b111) == 0b100)	// Biroute vient d'être retirée (deux 0 de suite !)
	{
		CAN_msg_t msg;
		msg.size = 1;
		msg.sid = IHM_SWITCH;
		msg.data[0] = ((Uint8)BIROUTE_IHM & SWITCH_ID);
		CAN_send(&msg);
	}
#endif
}

void SWITCHS_send_msg(switch_ihm_e switch_id){
	CAN_msg_t msg;
	msg.size = 1;
	msg.sid = IHM_SWITCH;
	msg.data[0] = (((Uint8)(SWITCHS_get(switch_id))<< 7) | ((Uint8)switch_id & SWITCH_ID));
	CAN_send(&msg);
}

bool_e SWITCHS_get(switch_ihm_e switch_id){
	return (switchs >> switch_id) & 1;
}

void SWITCHS_answer(CAN_msg_t *send){
	CAN_msg_t msg;
	Uint8 i;

	for(i = 0; i < send->size; ++i)
		msg.data[i] = (((Uint8)(SWITCHS_get(send->data[i]))<< 7) | (send->data[i] & SWITCH_ID));

	msg.sid = IHM_SWITCH;
	msg.size = send->size;
	CAN_send(&msg);
}

void SWITCHS_send_all(){
	CAN_msg_t msg;
	msg.sid = IHM_SWITCH_ALL;
	msg.size = 4;
	msg.data[0] = (Uint8)(switchs >> 24);
	msg.data[1] = (Uint8)(switchs >> 16);
	msg.data[2] = (Uint8)(switchs >> 8);
	msg.data[3] = (Uint8)(switchs);
	CAN_send(&msg);
}
