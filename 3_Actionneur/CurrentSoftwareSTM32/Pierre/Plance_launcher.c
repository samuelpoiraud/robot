/*  Club Robot ESEO 2012 - 2013
 *	BIG
 *
 *	Fichier : Lance_Launcher.c
 *	Package : Carte actionneur
 *	Description : Gestion du lanceur de lance
 *  Auteur : amaury
 *  Version 1
 *  Robot : BIG
 */

#include "Plance_launcher.h"
#ifdef I_AM_ROBOT_BIG

#include "../QS/QS_CANmsgList.h"
#include "../QS/QS_global_vars.h"
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
	#warning "On veut de la précision dans le timer, il faudrait eviter le WATCHDOG pour cela"
	#define TIMER_SRC_USE_WATCHDOG
#endif

#include "../QS/QS_setTimerSource.h"

#define ACTION_TIMEOUT 100 // [0.1s] au bout de 10sec, on arrête le lancé et renvoie l'info du problème à la strat: le lancé prend trop de temps (erreur soft probablement)
#define TIME_BETWEEN_LANCE 30 // Est multiplié par 10 derriére car sur 8 bits ne peut pas depasser 256ms sinon
#define TIME_HOLD_LAUNCHER_MAX 30 //    idem ci-dessus

bool_e EXECUTING_LAUNCH = FALSE;

static bool_e start_next_launcher();
static bool_e hold_state();

static Uint8 lance_launcher_last_launch = 0;
static bool_e start_next_launcher();
static Uint8 stateLauncher[6] = {0};
static Uint8 orderShootLauncher[6] = {0};
static Uint16 sens;

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
	if(msg->sid == ACT_LANCELAUNCHER) {
		LANCE_LAUNCHER_init();

		debug_printf("Recoit le message CAN\n");

		switch(msg->data[0]) {
			//Même action quelque soit la commande RUN

			case ACT_LANCELAUNCHER_RUN_1_BALL:
			case ACT_LANCELAUNCHER_RUN_5_BALL:
			case ACT_LANCELAUNCHER_RUN_ALL:
				EXECUTING_LAUNCH = TRUE;
				ACTQ_push_operation_from_msg(msg, QUEUE_ACT_lancelauncher, &LANCE_LAUNCHER_run_command, 0);  //param en centaine de ms, data[1] en sec
				break;

			case ACT_LANCELAUNCHER_STOP:
				LANCE_LAUNCHER_stop();
				component_printf(LOG_LEVEL_Debug, "lanceur stoppé !\n");
				break;

			default:
				component_printf(LOG_LEVEL_Warning, "invalid CAN msg data[0]=%u !\n", msg->data[0]);
		}

		sens = msg->data[1];

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
			if(lance_launcher_last_launch == 0) { //on verifie qu'aucun lanceur n'est activé (car seul un seul pour l'être à la fois)
				Uint8 command = QUEUE_get_arg(queueId)->canCommand;
				//CAN_msg_t resultMsg = {ACT_RESULT, {ACT_BALLINFLATER & 0xFF, command, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK}, 4};

				switch(command) {
					case ACT_LANCELAUNCHER_RUN_1_BALL:
						if(sens == TRUE)
							orderShootLauncher[0] = 1;
						else
							orderShootLauncher[0] = 5;

						lance_launcher_last_launch = 1;
						break;
					case ACT_LANCELAUNCHER_RUN_5_BALL:
						if(sens == TRUE){
							orderShootLauncher[4] = 2; // Premeire balle qui va être tirer
							orderShootLauncher[3] = 3; // 2émé balle à partir, ainsi de suite...
							orderShootLauncher[2] = 4;
							orderShootLauncher[1] = 5;
							orderShootLauncher[0] = 6;
						}else{
							orderShootLauncher[4] = 5;
							orderShootLauncher[3] = 3;
							orderShootLauncher[2] = 4;
							orderShootLauncher[1] = 1;
							orderShootLauncher[0] = 2;
						}

						lance_launcher_last_launch = 5;
						break;

					case ACT_LANCELAUNCHER_RUN_ALL:
						if(sens == TRUE){
							orderShootLauncher[5] = 2;
							orderShootLauncher[4] = 1;
							orderShootLauncher[3] = 3;
							orderShootLauncher[2] = 4;
							orderShootLauncher[1] = 5;
							orderShootLauncher[0] = 6;
						}else{
							orderShootLauncher[5] = 5;
							orderShootLauncher[4] = 6;
							orderShootLauncher[3] = 3;
							orderShootLauncher[2] = 4;
							orderShootLauncher[1] = 1;
							orderShootLauncher[0] = 2;
						}

						lance_launcher_last_launch = 6;
						break;

					case ACT_LANCELAUNCHER_STOP: //La queue n'est pas utilisée pour cette commande
						QUEUE_next(queueId, ACT_LANCELAUNCHER, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
						return; //on ne fait pas le cas normal

					default:
						error_printf("Invalid command: %u, code is broken !\n", command);
						QUEUE_next(queueId, ACT_LANCELAUNCHER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_LOGIC, __LINE__);
						return;
				}
				//Démarrage du premier launcher maintenant (si la strat veut lancer les balles, il ne faut pas attendre)
				debug_printf("Lancement de %d lanceur(s)\n", lance_launcher_last_launch);

				//start_next_launcher(); // Est appele a la fin du timer sinon peut envoyer
				//Démarrage de temps d'attente des prochains lancés
				TIMER_SRC_TIMER_start_ms(1);
			} else {
				warn_printf("Impossible d'effectuer la commande, le lance launcher est déjà utilisé (à l'état %d) (ne devrait jamais être le cas car on ne peut faire 2 actions en même temps avec la même queueid)\n", lance_launcher_last_launch);
				QUEUE_next(queueId, ACT_LANCELAUNCHER, ACT_RESULT_NOT_HANDLED, ACT_RESULT_ERROR_NO_RESOURCES, __LINE__);
			}
		} else {
			//Si terminé, on le dit à la strat (il faut éviter les QUEUE_behead pour cette raison)
			if(lance_launcher_last_launch == 0) {
				QUEUE_next(queueId, ACT_LANCELAUNCHER, ACT_RESULT_DONE, ACT_RESULT_ERROR_OK, __LINE__);
				EXECUTING_LAUNCH = FALSE;
			} else if(CLOCK_get_time() >= QUEUE_get_initial_time(queueId) + ACTION_TIMEOUT) {
				LANCE_LAUNCHER_stop();
				QUEUE_next(queueId, ACT_LANCELAUNCHER, ACT_RESULT_FAILED, ACT_RESULT_ERROR_TIMEOUT, __LINE__);
				EXECUTING_LAUNCH = FALSE;
			}
		}
	}

}

