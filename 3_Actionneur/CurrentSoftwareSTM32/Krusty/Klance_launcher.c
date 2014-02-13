/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Lance_Launcher.c
 *	Package : Carte actionneur
 *	Description : Gestion du lanceur de lance
 *  Auteur : amaury
 *  Version 1
 *  Robot : Krusty
 */

#include "Klance_launcher.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "../QS/QS_CANmsgList.h"
#include "../act_queue_utils.h"
#include "config_pin.h"

#include "config_debug.h"
#define LOG_PREFIX "LL: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_LANCELAUNCHER
#include "../QS/QS_outputlog.h"

#ifdef LANCELAUNCHER_TIMER_ID
	#define TIMER_SRC_TIMER_ID LANCELAUNCHER_TIMER_ID
#endif
#ifdef LANCELAUNCHER_TIMER_USE_WATCHDOG
	#warning "On veut de la pr�cision dans le timer, il faudrait eviter le WATCHDOG pour cela"
	#define TIMER_SRC_USE_WATCHDOG
#endif

#include "../QS/QS_setTimerSource.h"

#define ACTION_TIMEOUT 100 // [0.1s] au bout de 10sec, on arr�te le lanc� et renvoie l'info du probl�me � la strat: le lanc� prend trop de temps (erreur soft probablement)
#define TIME_BETWEEN_LANCE 30 // Est multipli� par 10 derri�re car sur 8 bits ne peut pas depasser 256ms sinon
#define TIME_HOLD_LAUNCHER 20 //    idem ci-dessus

static Uint8 lance_launcher_last_launch = 0;
static bool_e start_next_launcher();
static Uint8 stateLauncher[6] = {0};

void LANCE_LAUNCHER_init() {
	static bool_e initialized = FALSE;

	if(initialized)
		return;
	initialized = TRUE;
	LANCE_LAUNCHER_stop();
	TIMER_SRC_TIMER_init();
	component_printf(LOG_LEVEL_Debug, "Lance  launcher init !\n");
}

void LANCE_LAUNCHER_stop() {
	lance_launcher_last_launch = 0; //le timer vera que lance_launcher_last_launch vaut 0 et s'auto arretera
	LANCELAUNCHER_PIN_1 = 0;
	LANCELAUNCHER_PIN_2 = 0;
	LANCELAUNCHER_PIN_3 = 0;
	LANCELAUNCHER_PIN_4 = 0;
	LANCELAUNCHER_PIN_5 = 0;
	LANCELAUNCHER_PIN_6 = 0;
}


bool_e LANCE_LAUNCHER_CAN_process_msg(CAN_msg_t* msg) {
	 LANCE_LAUNCHER_init();
	if(msg->sid == ACT_LANCELAUNCHER) {
		switch(msg->data[0]) {
			//M�me action quelque soit la commande RUN
			case ACT_LANCELAUNCHER_RUN:
			case ACT_LANCELAUNCHER_RUN_2:
			case ACT_LANCELAUNCHER_RUN_3:
			case ACT_LANCELAUNCHER_RUN_4:
			case ACT_LANCELAUNCHER_RUN_5:
			case ACT_LANCELAUNCHER_RUN_6:
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_lancelauncher, &LANCE_LAUNCHER_run_command, 0);  //param en centaine de ms, data[1] en sec
				break;

			case ACT_LANCELAUNCHER_STOP:
				LANCE_LAUNCHER_stop();
				component_printf(LOG_LEVEL_Debug, "lanceur stopp� !\n");
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}
		return TRUE;
	}

	return FALSE;
}

