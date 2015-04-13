#ifndef STATE_MACHINE_HELPER_H
#define STATE_MACHINE_HELPER_H

#include "QS/QS_all.h"
#include "QS/QS_macro.h"
#include "queue.h"
#include "avoidance.h"
#include "foreach_preprocessor.h"

//ID des machines à états, sur 16 bits.
//0xRSMM: R: numéro de robot, S: numéro de strat (strat globale comme faire les cadeaux), MM: un octet pour un numéro de machine à état d'un truc précis...
//R = 0 Pour Krusty, 1 pour Tiny, numéro suivant pour autre chose ...
//Le numéro est utilisé que dans le message CAN de changement d'état pour reconnaitre la machine à état.
typedef enum {
	SM_ID_NOT_RENSEIGNED_YOU_SHOULD_CORRECT_IT = 0x0000,

	//0x100 STRATEGIES
	SM_ID_STRAT_COEF_PROPULSION 			= 0x0101,
	SM_ID_STRAT_COEF_PROP	  				= 0x0102,
	SM_ID_STRAT_REGLAGE_ODO_TRANSLATION		= 0x0103,
	SM_ID_STRAT_REGLAGE_ODO_SYMETRIE		= 0x0104,
	SM_ID_STRAT_REGLAGE_COMPUTE_COEFS		= 0x0105,
	SM_ID_STRAT_TOURNE_EN_ROND				= 0x0107,
	SM_ID_STRAT_TEST_AVOIDANCE				= 0x0108,

	SM_ID_STRAT_HOLLY_INUTILE				= 0x0111,
	SM_ID_STRAT_HOLLY_COLLECT_POPCORN		= 0x0112,
	SM_ID_STRAT_HOLLY_DROP_POPCORN			= 0x0113,
	SM_ID_STRAT_HOLLY_INITIAL				= 0x0114,

	SM_ID_STRAT_WOOD_INUTILE				= 0x0120,
	SM_ID_STRAT_WOOD_INITIAL				= 0x0121,
	SM_ID_STRAT_WOOD_PROTECT_OUR_POPCORN_MACHINE = 0x0122,
	SM_ID_STRAT_WOOD_DO_ADV_CLAP			= 0x0123,
	SM_ID_STRAT_WOOD_DO_OUR_CLAP			= 0x0124,
	SM_ID_STRAT_WOOD_GET_ADV_POPCORN        = 0x0125,
	SM_ID_STRAT_WOOD_DISPOSE_CUP			= 0x125,
	SM_ID_STRAT_WOOD_POSE_ONE_CUP			= 0x126,
	SM_ID_STRAT_WOOD_COLLECT_CUP_SOUTH_YELLOW  = 0x127,
	SM_ID_STRAT_WOOD_COLLECT_CUP_SOUTH_GREEN   = 0x128,
	SM_ID_STRAT_WOOD_COLLECT_CUP_MIDDLE        = 0x129,
	SM_ID_STRAT_WOOD_COLLECT_CUP_NORTH_YELLOW  = 0x130,
	SM_ID_STRAT_WOOD_COLLECT_CUP_NORTH_GREEN   = 0x131,
	SM_ID_STRAT_WOOD_COLLECT_CUP_GLOBAL        = 0x132,
	SM_ID_STRAT_WOOD_DISPOSE_STOLEN_CUP        = 0x133,
	SM_ID_SUB_WOOD_SCAN_AND_STEAL_NORTH_CUP    = 0x134,
	SM_ID_STRAT_WOOD_DISPOSE_CUP_END_MATCH     = 0x135,
	SM_ID_STRAT_WOOD_PUSH_ADV_NORTH_CUP		   = 0x136,
	SM_ID_STRAT_WOOD_PROTECT_NORTH			   = 0x137,
	SM_ID_STRAT_WOOD_PROTECT_SOUTH			   = 0x138,
	SM_ID_STRAT_WOOD_PROTECT_SOUTH_AND_NORTH   = 0x139,
	SM_ID_STRAT_WOOD_GET_OUR_POPCORN           = 0x140,
	SM_ID_STRAT_WOOD_PUSH_FEET				   = 0x141,

	//0x200 SUBACTIONS
	SM_ID_SUB_RECALAGE_X					= 0x0206,
	SM_ID_SUB_RECALAGE_Y					= 0x0207,
	SM_ID_SUB_DISPOSE_IN_START_ZONE			= 0x0208,
	SM_ID_SUB_MIDDLE_FEET_GATHERING			= 0x0209,
	SM_ID_SUB_NORTH_GATHERING				= 0x020A,
	SM_ID_SUB_NORTH_GATHERING_NO_REAR_FOOT	= 0x020B,
	SM_ID_SUB_NORTH_GATHERING_REAR_FEET		= 0x020C,
	SM_ID_SUB_NORTH_GATHERING_STAIRS_FEET	= 0x020D,
	SM_ID_SUB_SOUTH_GATHERING				= 0x020E,
	SM_ID_SUB_DISPOSE_CARPETS				= 0x020F,
	SM_ID_SUB_HOLLY_CARPET_LAUNCHER			= 0x0210,
	SM_ID_SUB_HOLLY_SCAN_DISPOSE_ON_ESTRAD	= 0x0211,
	SM_ID_SUB_HOLLY_TAKE_REAR_FEET_IN_LIFT	= 0x0212,
	SM_ID_SUB_HOLLY_END_OF_MATCH			= 0x0213,

	SM_ID_SUB_BOTH_WAIT						= 0x0230,
	SM_ID_SUB_TRY_RUSH						= 0x0231,

	SM_ID_ZONE_TRY_LOCK	 					= 0x0301,
	SM_ID_PATHFIND_TRY_GOING				= 0x0401,
	SM_ID_EXTRACTION_OF_FOE					= 0x0403,
	SM_ID_WOOD_ASK_FOR_ACCESS_SCAN			= 0x0404,
	SM_ID_SUB_HOLLY_TEST_ACT				= 0x0405,
	SM_ID_SUB_WOOD_TEST_ACT					= 0x0406,
	SM_ID_HOLLY_ASK_PROTECT					= 0x0407,
	SM_ID_HOLLY_MAE_SPOTIX					= 0x0408

} UTILS_state_machine_id_e;


