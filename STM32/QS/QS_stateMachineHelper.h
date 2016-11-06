#ifndef STATE_MACHINE_HELPER_H
#define STATE_MACHINE_HELPER_H

#include "QS_all.h"

//ID des machines � �tats, sur 16 bits.
//0xRSMM: R: num�ro de robot, S: num�ro de strat (strat globale comme faire les cadeaux), MM: un octet pour un num�ro de machine � �tat d'un truc pr�cis...
//R = 0 Pour Krusty, 1 pour Tiny, num�ro suivant pour autre chose ...
//Le num�ro est utilis� que dans le message CAN de changement d'�tat pour reconnaitre la machine � �tat.
typedef enum {
	SM_ID_NOT_RENSEIGNED_YOU_SHOULD_CORRECT_IT 		= 0x0000,

#ifdef I_AM_CARTE_STRAT
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

		SM_ID_STRAT_HARRY_INUTILE						= 0x0111,


		SM_ID_STRAT_ANNE_INUTILE						= 0x0140,



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
#endif

#ifdef I_AM_CARTE_PROP

#endif

#ifdef I_AM_CARTE_ACT

#endif

#ifdef I_AM_CARTE_IHM

#endif

} UTILS_state_machine_id_e;

// Fonction d'affichage de l'�tat initiale prototyper ici car besoin dans les macros suivantes
void UTILS_LOG_init_state(const char* sm_name, UTILS_state_machine_id_e sm_id, const char* init_state_name, Uint8 init_state_val);


// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------Machine � �tat rapide verbos�e ou non------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//D�clare les �tats pour une faire une machine � �tats.
// D�clare les variables:
// - state : contient l'�tat courant. C'est la variable � modifier quand on veut changer d'�tat.
// - entrance : vaut TRUE si l'�tat � chang�. Il est possible alors "d'initialiser" le nouveau �tat.
// - last_state : contient l'�tat pr�c�dent (ou init_state au d�but)
//Example d'utilisation: CREATE_MAE_WITH_VERBOSE(SM_ID_CHERRIES_MOVE, INIT, ETAT1, ETAT2, DONE);
// Le premier argument doit �tre d�fini (avec un #define) � un nombre 16 bits max, il identifie la machine � �tats.
// Le 2�me argument est l'�tat � l'initialisation.
// Les autres arguments sont les autre �tats de la machine � �tats.
#define CREATE_MAE_WITH_VERBOSE(state_machine_id, init_state, ...) \
	enum state_e { init_state = 0, __VA_ARGS__ }; \
	static const char * const state_str[] = { FOREACH(WORD_CONVERT_TO_STRING, init_state, __VA_ARGS__) }; \
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

// La m�me sans verbose afin de pouvoir cr�e rapidement une machine � �tat avec la gestion de l'entrance automatique
// et de pouvoir utiliser ON_LEAVING et RESET_MAE pour am�liorer la lisibilit� du code
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

// Macro � utiliser pour d�tecter la sortie d'un �tat
// current_state : l'�tat dans lequel on est
// Ne marche qu'avec 'CREATE_MAE_WITH_VERBOSE' et 'CREATE_MAE'
// Exemple d'utilisation :
//case SUB_ACTION :
//    ....
//    if(sortance(SUB_ACTION)){
//		// Action � effectuer lors de la sortie de la sub_action
//	  }
//    break;
#define ON_LEAVING(current_state) (state != current_state)
#define ON_LEAVE()					(state != last_state_for_check_entrance)

// R�initialise une machine � �tat d�clar�e avec CREATE_MAE_WITH_VERBOSE ou CREATE_MAE
// 0 est la premi�re valeur d'une �numeration sans donner explicitement une autre valeur
#define RESET_MAE() \
	{state = last_state = last_state_for_check_entrance = 0;  \
	initialized = FALSE;}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define STATECHANGE_log(state_define, old_state_name, old_state_id, new_state_name, new_state_id) \
	UTILS_LOG_state_changed(#state_define, state_define, old_state_name, old_state_id, new_state_name, new_state_id)

void UTILS_LOG_state_changed(const char* sm_name, UTILS_state_machine_id_e sm_id, const char* old_state_name, Uint8 old_state_id, const char* new_state_name, Uint8 new_state_id);

#endif // STATE_MACHINE_HELPER_H