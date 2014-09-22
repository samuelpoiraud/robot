#include "switch.h"

#include "_IHM_config.h"
#include "QS/QS_types.h"
#include "QS/QS_buttons.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_outputlog.h"

// Il y a deux types de switch ceux dont l'information sera envoyé vers l'extérieure
static bool_e switchs[SWITCHS_NUMBER];
// Et les switchs internes
static switch_action_t switchs_int[SW_INT_NUMBER];


void SWITCHS_init(){
	Uint8 i;

	static bool_e initialized = FALSE;
	if(initialized)
		return;

	// Init des switchs internes
	for(i=0;i<SW_INT_NUMBER;i++){
		SWITCHS_INT_define_actions((switch_int_id_e)i,NULL);
	}
}


void SWITCHS_INT_define_actions(switch_int_id_e switch_id,switch_action_t changement){
	assert( switch_id<SW_INT_NUMBER && switch_id>=0);
	//switch_action_t* switch_int = &(switchs_int[switch_id]);
	switchs_int[switch_id] = changement;
}


void SWITCHS_update(){
	static Uint32 switchs_were_pressed = 0x0000; //état des switchs dans le passage précédent
	Uint32 switchs_pressed = 0x0000; //état des switchs
	Uint32 switchs_rising_edge; //état des switchs

	static Uint8 switchs_int_were_pressed = 0x00; //état des switchs dans le passage précédent
	Uint8 switchs_int_pressed = 0x00; //état des switchs
	Uint8 switchs_int_rising_edge; //état des switchs
	Uint8 i;


	// Gère tous les switchs qui envoient un message vers le monde extérieure
	switchs_pressed = 0;
	#ifdef SWITCH0_PORT
		switchs_pressed = (SWITCH0_PORT)? switchs_pressed|1 : switchs_pressed;
	#endif
	#ifdef SWITCH1_PORT
		switchs_pressed = (SWITCH1_PORT)? switchs_pressed|(1<<1) : switchs_pressed;
	#endif
	#ifdef SWITCH2_PORT
		switchs_pressed = (SWITCH2_PORT)? switchs_pressed|(1<<2) : switchs_pressed;
	#endif
	#ifdef SWITCH3_PORT
		switchs_pressed = (SWITCH3_PORT)? switchs_pressed|(1<<3) : switchs_pressed;
	#endif
	#ifdef SWITCH4_PORT
		switchs_pressed = (SWITCH4_PORT)? switchs_pressed|(1<<4) : switchs_pressed;
	#endif
	#ifdef SWITCH5_PORT
		switchs_pressed = (SWITCH5_PORT)? switchs_pressed|(1<<5) : switchs_pressed;
	#endif
	#ifdef SWITCH6_PORT
		switchs_pressed = (SWITCH6_PORT)? switchs_pressed|(1<<6) : switchs_pressed;
	#endif
	#ifdef SWITCH7_PORT
		switchs_pressed = (SWITCH7_PORT)? switchs_pressed|(1<<7) : switchs_pressed;
	#endif
	#ifdef SWITCH8_PORT
		switchs_pressed = (SWITCH8_PORT)? switchs_pressed|(1<<8) : switchs_pressed;
	#endif
	#ifdef SWITCH9_PORT
		switchs_pressed = (SWITCH9_PORT)? switchs_pressed|(1<<9) : switchs_pressed;
	#endif
	#ifdef SWITCH10_PORT
		switchs_pressed = (SWITCH10_PORT)? switchs_pressed|(1<<10) : switchs_pressed;
	#endif
	#ifdef SWITCH11_PORT
		switchs_pressed = (SWITCH11_PORT)? switchs_pressed|(1<<11) : switchs_pressed;
	#endif
	#ifdef SWITCH12_PORT
		switchs_pressed = (SWITCH12_PORT)? switchs_pressed|(1<<12) : switchs_pressed;
	#endif
	#ifdef SWITCH13_PORT
		switchs_pressed = (SWITCH13_PORT)? switchs_pressed|(1<<13) : switchs_pressed;
	#endif
	#ifdef SWITCH14_PORT
		switchs_pressed = (SWITCH14_PORT)? switchs_pressed|(1<<14) : switchs_pressed;
	#endif
	#ifdef SWITCH15_PORT
		switchs_pressed = (SWITCH15_PORT)? switchs_pressed|(1<<15) : switchs_pressed;
	#endif
	#ifdef SWITCH16_PORT
		switchs_pressed = (SWITCH16_PORT)? switchs_pressed|(1<<16) : switchs_pressed;
	#endif
	#ifdef SWITCH17_PORT
		switchs_pressed = (SWITCH17_PORT)? switchs_pressed|(1<<17) : switchs_pressed;
	#endif
	#ifdef SWITCH18_PORT
		switchs_pressed = (SWITCH18_PORT)? switchs_pressed|(1<<18) : switchs_pressed;
	#endif

	//détection des fronts montant
	switchs_rising_edge = (~switchs_were_pressed) & switchs_pressed;

	for(i=0;i<SWITCHS_NUMBER;i++){
		// Mets à jour le tableau pour une éventuelle demande extérieure
		switchs[i] = switchs_rising_edge & (1<<i);

		if(switchs_rising_edge & (1<<i)){
			SWITCHS_send_msg(i);

		#ifdef USE_VERBOSE
			debug_printf("SWITCH %d : State %s",i,(switchs[i])?"UP":"DOWN");
		#endif
		}
	}

	switchs_were_pressed = switchs_pressed;


	// Switchs interne
	switchs_int_pressed = 0;
	#ifdef SWITCH_COLOR
		switchs_int_pressed = (SWITCH_COLOR)? switchs_int_pressed|1 : switchs_int_pressed;
	#endif
	#ifdef SWITCH_LCD_PORT
		switchs_int_pressed = (SWITCH_LCD_PORT)? switchs_int_pressed|2 : switchs_int_pressed;
	#endif

	//détection des fronts montant
	switchs_int_rising_edge = (~switchs_int_were_pressed) & switchs_int_pressed;

	for(i=0;i<SW_INT_NUMBER;i++){
		if((switchs_int_rising_edge & (1<<i)) && switchs_int[i] != NULL){
			(switchs_int[i])();

		#ifdef USE_VERBOSE
			if(i == 0)
				debug_printf("SWITCH COLOR : State %s",((switchs_int_rising_edge & (1<<i))?"UP":"DOWN"));
			else if(i == 1)
				debug_printf("SWITCH LCD : State %s",((switchs_int_rising_edge & (1<<i))?"UP":"DOWN"));
		#endif
		}
	}

	switchs_int_were_pressed = switchs_int_pressed;
}


void SWITCHS_send_msg(switch_id_e switch_id){
	CAN_msg_t msg;
	msg.size = 2;
	msg.sid = IHM_SWITCH;
	msg.data[0] = switch_id;
	msg.data[1] = switchs[switch_id];
	CAN_send(&msg);
}