//change le launcher actif, renvoi TRUE si un launcher est encore actif ou FALSE si tous les launchers sont éteints
static bool_e start_next_launcher() {
	switch(orderShootLauncher[lance_launcher_last_launch-1]) {
		default:
			warn_printf("lance_launcher_last_launch est invalide: %d\n", lance_launcher_last_launch);
				lance_launcher_last_launch = 0;
			//PAS DE BREAK. Si lance_launcher_last_launch est invalide, alors on fait un stop aussi

		case 0:
			break;
		case 1 :
			LANCELAUNCHER_PIN_1 = 1;
			stateLauncher[0] = 30;
			break;
		case 2 :
			LANCELAUNCHER_PIN_2 = 1;
			stateLauncher[1] = 20;
			break;
		case 3 :
			LANCELAUNCHER_PIN_3 = 1;
			stateLauncher[2] = 20;
			break;
		case 4 :
			LANCELAUNCHER_PIN_4 = 1;
			stateLauncher[3] = 30;
			break;
		case 5 :
			LANCELAUNCHER_PIN_5 = 1;
			stateLauncher[4] = 20;
			break;
		case 6 :
			LANCELAUNCHER_PIN_6 = 1;
			stateLauncher[5] = 20;
			break;
	}

	//Si stoppé, on décrémente pas, on reste en stoppé (ceci prépare l'action suivante)
	if(lance_launcher_last_launch > 0) {
		lance_launcher_last_launch--;
		return TRUE;
	} else {
		return FALSE;  //On a terminé le lancé que si on vient de faire un stop
	}
}


//Maintien l'état des actionneurs actif
static bool_e hold_state(){

	int i;
	for(i=0;i < 6;i++){
		if(stateLauncher[i] > 0 && stateLauncher[i] <= TIME_HOLD_LAUNCHER_MAX)
			stateLauncher[i]--;

		// Pas de sinon car si passe stateLauncher[i] passe a 0 on ira pas dans le sinon apres (Pose probléme pour le dernier aimant)
		if(stateLauncher[i] <= 0 || stateLauncher[i] > TIME_HOLD_LAUNCHER_MAX){ // On passe les pins a 0 si non active (securite)
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
					LANCELAUNCHER_PIN_1 = 0; //quoi qu'il arrive on coupe tout les lanceurs (par sécurité)
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
		//Si lancé terminé, alors on arrête le timer
		if(start_next_launcher() == FALSE)
			end_lance_launcher = TRUE;

		lance_launcher_timer = 0;
	}

	if(hold_state() == FALSE && end_lance_launcher == TRUE ){
		end_lance_launcher = FALSE;
		TIMER_SRC_TIMER_stop();
	}

	TIMER_SRC_TIMER_resetFlag();
}

#endif	//I_AM_ROBOT_BIG
