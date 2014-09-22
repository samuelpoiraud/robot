/*
 *	Club Robot ESEO 2014
 *
 *	Fichier : button.c
 *	Package : Carte IHM
 *	Description : Contrôle les boutons, code inspiré de QS_button
 *	Auteur : Anthony
 *	Version 2012/01/14
 */

#include "button.h"

#include "Global_config.h"
#include "QS/QS_types.h"
#include "QS/QS_buttons.h"
#include "QS/QS_can.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_outputlog.h"

// Temps pour les buttons externes pour un appuie long
#define TIME_LONG_PUSH 200


// Pour les buttons internes
static button_ihm_t buttons_int[BP_INT_NUMBER];
static Uint8 push_int_time[BP_INT_NUMBER]={0};

// Pour les buttons qui communiquent avec le monde extérieure
static bool_e buttons[BUTTONS_IHM_NUMBER]; // Pour détecter les appuies long
static Uint8 push_time[BUTTONS_IHM_NUMBER]={0};


void BUTTONS_IHM_init(){
	Uint8 i;

	static bool_e initialized = FALSE;
	if(initialized)
		return;

	// Init des buttons internes
	for(i=0;i<BP_INT_NUMBER;i++){
		BUTTONS_IHM_define_actions((button_int_id_e)i,NULL,NULL,0);
	}
}


void BUTTONS_IHM_define_actions(button_int_id_e button_id,button_ihm_action_t direct_push, button_ihm_action_t after_long_push, Uint8 long_push_time){
	assert(button_id<BP_INT_NUMBER && button_id>=0);
	button_ihm_t* button = &(buttons_int[button_id]);
	button->direct_push=direct_push;
	button->after_long_push=after_long_push;
	button->long_push_time=long_push_time*10;
}


void BUTTONS_IHM_update(){
	static Uint8 buttons_were_pressed = 0x00; //état des buttons dans le passage précédent
	Uint8 buttons_pressed = 0x00; //état des buttons
	Uint8 buttons_rising_edge; //état des buttons
	Uint8 buttons_falling_edge; //état des boutons

	static Uint8 buttons_int_were_pressed = 0x00; //état des buttons dans le passage précédent
	Uint8 buttons_int_pressed = 0x00; //état des buttons
	Uint8 buttons_int_rising_edge; //état des buttons
	Uint8 buttons_int_falling_edge; //état des boutons
	button_ihm_action_t action;
	button_ihm_t* button = NULL;
	Uint8 i;


	// Gère tous les buttons qui envoient un message vers le monde extérieure
	buttons_pressed = 0;
#ifdef BUTTON0_PORT
	buttons_pressed = (BUTTON0_PORT)? buttons_pressed|1 : buttons_pressed;
#endif
#ifdef BUTTON1_PORT
	buttons_pressed = (BUTTON1_PORT)? buttons_pressed|(1<<1) : buttons_pressed;
#endif
#ifdef BUTTON2_PORT
	buttons_pressed = (BUTTON2_PORT)? buttons_pressed|(1<<2) : buttons_pressed;
#endif
#ifdef BUTTON3_PORT
	buttons_pressed = (BUTTON3_PORT)? buttons_pressed|(1<<3) : buttons_pressed;
#endif
#ifdef BUTTON4_PORT
	buttons_pressed = (BUTTON4_PORT)? buttons_pressed|(1<<4) : buttons_pressed;
#endif
#ifdef BUTTON5_PORT
	buttons_pressed = (BUTTON5_PORT)? buttons_pressed|(1<<5) : buttons_pressed;
#endif


	//détection des fronts montant
	buttons_rising_edge = (~buttons_were_pressed) & buttons_pressed;

	for(i=0;i<BUTTONS_IHM_NUMBER;i++){
		if(buttons_rising_edge&(1<<i)){ // Détecte les fronts montant sur chaque bouton
			push_time[i] = TIME_LONG_PUSH;
			buttons[i] = FALSE;
		}
		else{
			buttons_falling_edge = buttons_were_pressed & (~buttons_pressed);

			if(buttons[i] == FALSE) {
				if((buttons_pressed & (1<<i)) && push_time[i] == 0){// long push
					buttons[i] = TRUE;
					BUTTONS_IHM_send_msg(i,TRUE);

					#ifdef VERBOSE_MODE
						debug_printf("Button %d active, long push",i);
					#endif
				}
				else if(buttons_falling_edge & (1<<i)){ // direct push
					BUTTONS_IHM_send_msg(i,FALSE);
					#ifdef VERBOSE_MODE
						debug_printf("Button %d active, direct push",i);
					#endif
				}
			}
		}
	}

	buttons_were_pressed = buttons_pressed;




	// Buttons interne
	buttons_int_pressed = 0;
#ifdef BUTTON_SELFTEST_PORT
	buttons_int_pressed = (BUTTON_SELFTEST_PORT)? buttons_int_pressed|1 : buttons_int_pressed;
#endif
#ifdef BUTTON_CALIBRATION_PORT
	buttons_int_pressed = (BUTTON_CALIBRATION_PORT)? buttons_int_pressed|2 : buttons_int_pressed;
#endif
#ifdef BUTTON_PRINTMATCH_PORT
	buttons_int_pressed = (BUTTON_PRINTMATCH_PORT)? buttons_int_pressed|1 : buttons_int_pressed;
#endif
#ifdef BUTTON_OK_PORT
	buttons_int_pressed = (BUTTON_OK_PORT)? buttons_int_pressed|2 : buttons_int_pressed;
#endif
#ifdef BUTTON_UP_PORT
	buttons_int_pressed = (BUTTON_UP_PORT)? buttons_int_pressed|1 : buttons_int_pressed;
#endif
#ifdef BUTTON_DOWN_PORT
	buttons_int_pressed = (BUTTON_DOWN_PORT)? buttons_int_pressed|2 : buttons_int_pressed;
#endif
#ifdef BUTTON_SET_PORT
	buttons_int_pressed = (BUTTON_SET_PORT)? buttons_int_pressed|1 : buttons_int_pressed;
#endif
#ifdef BUTTON_RFU_PORT
	buttons_int_pressed = (BUTTON_RFU_PORT)? buttons_int_pressed|2 : buttons_int_pressed;
#endif

	//détection des fronts montant
	buttons_int_rising_edge = (~buttons_int_were_pressed) & buttons_int_pressed;

	for(i=0;i<BP_INT_NUMBER;i++){
		button = &(buttons_int[i]);
		if(buttons_int_rising_edge&(1<<i))
		{
			push_int_time[i]=button->long_push_time;
			button->long_push_already_detected = FALSE;
		}
		else
		{
			buttons_int_falling_edge = buttons_int_were_pressed & (~buttons_int_pressed);

			if(button->long_push_already_detected == FALSE) {
				action = button->after_long_push;
				if((buttons_int_pressed & (1<<i)) && action != NULL && push_int_time[i] == 0)
				{
					button->long_push_already_detected = TRUE;
					(*action)();
				}
				else if(buttons_int_falling_edge & (1<<i))
				{
					action = button->direct_push;
					if(action != NULL)
					{
						(*action)();
					}
				}
			}
		}
	}

	buttons_int_were_pressed = buttons_int_pressed;
}

