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
	SM_ID_NOT_RENSEIGNED_YOU_SHOULD_CORRECT_IT 		= 0x0000,

	//0x100 STRATEGIES
	SM_ID_STRAT_COEF_PROPULSION						= 0x0101,
	SM_ID_STRAT_COEF_PROP							= 0x0102,
	SM_ID_STRAT_REGLAGE_ODO_ROTATION				= 0x0103,
	SM_ID_STRAT_REGLAGE_ODO_TRANSLATION				= 0x0104,
	SM_ID_STRAT_REGLAGE_ODO_SYMETRIE				= 0x0105,
	SM_ID_STRAT_REGLAGE_COMPUTE_COEFS				= 0x0106,
	SM_ID_STRAT_TOURNE_EN_ROND						= 0x0107,
	SM_ID_STRAT_TEST_AVOIDANCE						= 0x0108,
	SM_ID_STRAT_STOP_ROBOT							= 0x0109,

	SM_ID_STRAT_BLACK_INUTILE						= 0x0111,
	SM_ID_STRAT_BLACK_INITIALE						= 0x0112,
	SM_ID_SUB_COLLECT_FISHS							= 0x0113,
	SM_ID_SUB_RUSH_TO_DUNE						    = 0x0114,
	SM_ID_SUB_DEPOSE_SAND_ELEMENTS				    = 0x0115,
	SM_ID_SUB_BLACK_TAKE_SAND_BLOC_NEXT_DUNE	    = 0x0116,
	SM_ID_SUB_BLACK_CLOSE_DOORS              	    = 0x0117,
	SM_ID_SUB_PUSH_SEASHELLS                 	    = 0x0118,
	SM_ID_SUB_BLACK_APOCALYPSE_DUNE					= 0x0119,
	SM_ID_SUB_BLACK_TAKE_SAND_BLOC_START_ZONE       = 0x011A,
	SM_ID_SUB_BLACK_END_OF_MATCH                    = 0x011B,
	SM_ID_STRAT_BLACK_TESTS                         = 0x011C,
	SM_ID_SUB_SNOWPLOW								= 0x011D,
	SM_ID_SUB_CATCH_SEASHELLS_ROCK_BLACK_BACK		= 0x011E,
	SM_ID_SUB_CATCH_SEASHELLS_ROCK_BLACK_FRONT		= 0x011F,
	SM_ID_SUB_BLACK_DEPOSE_END_OF_MATCH      		= 0x0120,
	SM_ID_SUB_BLACK_WAIT_ONE_SEC   					= 0x0121,
	SM_ID_SUB_BLACK_SWEEP_AND_TAKE_DUNE   			= 0x0122,
	SM_ID_SUB_DEPOSE_SAND_ELEMENTS_SHOVEL			= 0x0123,

	SM_ID_STRAT_PEARL_INUTILE						= 0x0140,
	SM_ID_STRAT_PEARL_INITIALE						= 0x0141,
	SM_ID_SUB_PEARL_TAKE_SAND_BLOC_NEXT_DUNE	    = 0x0142,
	SM_ID_SUB_PEARL_TAKE_SAND_BLOC_START_ZONE	    = 0x0143,
	SM_ID_SUB_PEARL_CLOSE_DOORS 	                = 0x0144,
	SM_ID_SUB_COLLECT_COQUILLAGES_ONE_AND_TWO_CLOSE			= 0x0145,
	SM_ID_SUB_COLLECT_COQUILLAGES_TWO_CLOSE			= 0x0146,
	SM_ID_SUB_COLLECT_COQUILLAGES_THREE_AND_FOUR_CLOSE		= 0x0147,
	SM_ID_SUB_COLLECT_COQUILLAGES_FIVE_CLOSE		= 0x0148,
	SM_ID_SUB_PEARL_ATTAQUE							= 0x0149,
	SM_ID_SUB_PEARL_LACHE_COQUILLAGE				= 0x014A,
	SM_ID_SUB_LIVRAISON_COQUILLAGES					= 0x014E,
	SM_ID_SUB_PEARL_PLACE_SAND_BLOC_NEXT_DUNE       = 0x014F,
	SM_ID_SUB_COQUILLAGES_CHOICE                    = 0x0150,
	SM_ID_SUB_BLACK_DEPOSE_ALL_SAND_BLOC		    = 0x0151,
	SM_ID_SUB_PEARL_END_OF_MATCH                    = 0x0152,
	SM_ID_SUB_PEARL_DEPOSE_ALL_SAND_BLOC			= 0x0153,
	SM_ID_STRAT_PEARL_TESTS                         = 0x0154,
	SM_ID_SUB_PEARL_COLLECT_FISHS					= 0x0155,
	SM_ID_SUB_PEARL_WAIT_NORTH    					= 0x0156,
	SM_ID_SUB_PEARL_WAIT_ONE_SEC   					= 0x0157,
	SM_ID_SUB_COLLECT_COQUILLAGES_ONE_MIDDLE		= 0x0145,
	SM_ID_SUB_COLLECT_COQUILLAGES_TWO_MIDDLE		= 0x0146,
	SM_ID_SUB_COLLECT_COQUILLAGES_THREE_MIDDLE 	= 0x0147,
	SM_ID_SUB_COLLECT_COQUILLAGES_FOUR_MIDDLE		= 0x0148,
	SM_ID_SUB_COLLECT_COQUILLAGES_FIVE_MIDDLE       = 0x0149,

	//0x200 SUBACTIONS
	SM_ID_SUB_RECALAGE_X							= 0x0201,
	SM_ID_SUB_RECALAGE_Y							= 0x0202,
	SM_ID_STRAT_EVITEMENT_TESTS                     = 0x0203,


	//0x300 SUBFUNCTIONS
	SM_ID_BOTH_WAIT									= 0x0300,
	SM_ID_TRY_RUSH									= 0x0301,
	SM_ID_ZONE_TRY_LOCK	 							= 0x0305,
	SM_ID_PATHFIND_TRY_GOING						= 0x0306,
	SM_ID_EXTRACTION_OF_FOE							= 0x0307,
	SM_ID_ASTAR_TRY_GOING							= 0x0308,
	SM_ID_GO_TO_START_ZONE							= 0x0309,
	SM_ID_CHECK_COMMUNICATION						= 0x0310,
	SM_ID_ASK_CONFIG_COQUILLAGES					= 0x0311

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

//Vérifie l'état d'un actionneur: action en cours, action terminée correctement ou erreur. S'utilise comme try_going pour les états.
//Voir act_function.h pour les fonctions des actions et pour un exemple de code.
Uint8 check_act_status(queue_id_e act_queue_id, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state);

//Vérifie l'état d'une microstrat: microstrat en cours, microstrat terminée correctement ou microstrat terminée avec une erreur. S'utilise comme try_going pour les états.
Uint8 check_sub_action_result(error_e sub_action, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state);



#endif // STATE_MACHINE_HELPER_H