#define STATE_CONVERT_TO_STRING(val) #val

// Fonction d'affichage de l'état initiale prototyper ici car besoin dans les macros suivantes
void UTILS_LOG_init_state(const char* sm_name, UTILS_state_machine_id_e sm_id, const char* init_state_name, Uint8 init_state_val);


// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------Machine à état rapide verbosée ou non------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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
	enum state_e { init_state = 0, __VA_ARGS__ }; \
	static const char * const state_str[] = { FOREACH(STATE_CONVERT_TO_STRING, init_state, __VA_ARGS__) }; \
	static Uint8 size_MAE = sizeof(state_str) / sizeof(const char *); \
	static enum state_e state = init_state; \
	static enum state_e last_state = init_state; \
	static enum state_e last_state_for_check_entrance = init_state; \
	static bool_e initialized = FALSE; \
	bool_e entrance = last_state_for_check_entrance != state || !initialized; \
	assert(state >= 0 && state < size_MAE); \
	if(entrance) last_state = last_state_for_check_entrance; \
	last_state_for_check_entrance = state; \
	if(entrance && initialized) UTILS_LOG_state_changed(#state_machine_id, state_machine_id, state_str[last_state], last_state, state_str[state], state); \
	if(!initialized) UTILS_LOG_init_state(#state_machine_id, state_machine_id, state_str[state], state); \
	initialized = TRUE

// La même sans verbose afin de pouvoir crée rapidement une machine à état avec la gestion de l'entrance automatique
// et de pouvoir utiliser ON_LEAVING et RESET_MAE pour améliorer la lisibilité du code
#define CREATE_MAE(init_state, ...) \
	enum state_e { init_state = 0, __VA_ARGS__ }; \
	static enum state_e state = init_state; \
	static enum state_e last_state = init_state; \
	static enum state_e last_state_for_check_entrance = init_state; \
	static bool_e initialized = FALSE; \
	bool_e entrance = last_state_for_check_entrance != state || !initialized; \
	if(entrance) last_state = last_state_for_check_entrance; \
	last_state_for_check_entrance = state; \
	initialized = TRUE; \
	UNUSED_VAR(last_state)

// Macro à utiliser pour détecter la sortie d'un état
// current_state : l'état dans lequel on est
// Ne marche qu'avec 'CREATE_MAE_WITH_VERBOSE' et 'CREATE_MAE'
// Exemple d'utilisation :
//case SUB_ACTION :
//    ....
//    if(sortance(SUB_ACTION)){
//		// Action à effectuer lors de la sortie de la sub_action
//	  }
//    break;
#define ON_LEAVING(current_state) (state != current_state)

// Réinitialise une machine à état déclarée avec CREATE_MAE_WITH_VERBOSE ou CREATE_MAE
// 0 est la première valeur d'une énumeration sans donner explicitement une autre valeur
#define RESET_MAE() \
	{state = last_state = last_state_for_check_entrance = 0;  \
	initialized = FALSE;}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//Déclare seulement l'enum state_e et le tableau de correspondance état <-> représentation textuelle
//Utilisé quand CREATE_MAE_WITH_VERBOSE ne peut l'être (machines à états spéciales)
#define DECLARE_STATES(...) \
	enum state_e { __VA_ARGS__ }; \
	static const char * const state_str[] = { FOREACH(STATE_CONVERT_TO_STRING, __VA_ARGS__) };

// Crée une typedef enum de nom 'enum_name' et une tableau de chaine de caractère de nom 'string_name' contenant toutes les valeurs qui suivent
#define TYPEDEF_ENUM_WITH_STRING(enum_name, string_name, init_state, ...) \
	typedef enum { init_state = 0, __VA_ARGS__ } enum_name; \
	static const char * const string_name[] = { FOREACH(STATE_CONVERT_TO_STRING, init_state, __VA_ARGS__) }

#define STATECHANGE_log(state_define, old_state_name, old_state_id, new_state_name, new_state_id) \
	UTILS_LOG_state_changed(#state_define, state_define, old_state_name, old_state_id, new_state_name, new_state_id)

void UTILS_LOG_state_changed(const char* sm_name, UTILS_state_machine_id_e sm_id, const char* old_state_name, Uint8 old_state_id, const char* new_state_name, Uint8 new_state_id);
void UTILS_CAN_send_state_changed(Uint16 state_machine_id, Uint8 old_state, Uint8 new_state, Uint8 nb_params, ...);

//Vérifie l'état d'un actionneur: action en cours, action terminée correctement ou erreur. S'utilise comme try_going pour les états.
//Voir act_function.h pour les fonctions des actions et pour un exemple de code.
Uint8 check_act_status(queue_id_e act_queue_id, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state);

//Vérifie l'état d'une microstrat: microstrat en cours, microstrat terminée correctement ou microstrat terminée avec une erreur. S'utilise comme try_going pour les états.
Uint8 check_sub_action_result(error_e sub_action, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state);



#endif // STATE_MACHINE_HELPER_H
