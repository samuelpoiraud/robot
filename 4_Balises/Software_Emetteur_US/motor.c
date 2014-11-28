/*
 *	Club Robot ESEO 2012
 *
 *	Fichier : motor.c
 *	Package : Projet Balise Récepteur US
 *	Description : Gestion du pilotage du moteur avec asservissement en vitesse.
 *					La vitesse angulaire doit être le plus constante possible.
 *	Auteur : Nirgal
 *	Version 201203
 */

#include "motor.h"
#include "QS/QS_qei.h"
#include "QS/QS_pwm.h"
#include <pwm.h>
#include "QS/QS_timer.h"
#include "QS/QS_outputlog.h"
#include "Global_config.h"
#include "QS/QS_rf.h"
#include "EmissionIR.h"

#define AROUND(x)   ((((x)>0)?((Sint16)(x)+0.5):((Sint16)(x)-0.5)))

static volatile bool_e motor_enable;
#define NB_EMISSION_TOUR        (Sint16)(2)     // Nombre d'emission par tour
#define PERIOD_IT_MOTOR 2 //ms
#define KV		 	(Sint16)(8)
#define KP			(Sint16)(32)
#define KD			(Sint16)(4)
#define KI			(Sint16)(2)
//#define COMMAND               (Sint16)(70)//pas par 2ms = 2048*17 pas par secondes = 17 tours par seconde

#define COMMAND                 (Sint16)(AROUND(2048 * (NB_EMISSION_TOUR * 1000. / (PERIODE_FLASH*2 - NO_FLASH_TIME*2))/500.))  // pas par 2ms
                                        //2048*(nb_tour*1000/temps_emission) pas par secondes
                                        //(nb_tour*1000/temps_emission) tours par seconde




void MOTOR_send_msg_battery_low(void);

void MOTOR_init(void)
{
	QEI_init();
	PWM_init();
	motor_enable = TRUE;
	TIMER4_run(PERIOD_IT_MOTOR);
}	

static volatile Sint16 count, error, delta_error, error_prec;
static volatile Sint32 integral = 0;


#define THRESHOLD_DUTY_MAX		330    	// La batterie a une tension inférieure à 7V
#define REPITION_MSG_BAT_LOW	2500
#define FILTER_BATTERY			10
#define START_VALUE_FILTER		300

// compteur pour la reptition des messages quand la batterie est faible
static volatile Sint16 compt_bat_low = 500; // Attend une seconde avant envoyer premier message car pic à l'allumage
static volatile Uint16 duty_filter = START_VALUE_FILTER;

#define NB_IT_PER_TURN			(Uint8)(30)	//it = 2ms, 1 tour = 60ms => 30 it par tour.
#define	NB_ENCODER_STEP_PER_IT	(Uint8)(68)	
static volatile Sint16 speed;
static volatile Sint16 duty;


void MOTOR_send_bat_low(void);

//A appeler toutes les 2 ms.
//ce choix est arbitraire, et très lié au fait que le reste du code utilise également avec une IT 2ms
void MOTOR_process_it(void)
{
	static Sint16 count_prec = 0;
	static Sint16 error_prec = 0;
	static Sint16 sum_error = 0;
	Sint16 count, error, delta_error;
	
	count = QEI_get_count();			//Lecture codeur.
	
	speed = absolute(count - count_prec);	//Calcul vitesse
        
	error = COMMAND - speed;			//Calcul de l'erreur sur la vitesse
	
	delta_error = error - error_prec;	//Calcul de la dérivée de l'erreur sur la vitesse
	
	sum_error += (speed < 10)?0:error;	//Calcul de l'intégrale de l'erreur sur la vitesse (avec sécurité si vitesse faible !)

	//Calcul du rapport cyclique de la PWM à envoyer au moteur.
	duty = (		KV * COMMAND 
				+ 	KP * error 
				+ 	KD * delta_error
				+ 	KI * sum_error
			)/16;

	if(duty <0)	//Ecretage mini
		duty = 1;
	
	SetDCMCPWM(2, duty, 0);	
	
	//On sauvegarde pour le prochain tour.
	count_prec = count;
	error_prec = error;

	// Pour plus de precision multiplie par 10
	if(duty > 20 && duty < 50) // valeurs incoherente sinon
		duty_filter = (duty_filter*(100-FILTER_BATTERY) + (Uint16)(duty)*FILTER_BATTERY*10)/100;

	if(duty_filter > THRESHOLD_DUTY_MAX)
		MOTOR_send_bat_low();
}	

void MOTOR_process_main(void)
{
	//                                                                                                                                                                                                                                                                                       debug_printf("d%d\tV%d\tduty_filter %d",duty,speed,duty_filter);
	//debug_printf("\n");
}

void MOTOR_togle_enable(void)
{
	motor_enable = !motor_enable;
}	

void _ISR _T4Interrupt(void)
{
	MOTOR_process_it();
	IFS1bits.T4IF = 0;
}

void MOTOR_send_bat_low(void){
	compt_bat_low--;

	if(compt_bat_low >= 0)
		return;

	compt_bat_low = REPITION_MSG_BAT_LOW;

	CAN_msg_t msg;
	msg.sid = STRAT_BALISE_BATTERY_STATE;
	msg.size = 1;
	msg.data[0] = NUMERO_BALISE_EMETTRICE;
	msg.data[1] = 0;

	RF_can_send(RF_SMALL, &msg);
	RF_can_send(RF_BIG, &msg);

	debug_printf("Send message battery low\n");
}

void MOTOR_send_bat_state(void){
	CAN_msg_t msg;
	msg.sid = STRAT_BALISE_BATTERY_STATE;
	msg.size = 1;
	msg.data[0] = NUMERO_BALISE_EMETTRICE;
	msg.data[1] = duty_filter < THRESHOLD_DUTY_MAX;

	RF_can_send(RF_SMALL, &msg);
	RF_can_send(RF_BIG, &msg);

	debug_printf("Send message battery %s   duty %d     duty_filter %d\n",(duty_filter > THRESHOLD_DUTY_MAX)?"FAIBLE":"OK",duty,duty_filter);
}
