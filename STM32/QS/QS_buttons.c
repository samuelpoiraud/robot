/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : QS_button.c
 *	Package : QS
 *	Description : Fonctions de gestion des boutons de la carte generique
 *  Auteur : Adrien GIRARD
 *	Auteurs Originaux : Jacen & Ronan
 *	Version 20110324
 */
 
#define QS_BUTTONS_C

#include "QS_buttons.h"

#ifdef USE_BUTTONS

#include "QS_ports.h"

#ifdef NEW_CONFIG_ORGANISATION
	#include "config_pin.h"
#endif

#if !defined(BUTTONS_TIMER) && !defined(BUTTONS_TIMER_USE_WATCHDOG)
	#define BUTTONS_NO_IT

	#ifndef I_ASSUME_I_WILL_CALL_BUTTONS_PROCESS_IT_ON_MY_OWN
		#warning "vous avez pris la liberté de ne pas définir de timer pour BUTTONS, ayez au moins le respect d'appeler vous même BUTTONS_process_it() périodiquement..."
		#warning "BUTTONS_TIMER doit etre 1 2 3 ou 4, sauf si vous ne le définissez pas, volontairement."
	#endif
#else
	#ifdef BUTTONS_TIMER
		#define TIMER_SRC_TIMER_ID BUTTONS_TIMER
	#endif
	#ifdef BUTTONS_TIMER_USE_WATCHDOG
		#define TIMER_SRC_USE_WATCHDOG
	#endif

	#include "QS_setTimerSource.h"
#endif

static button_t buttons[BUTTONS_NUMBER];
static Uint8 push_time[BUTTONS_NUMBER]={0};

void BUTTONS_init()
{
	Uint8 i;
	
	static bool_e initialized = FALSE;
	if(initialized)
		return;
		
	for(i=0;i<BUTTONS_NUMBER;i++)
	{
		BUTTONS_define_actions((button_id_e)i,NULL,NULL,0);
	}
	#ifndef BUTTONS_NO_IT
		TIMER_SRC_TIMER_init();
		TIMER_SRC_TIMER_start_ms(100);
	#endif
}

void BUTTONS_define_actions(button_id_e button_id,button_action_t direct_push, button_action_t after_long_push, Uint8 long_push_time)
{
	assert(button_id<BUTTONS_NUMBER&&button_id>=0);
	button_t* button = &(buttons[button_id]);
	button->direct_push=direct_push;
	button->after_long_push=after_long_push;
	button->long_push_time=long_push_time*10;
}

void BUTTONS_update() 
{
	static Uint8 buttons_were_pressed = 0x00; //état des boutons dans le passage précédent
	Uint8 buttons_pressed = 0x00; //état des boutons
	Uint8 buttons_falling_edge; //état des boutons
	Uint8 buttons_rising_edge; //état des boutons
	button_action_t action;	
	button_t* button = NULL;
	Uint8 i;
	
	buttons_pressed = 0;
	#ifdef BUTTON0_PORT
		buttons_pressed = (BUTTON0_PORT)? buttons_pressed|1 : buttons_pressed;
	#endif
	#ifdef BUTTON1_PORT
		buttons_pressed = (BUTTON1_PORT)? buttons_pressed|2 : buttons_pressed;
	#endif
	#ifdef BUTTON2_PORT
		buttons_pressed = (BUTTON2_PORT)? buttons_pressed|4 : buttons_pressed;
	#endif
	#ifdef BUTTON3_PORT
		buttons_pressed = (BUTTON3_PORT)? buttons_pressed|8 : buttons_pressed;
	#endif
	#ifdef BUTTON4_PORT
		buttons_pressed = (BUTTON4_PORT)? buttons_pressed|16 : buttons_pressed;
	#endif
	#ifdef BUTTON5_PORT
		buttons_pressed = (BUTTON5_PORT)? buttons_pressed|32 : buttons_pressed;
	#endif
	#ifdef BUTTON6_PORT
		buttons_pressed = (BUTTON6_PORT)? buttons_pressed|64 : buttons_pressed;
	#endif
	#ifdef BUTTON7_PORT
		buttons_pressed = (BUTTON7_PORT)? buttons_pressed|128 : buttons_pressed;
	#endif

	//détection des fronts montant
	buttons_rising_edge = (~buttons_were_pressed) & buttons_pressed;
	
	for(i=0;i<BUTTONS_NUMBER;i++)
	{
		button = &(buttons[i]);
		if(buttons_rising_edge&(1<<i))
		{			
			push_time[i]=button->long_push_time;
			button->long_push_already_detected = FALSE;
		}
		else
		{
			buttons_falling_edge = buttons_were_pressed & (~buttons_pressed);

			//Nouveau code pour detecter les appui long sans avoir besoin de relacher le bouton avant
			//Prise en compte de l'appui long lorsque le compteur est passé à 0 (temps necessaire avant appui long detecté écoulé)
			//Et bouton pressé (et fonction action non nulle)

			if(button->long_push_already_detected == FALSE) {
				action = button->after_long_push;
				if((buttons_pressed & (1<<i)) && action != NULL && push_time[i] == 0)
				{
					button->long_push_already_detected = TRUE;
					(*action)();
				}
				else if(buttons_falling_edge & (1<<i))
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

	buttons_were_pressed = buttons_pressed;
}

void BUTTONS_process_it(void)
{
	Uint8 i;
	for(i=0;i<BUTTONS_NUMBER;i++)
	{
		if(push_time[i] != 0)
		{		
			push_time[i]--;
		}
	}
}
	
#ifndef BUTTONS_NO_IT
	
	void TIMER_SRC_TIMER_interrupt()
	{
		BUTTONS_process_it();
		TIMER_SRC_TIMER_resetFlag();
	}

#endif

#endif