void LANCE_LAUNCHER_run_command(queue_id_t queueId, bool_e init) {
	if(QUEUE_get_act(queueId) == QUEUE_ACT_lancelauncher) {
		if(QUEUE_has_error(queueId)) {
			QUEUE_behead(queueId);
			return;
		}

		if(init) {
			if(lance_launcher_last_launch == 0) { //on verifie qu'aucun lanceur n'est activ� (car seul un seul pour l'�tre � la fois)
				Uint8 command = QUEUE_get_arg(queueId)->canCommand;
				//CAN_msg_t resultMsg = {ACT_RESULT, {ACT_BALLINFLATER & 0xFF, command, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK}, 4};

				switch(command) {
					case ACT_LANCELAUNCHER_RUN:
						lance_launcher_last_launch = 1;
						//On ne passe pas direct a la commande suivant, on fait une v�rification du temps pour arr�ter le gonflage apr�s le temps demand�
						break;

					case ACT_LANCELAUNCHER_RUN_2:
						lance_launcher_last_launch = 2;
						//On ne passe pas direct a la commande suivant, on fait une v�rification du temps pour arr�ter le gonflage apr�s le temps demand�
						break;

					case ACT_LANCELAUNCHER_RUN_3:
						lance_launcher_last_launch = 3;
						//On ne passe pas direct a la commande suivant, on fait une v�rification du temps pour arr�ter le gonflage apr�s le temps demand�
						break;

					case ACT_LANCELAUNCHER_RUN_4:
						lance_launcher_last_launch = 4;
						//On ne passe pas direct a la commande suivant, on fait une v�rification du temps pour arr�ter le gonflage apr�s le temps demand�
						break;

					case ACT_LANCELAUNCHER_RUN_5:
						lance_launcher_last_launch = 6;
						//On ne passe pas direct a la commande suivant, on fait une v�rification du temps pour arr�ter le gonflage apr�s le temps demand�
						break;

					case ACT_LANCELAUNCHER_RUN_6:
						lance_launcher_last_launch = 20;
						//On ne passe pas direct a la commande suivant, on fait une v�rification du temps pour arr�ter le gonflage apr�s le temps demand�
						break;


					case ACT_LANCELAUNCHER_STOP: //La queue n'est pas utilis�e pour cette commande
						debug_printf("lanceur stop\n");
						QUEUE_next(queueId, ACT_LANCELAUNCHER, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
						return; //on ne fait pas le cas normal

					default:
						error_printf("Invalid command: %u, code is broken !\n", command);
						QUEUE_next(queueId, ACT_LANCELAUNCHER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
						return;
				}
				//D�marrage du premier launcher maintenant (si la strat veut lancer les balles, il ne faut pas attendre)
				debug_printf("Lancement de %d lanceur(s)\n", lance_launcher_last_launch);

				//start_next_launcher(); // Est appele a la fin du timer sinon peut envoyer
				//D�marrage de temps d'attente des prochains lanc�s
				TIMER_SRC_TIMER_start_ms(1);
			} else {
				warn_printf("Impossible d'effectuer la commande, le lance launcher est d�j� utilis� (� l'�tat %d) (ne devrait jamais �tre le cas car on ne peut faire 2 actions en m�me temps avec la m�me queueid)", lance_launcher_last_launch);
				QUEUE_next(queueId, ACT_LANCELAUNCHER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES, __LINE__);
			}
		} else {
			//Si termin�, on le dit � la strat (il faut �viter les QUEUE_behead pour cette raison)
			if(lance_launcher_last_launch == 0) {
				QUEUE_next(queueId, ACT_LANCELAUNCHER, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
			} else if(CLOCK_get_time() >= QUEUE_get_initial_time(queueId) + ACTION_TIMEOUT) {
				LANCE_LAUNCHER_stop();
				QUEUE_next(queueId, ACT_LANCELAUNCHER, ACT_RESULT_FAILED, ACT_RESULT_ERROR_TIMEOUT, __LINE__);
			}
		}
	}
}

//change le launcher actif, renvoi TRUE si un launcher est encore actif ou FALSE si tous les launchers sont �teints
static bool_e start_next_launcher() {
	switch(lance_launcher_last_launch) {
		default:
			warn_printf("lance_launcher_last_launch est invalide: %d\n", lance_launcher_last_launch);
			//if(lance_launcher_last_launch > 20 || lance_launcher_last_launch < 0)
				lance_launcher_last_launch = 0;
			//PAS DE BREAK. Si lance_launcher_last_launch est invalide, alors on fait un stop aussi


		case 0 :
			break;
		case 1 :
			LANCELAUNCHER_PIN_1 = 1;
			stateLauncher[0] = 25;
			debug_printf("Lanceur 1 \n");
			break;
		case 2 :
			LANCELAUNCHER_PIN_2 = 1;
			stateLauncher[1] = 20;
			debug_printf("Lanceur 2 \n");
			break;
		case 3 :
			LANCELAUNCHER_PIN_3 = 1;
			stateLauncher[2] = 20;
			debug_printf("Lanceur 3 \n");
			break;
		case 4 :
			LANCELAUNCHER_PIN_4 = 1;
			stateLauncher[3] = 25;
			debug_printf("Lanceur 4 \n");
			break;
		case 5 :
			LANCELAUNCHER_PIN_5 = 1;
			stateLauncher[4] = 20;
			debug_printf("Lanceur 5 \n");
			break;
		case 6 :
			LANCELAUNCHER_PIN_6 = 1;
			stateLauncher[5] = 20;
			debug_printf("Lanceur 6 \n");
			break;

		/*case 5 :
			LANCELAUNCHER_PIN_1 = 0; //quoi qu'il arrive on coupe toujours tout les lanceurs (par s�curit�)
			LANCELAUNCHER_PIN_2 = 0;
			LANCELAUNCHER_PIN_3 = 0;
			LANCELAUNCHER_PIN_4 = 0;
			LANCELAUNCHER_PIN_5 = 0;
			LANCELAUNCHER_PIN_6 = 1;
			break;*/

	}

	//Si stopp�, on d�cr�mente pas, on reste en stopp� (ceci pr�pare l'action suivante)
	if(lance_launcher_last_launch > 0) {
		lance_launcher_last_launch--;
		return TRUE;
	} else {
		return FALSE;  //On a termin� le lanc� que si on vient de faire un stop
	}
}


//Maintien l'�tat des actionneurs actif
static bool_e hold_state(){

	int i;
	for(i=0;i < 6;i++){

		//debug_printf("etat %d lanceur %d \n",stateLauncher[i],i+1);

		if(stateLauncher[i] > 0 && stateLauncher[i] <= 25){
			stateLauncher[i]--;
		}

		// Pas de sinon car si passe stateLauncher[i] passe a 0 on ira pas dans le sinon apres (Pose probl�me pour le dernier aimant)
		if(stateLauncher[i] <= 0 || stateLauncher[i] > 25){ // On passe les pins a 0 si non active (securite)
			//debug_printf("				PASSE A 0 lanceur %d \n",i);
			switch(i){
				case 0:
					LANCELAUNCHER_PIN_1 = 0;
					break;
				case 1:
					LANCELAUNCHER_PIN_2 = 0;
					break;
				case 2:
					LANCELAUNCHER_PIN_3 = 0;
					break;
				case 3:
					LANCELAUNCHER_PIN_4 = 0;
					break;
				case 4:
					LANCELAUNCHER_PIN_5 = 0;
					break;
				case 5:
					LANCELAUNCHER_PIN_6 = 0;
					break;
				default:
					LANCELAUNCHER_PIN_1 = 0; //quoi qu'il arrive on coupe tout les lanceurs (par s�curit�)
					LANCELAUNCHER_PIN_2 = 0;
					LANCELAUNCHER_PIN_3 = 0;
					LANCELAUNCHER_PIN_4 = 0;
					LANCELAUNCHER_PIN_5 = 0;
					LANCELAUNCHER_PIN_6 = 0;
			}
		}
	}


	if(stateLauncher[0] != 0 || stateLauncher[1] != 0 || stateLauncher[2] != 0 || stateLauncher[3] != 0 || stateLauncher[4] != 0 || stateLauncher[5] != 0)
		return TRUE; // Un etat a toujours besoins d'etre tenu
	else
		return FALSE;  //Si tous les etats sont revenus a 0
}



void TIMER_SRC_TIMER_interrupt() {
	/* pour avoir une activation d'une seconde pour les lanceurs de lances*/
	static Uint8 lance_launcher_timer = TIME_BETWEEN_LANCE;
	lance_launcher_timer++;

	static bool_e end_lance_launcher = FALSE;

	if (lance_launcher_timer >= TIME_BETWEEN_LANCE) {
		//Si lanc� termin�, alors on arr�te le timer
		if(start_next_launcher() == FALSE)
			end_lance_launcher = TRUE;

		lance_launcher_timer = 0;
	}

	if(hold_state() == FALSE && end_lance_launcher == TRUE )
		TIMER_SRC_TIMER_stop();


	TIMER_SRC_TIMER_resetFlag();
}

#endif	//I_AM_ROBOT_TINY
