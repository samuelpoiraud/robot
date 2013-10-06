#ifndef STATE_MACHINE_HELPER_H
#define STATE_MACHINE_HELPER_H

#include "../queue.h"
#include "../avoidance.h"
#include "../foreach_preprocessor.h"

//ID des machines à états, sur 16 bits.
//0xRSMM: R: numéro de robot, S: numéro de strat (strat globale comme faire les cadeaux), MM: un octet pour un numéro de machine à état d'un truc précis...
//R = 0 Pour Krusty, 1 pour Tiny, numéro suivant pour autre chose ...
#define SM_ID_KRUSTY_STRAT_ALEXIS	0x0000
#define SM_ID_KRUSTY_STRAT_ALEXIS_FINALE	0x0001
#define SM_ID_GLASSES_DO		0x0101
#define SM_ID_GLASSES_GRAB		0x0102
#define SM_ID_GLASSES_PUT_DOWN	0x0103
#define SM_ID_CHERRIES_MAIN		0x0200
#define SM_ID_CHERRIES_MOVE		0x0201
#define SM_ID_CHERRIES_GRAB		0x0202
#define SM_ID_CHERRIES_LAUNCH	0x0203
#define SM_ID_CHERRIES_DROP		0x0204
#define SM_ID_ZONE_TRY_LOCK		0x0301


#define STATE_CONVERT_TO_STRING(val) #val

//Déclare les états pour une faire une machine à états.
// Déclare les variables:
// - state : contient l'état courant. C'est la variable à modifier quand on veut changer d'état.
// - entrance : vaut TRUE si l'état à changé. Il est possible alors "d'initialiser" le noveau état.
// - last_state : contient l'état précédent lorsque entrance vaut TRUE.
//Example d'utilisation: CREATE_MAE_WITH_VERBOSE(SM_ID_CHERRIES_MOVE, INIT, ETAT1, ETAT2, DONE);
// Le premier argument doit être défini (avec un #define) à un nombre 16 bits max, il identifie la machine à états.
// Le 2ème argument est l'état à l'initialisation.
// Les autres arguments sont les autre états de la machine à états.
#define CREATE_MAE_WITH_VERBOSE(state_machine_id, init_state, ...) \
	enum state_e { init_state, __VA_ARGS__ }; \
	const char *state_str[] = { FOREACH(STATE_CONVERT_TO_STRING, init_state, __VA_ARGS__) }; \
	static enum state_e state = init_state; \
	static enum state_e last_state = init_state; \
	static enum state_e last_state_for_check_entrance = init_state; \
	bool_e entrance = last_state_for_check_entrance != state; \
	last_state = last_state_for_check_entrance; \
	last_state_for_check_entrance = state; \
	if(entrance) UTILS_LOG_state_changed(#state_machine_id, state_machine_id, state_str[last_state], last_state, state_str[state], state)

//Défini un nom d'état dans un tableau de string. Utilisé pour afficher le nom d'une valeur d'un enum, par exemple:
//Pour que tableau[UN_ETAT_OU_ENUM] = "UN_ETAT_OU_ENUM", il faut faire STATE_STR_DECLARE(tableau, UN_ETAT_OU_ENUM)
#define STATE_STR_DECLARE(tableau, state)  tableau[state] = #state;

//Le bloc de code est dans une do {} while(0) pour pouvoir être mis dans un if sans accolade du genre if(condition) STATE_STR_INIT_UNDECLARED(tableau, nbstate);
//Initialise les états non défini d'un tableau de string.
//Les noms des états non définis est mis à "Unknown"
//Pour définir un état dans un tableau de nom d'état, veuillez utiliser STATE_STR_DECLARE(tableau, etat)
//Un exemple est dispo dans K_STRAT_sub_glasses_alexis
#define STATE_STR_INIT_UNDECLARED(tableau, nbstate) \
	do {\
		Uint8 i; \
		for(i=0; i<(nbstate); i++) {\
			if(tableau[i] == 0) \
				tableau[i] = "Unknown"; \
		} \
	}while(0)

#define STATECHANGE_log(state_define, old_state_name, old_state_id, new_state_name, new_state_id) \
	UTILS_LOG_state_changed(#state_define, state_define, old_state_name, old_state_id, new_state_name, new_state_id)

void UTILS_LOG_state_changed(const char* sm_name, Uint16 sm_id, const char* old_state_name, Uint8 old_state_id, const char* new_state_name, Uint8 new_state_id);
void UTILS_CAN_send_state_changed(Uint16 state_machine_id, Uint8 old_state, Uint8 new_state, Uint8 nb_params, ...);


Uint8 check_act_status(queue_id_e act_queue_id, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state);
Uint8 check_sub_action_result(error_e sub_action, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state);

#endif // STATE_MACHINE_HELPER_H