void BUTTONS_IHM_send_msg(button_ihm_e button_id,bool_e push_long){
	CAN_msg_t msg;
	msg.size = 2;
	msg.sid = IHM_BUTTON;
	msg.data[0] = button_id;
	msg.data[1] = push_long;
	CAN_send(&msg);
}


void BUTTONS_IHM_process_it(void)
{
	Uint8 i;
	for(i=0;i<BUTTONS_IHM_NUMBER;i++)
	{
		if(push_time[i] != 0)
		{
			push_time[i]--;
		}
	}

	for(i=0;i<BP_INT_NUMBER;i++)
	{
		if(push_int_time[i] != 0)
		{
			push_int_time[i]--;
		}
	}
}

void BUTTONS_IHM_VERBOSE(void){
	Uint8 bp_selftest=2, bp_calibration=2, bp_print_match=2, bp_ok=2, bp_up=2, bp_down=2, bp_set=2, bp_rfu=2;

	if(BUTTON_SELFTEST_PORT 	!= bp_selftest		)	{	bp_selftest 	= BUTTON_SELFTEST_PORT;		debug_printf("bp_selftest = %s\n"	, (bp_selftest)?	"ON":"OFF");	 }
	if(BUTTON_CALIBRATION_PORT 	!= bp_calibration	)	{	bp_calibration  = BUTTON_CALIBRATION_PORT;	debug_printf("bp_calibration = %s\n", (bp_calibration)?	"ON":"OFF");	 }
	if(BUTTON_PRINTMATCH_PORT 	!= bp_print_match	)	{	bp_print_match 	= BUTTON_PRINTMATCH_PORT;	debug_printf("bp_print_match = %s\n", (bp_print_match)?	"ON":"OFF");	 }
	if(BUTTON_OK_PORT			!= bp_ok			)	{	bp_ok			= BUTTON_OK_PORT;			debug_printf("bp_ok = %s\n"			, (bp_ok)?			"ON":"OFF");	 }
	if(BUTTON_UP_PORT			!= bp_up			)	{	bp_up			= BUTTON_UP_PORT;			debug_printf("bp_up = %s\n"			, (bp_up)?			"ON":"OFF");	 }
	if(BUTTON_DOWN_PORT			!= bp_down			)	{	bp_down			= BUTTON_DOWN_PORT;			debug_printf("bp_down = %s\n"		, (bp_down)?		"ON":"OFF");	 }
	if(BUTTON_SET_PORT			!= bp_set			)	{	bp_set			= BUTTON_SET_PORT;			debug_printf("bp_set = %s\n"		, (bp_set)?			"ON":"OFF");	 }
	if(BUTTON_RFU_PORT			!= bp_rfu			)	{	bp_rfu			= BUTTON_RFU_PORT;			debug_printf("bp_rfu = %s\n"		, (bp_rfu)?			"ON":"OFF");	 }
}


//#ifndef BUTTONS_NO_IT

//	void TIMER_SRC_TIMER_interrupt()
//	{
//		BUTTONS_IHM_process_it();
//		TIMER_SRC_TIMER_resetFlag();
//	}

//#endif
