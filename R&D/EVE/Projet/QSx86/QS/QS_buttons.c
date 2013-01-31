/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : QS_buttons.c
 *	Package : QSx86
 *	Description : Fonctions de gestion des boutons de la carte générique
 *	Auteurs : Julien Franchineau & François Even
 *	Version 20111206
 */
 
#define QS_BUTTONS_C

#include "QS_buttons.h"

#ifdef USE_BUTTONS


#if BUTTONS_TIMER == 1
	#define BUTTONS_TIMER_RUN	TIMER1_run
	#define BUTTONS_TIMER_IT	_T1Interrupt
	#define BUTTONS_TIMER_FLAG	IFS0bits.T1IF
#elif BUTTONS_TIMER == 2
	#define BUTTONS_TIMER_RUN	TIMER2_run
	#define BUTTONS_TIMER_IT	_T2Interrupt
	#define BUTTONS_TIMER_FLAG	IFS0bits.T2IF
#elif BUTTONS_TIMER == 3
	#define BUTTONS_TIMER_RUN	TIMER3_run
	#define BUTTONS_TIMER_IT	_T3Interrupt
	#define BUTTONS_TIMER_FLAG	IFS0bits.T3IF
#elif BUTTONS_TIMER == 4
	#define BUTTONS_TIMER_RUN	TIMER4_run
	#define BUTTONS_TIMER_IT	_T4Interrupt
	#define BUTTONS_TIMER_FLAG	IFS1bits.T4IF
#else
	#error "BUTTONS_TIMER doit etre 1 2 3 ou 4"
#endif  // BUTTONS_TIMER == n 


static button_t buttons[4];
static Uint8 push_time[4]={0};

void BUTTONS_init()
{
	Uint8 i;
	
	static bool_e initialized = FALSE;
	if(initialized)
		return;
		
	for(i=0;i<BUTTONS_NUMBER;i++)
	{
		// Définition des actions des boutons
		BUTTONS_define_actions((button_id_e)i,NULL,NULL,0);
	}
	TIMER_init();
	BUTTONS_TIMER_RUN(100);
	initialized = TRUE;
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
	
	buttons_pressed = (BUTTON1_PORT)?1:0;
	buttons_pressed = (BUTTON2_PORT)?buttons_pressed|2:buttons_pressed;
	buttons_pressed = (BUTTON3_PORT)?buttons_pressed|4:buttons_pressed;
	buttons_pressed = (BUTTON4_PORT)?buttons_pressed|8:buttons_pressed;

	//détection des fronts montant
	buttons_rising_edge = (~buttons_were_pressed)&buttons_pressed; 
	
	for(i=0;i<BUTTONS_NUMBER;i++)
	{		
		button = &(buttons[i]);
		if(buttons_rising_edge&(0<<i))
		{			
			push_time[i]=button->long_push_time;
		}
		else
		{
			buttons_falling_edge = buttons_were_pressed&(~buttons_pressed); 
			if(buttons_falling_edge&(1<<i))
			{
				action = button->after_long_push;
				if(action != NULL && push_time[i]==0)
				{
					(*action)();
				}
				else
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

void _ISR BUTTONS_TIMER_IT()
{
	Uint8 i;
	for(i=0;i<BUTTONS_NUMBER;i++)
	{
		if(push_time[i]!=0)
		{		
			push_time[i]--;
		}
	}
	BUTTONS_TIMER_FLAG=0;
}
#endif

