#ifndef STATE_MACHINE_HELPER_H
#define STATE_MACHINE_HELPER_H

#include "QS/QS_types.h"
#include "queue.h"
#include "avoidance.h"
#include "foreach_preprocessor.h"

//ID des machines à états, sur 16 bits.
//0xRSMM: R: numéro de robot, S: numéro de strat (strat globale comme faire les cadeaux), MM: un octet pour un numéro de machine à état d'un truc précis...
//R = 0 Pour Krusty, 1 pour Tiny, numéro suivant pour autre chose ...
//Le numéro est utilisé que dans le message CAN de changement d'état pour reconnaitre la machine à état.
typedef enum {
	SM_ID_NOT_RENSEIGNED_YOU_SHOULD_CORRECT_IT = 0x0000,
	SM_ID_KRUSTY_STRAT_ALEXIS_FINALE = 0x0001,
	SM_ID_KRUSTY_STRAT_ALEXIS = 0x0002,
	SM_ID_GLASSES_DO	 = 0x0101,
	SM_ID_GLASSES_GRAB	 = 0x0102,
	SM_ID_GLASSES_PUT_DOWN = 0x0103,
	SM_ID_CHERRIES_MAIN	 = 0x0200,
	SM_ID_CHERRIES_MOVE	 = 0x0201,
	SM_ID_CHERRIES_GRAB	 = 0x0202,
	SM_ID_CHERRIES_LAUNCH = 0x0203,
	SM_ID_CHERRIES_DROP	 = 0x0204,
	SM_ID_STRAT_COEF_PROPULSION = 0x205,
	SM_ID_STRAT_COEF_ASSER = 0x206,
	SM_ID_ZONE_TRY_LOCK	 = 0x0301,
	SM_ID_PATHFIND_TRY_GOING = 0x0401,
	SM_ID_ACTIONS_BOTH_2014_TEST_PATHFIND = 0x0402

} UTILS_state_machine_id_e;


#define STATE_CONVERT_TO_STRING(val) #val

//Déclare les états pour une faire une machine à états.
// Déclare les variables:
// - state : contient l'état courant. C'est la variable à modifier quand on veut changer d'état.
// - entrance : vaut TRUE si l'état à changé. Il est possible alors "d'initialiser" le nouveau état.
// - last_state : contient l'état précédent (ou init_state au début)
//Example d'utilisation: CREATE_MAE_WITH_VERBOSE(SM_ID_CHERRIES_MOVE, INIT, ETAT1, ETAT2, DONE);
// Le premier argument doit être défini (avec un #define) à un nombre 16 bits max, il identifie la machine à états.
// Le 2ème argument est l'état à l'initialisation.
// Les autres arguments sont les autre états de la machine à états.
#define CREATE_MAE_WITH_VERBOSE(state_machine_id, init_state, ...) \
	enum state_e { init_state, __VA_ARGS__ }; \
	static const char * const state_str[] = { FOREACH(STATE_CONVERT_TO_STRING, init_state, __VA_ARGS__) }; \
	static enum state_e state = init_state; \
	static enum state_e last_state = init_state; \
	static enum state_e last_state_for_check_entrance = init_state; \
	static bool_e initialized = FALSE; \
	bool_e entrance = last_state_for_check_entrance != state || !initialized; \
	if(entrance) last_state = last_state_for_check_entrance; \
	last_state_for_check_entrance = state; \
	if(entrance && initialized) UTILS_LOG_state_changed(#state_machine_id, state_machine_id, state_str[last_state], last_state, state_str[state], state); \
	initialized = TRUE

// Macro à utiliser pour détecter la sortie d'un état
// current_state : l'état dans lequel on est
// Ne marche qu'avec 'CREATE_MAE_WITH_VERBOSE'
// Exemple d'utilisation :
//case SUB_ACTION :
//    ....
//    if(sortance(SUB_ACTION)){
//		// Action à effectuer lors de la sortie de la sub_action
//	  }
//    break;
#define ON_LEAVING(current_state) (state != current_state)

//Réinitialise une machine à état déclarée avec CREATE_MAE_WITH_VERBOSE.
#define RESET_MAE() \
	state = last_state = last_state_for_check_entrance = 0;	//0 est la première valeur d'une énumeration sans donner explicitement une autre valeur

//Déclare seulement l'enum state_e et le tableau de correspondance état <-> représentation textuelle
//Utilisé quand CREATE_MAE_WITH_VERBOSE ne peut l'être (machines à états spéciales)
#define DECLARE_STATES(...) \
	enum state_e { __VA_ARGS__ }; \
	static const char * const state_str[] = { FOREACH(STATE_CONVERT_TO_STRING, __VA_ARGS__) };

#define STATECHANGE_log(state_define, old_state_name, old_state_id, new_state_name, new_state_id) \
	UTILS_LOG_state_changed(#state_define, state_define, old_state_name, old_state_id, new_state_name, new_state_id)

void UTILS_LOG_state_changed(const char* sm_name, UTILS_state_machine_id_e sm_id, const char* old_state_name, Uint8 old_state_id, const char* new_state_name, Uint8 new_state_id);
void UTILS_CAN_send_state_changed(Uint16 state_machine_id, Uint8 old_state, Uint8 new_state, Uint8 nb_params, ...);

//Vérifie l'état d'un actionneur: action en cours, action terminée correctement ou erreur. S'utilise comme try_going pour les états.
//Voir act_function.h pour les fonctions des actions et pour un exemple de code.
Uint8 check_act_status(queue_id_e act_queue_id, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state);

//Vérifie l'état d'une microstrat: microstrat en cours, microstrat terminée correctement ou microstrat terminée avec une erreur. S'utilise comme try_going pour les états.
Uint8 check_sub_action_result(error_e sub_action, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state);

// Crée une typedef enum de nom 'enum_name' et une tableau de chaine de caractère de nom 'string_name' contenant toutes les valeurs qui suivent
#define TYPEDEF_ENUM_WITH_STRING(enum_name, string_name, init_state, ...) \
	typedef enum { init_state = 0, __VA_ARGS__ } enum_name; \
	static const char * const string_name[] = { FOREACH(STATE_CONVERT_TO_STRING, init_state, __VA_ARGS__) }

#endif // STATE_MACHINE_HELPER_H
