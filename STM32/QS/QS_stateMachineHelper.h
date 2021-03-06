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
		SM_ID_STRAT_HARRY_INITIALE    					= 0x0112,
		SM_ID_STRAT_HARRY_END_OF_MATCH					= 0x0113,
		SM_ID_STRAT_HARRY_ROCKET_MONOCOLOR				= 0x0114,
		SM_ID_STRAT_HARRY_FUSEE_MULTICOLOR				= 0x0115,
		SM_ID_STRAT_HARRY_PRISE_ROCKET_MULTICOLOR		= 0x0116,
		SM_ID_STRAT_HARRY_PRISE_MODULES_CENTRE			= 0x0116,
		SM_ID_STRAT_HARRY_TAKE_BIG_CRATER				= 0x0117,
		SM_ID_STRAT_HARRY_DEPOSE_MINERAIS				= 0x0118,
		SM_ID_STRAT_HARRY_DEPOSE_MODULES_CENTRE			= 0x0119,
		SM_ID_STRAT_HARRY_DEPOSE_MODULES_SIDE			= 0x011A,
		SM_ID_STRAT_HARRY_CYLINDER_DEPOSE_MANAGER		= 0x011B,
		SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_MOVE_POS_YELLOW	= 0x011C,
		SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_MOVE_POS_BLUE	= 0x011D,
		SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_YELLOW		= 0x011E,   /// rename
		SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_YELLOW_FUSE	= 0x011F,	// delete
		SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_YELLOW_CORNER	= 0x0120,	//delete
		SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_BLUE			= 0x0121,	///rename
		SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_BLUE_FUSE		= 0x0122,	//delete
		SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_BLUE_CORNER	= 0x0123,	//delete
		SM_ID_STRAT_HARRY_GET_IN_DEPOSE_MINERAIS		=0x0124,
		SM_ID_STRAT_HARRY_SHOOTING_DEPOSE_MINERAIS		=0x0125,
		SM_ID_STRAT_HARRY_DEPOSE_MINERAIS_ALTERNATIVE	=0x0126,
		SM_ID_STRAT_HARRY_GET_IN_DEPOSE_MINERAIS_ALTERNATIVE	=0x0127,
		SM_ID_STRAT_HARRY_RETURN_MODULES_SIDE			=0x0128,
		SM_ID_STRAT_HARRY_TAKE_NORTH_LITTLE_CRATER		=0x0129,
		SM_ID_STRAT_HARRY_GET_IN_NORTH_LITTLE_CRATER	=0x012A,
		SM_ID_STRAT_HARRY_TAKE_SOUTH_LITTLE_CRATER		=0x012B,
		SM_ID_STRAT_HARRY_GET_IN_SOUTH_LITTLE_CRATER	=0x012C,
		SM_ID_STRAT_HARRY_MODULE_CENTER					=0x012D,
		SM_ID_STRAT_HARRY_INIT_CYLINDER					=0x012E,
		SM_ID_STRAT_HARRY_DEFENCE_OUR_DEPOSE_ZONE		=0x012F,
		SM_ID_STRAT_HARRY_DEFENCE_MIDDLE_DEPOSE_ZONE	=0x0130,
		SM_ID_STRAT_HARRY_GET_IN_DEPOSE_MODULES_CENTRE	=0x0135,
		SM_ID_ACT_HARRY_TAKE_ROCKET						=0x0136,
		SM_ID_ACT_HARRY_MAE_STORE_MODULES				=0x0137,
		SM_ID_STRAT_HARRY_MODULE_START_CENTER			=0x0138,
		SM_ID_STRAT_HARRY_MODULE_SIDE_CENTER			=0x0139,
		SM_ID_STRAT_HARRY_MODULE_BASE_CENTER			=0x013A,
		SM_ID_STRAT_HARRY_MODULE_UNICOLOR_NORTH			=0x013B,
		SM_ID_STRAT_HARRY_MODULE_UNICOLOR_SOUTH			=0x013C,
		SM_ID_STRAT_HARRY_MODULES_MANAGER				=0x013D,
		SM_ID_STRAT_HARRY_ORE_DEPOSE_MANAGER			=0x013E,
		SM_ID_STRAT_HARRY_START_TAKE					=0x013F,
		SM_ID_STRAT_HARRY_END_OF_STRAT					=0x0140,
		SM_ID_STRAT_HARRY_GET_IN_POS_1_DEPOSE_MODULES_CENTRE	=0x0141,
		SM_ID_STRAT_HARRY_GET_IN_POS_2_DEPOSE_MODULES_CENTRE	=0x0142,
		SM_ID_STRAT_HARRY_GET_IN_POS_3_DEPOSE_MODULES_CENTRE	=0x0143,
		SM_ID_STRAT_HARRY_GET_IN_POS_4_DEPOSE_MODULES_CENTRE	=0x0144,
		SM_ID_STRAT_HARRY_GET_IN_POS_5_DEPOSE_MODULES_CENTRE	=0x0145,
		SM_ID_STRAT_HARRY_GET_IN_POS_6_DEPOSE_MODULES_CENTRE	=0x0146,
		SM_ID_STRAT_HARRY_POS_1_DEPOSE_MODULES_CENTRE	=0x0147,
		SM_ID_STRAT_HARRY_POS_2_DEPOSE_MODULES_CENTRE	=0x0148,
		SM_ID_STRAT_HARRY_POS_3_DEPOSE_MODULES_CENTRE	=0x0149,
		SM_ID_STRAT_HARRY_POS_4_DEPOSE_MODULES_CENTRE	=0x014A,
		SM_ID_STRAT_HARRY_POS_5_DEPOSE_MODULES_CENTRE	=0x014B,
		SM_ID_STRAT_HARRY_POS_6_DEPOSE_MODULES_CENTRE	=0x014C,
		SM_ID_STRAT_HARRY_MANAGER_DEFENCE				=0x014D,
		SM_ID_ACT_HARRY_MAE_DISPOSE_MODULES				=0x014E,
		SM_ID_ACT_HARRY_MAE_PREPARE_MODULES_FOR_DISPOSE =0x014F,
		SM_ID_STRAT_HARRY_DEPOSE_MODULES_SIDE_INTRO		=0x0150,
		SM_ID_STRAT_HARRY_DEPOSE_MODULES_SIDE_FIN		=0x0151,
		SM_ID_STRAT_HARRY_PUSH_MODULES_BRETAGNE			=0x0152,
		SM_ID_STRAT_HARRY_LAUNCH_PRISE_MODULE			=0x0153,
		SM_ID_STRAT_HARRY_PUSH_MODULE_NORTH				=0x0154,
		SM_ID_START_HARRY_GET_ONE_MODULE				=0x0155,
		SM_ID_STRAT_HARRY_MODULES_INITIALE				=0x0156,
		SM_ID_STRAT_HARRY_DEPOSE_CENTRE_MANAGER			=0x0157,
		SM_ID_ACT_HARRY_TAKE_ROCKET_PARALLEL			=0x0158,
		SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_YELLOW_MIDDLE	=0x0159,
		SM_ID_STRAT_HARRY_TAKE_BIG_CRATER_BLUE_MIDDLE	=0x015A,

		SM_ID_STRAT_HARRY_DEPOSE_MINERAIS_ZONE			=0x015B, // ajout pour france
		SM_ID_STRAT_HARRY_TAKE_MINERAIS					=0x015C,
		SM_ID_STRAT_HARRY_CRATER_TEST					=0x015D, // just pour test
		SM_ID_STRAT_HARRY_TAKE_ORE_DURING_DISPOSE		=0x015E,

		SM_ID_STRAT_ANNE_INUTILE						= 0x0200,
		SM_ID_STRAT_ANNE_INITIALE    					= 0x0201,
		SM_ID_STRAT_ANNE_END_OF_MATCH					= 0x0202,
		SM_ID_STRAT_ANNE_FUSEE_COLOR					= 0x0203,
		SM_ID_STRAT_ANNE_FUSEE_MULTICOLOR				= 0x0204,
		SM_ID_STRAT_ANNE_PRISE_MODULES_CENTRE			= 0x0205,
		SM_ID_STRAT_ANNE_PRISE_GROS_CRATERE				= 0x0206,
		SM_ID_STRAT_ANNE_PRISE_PETIT_CRATERE			= 0x0207,
		SM_ID_STRAT_ANNE_DEPOSE_MINERAIS				= 0x0208,
		SM_ID_STRAT_ANNE_DEPOSE_MODULES_CENTRE			= 0x0209,
		SM_ID_STRAT_ANNE_MODULE_START_CENTER			= 0x020A,
		SM_ID_STRAT_ANNE_MODULE_SIDE_CENTER				= 0x020B,
		SM_ID_STRAT_ANNE_MODULE_BASE_CENTER				= 0x020C,
		SM_ID_STRAT_ANNE_MODULE_UNICOLOR_NORTH			= 0x020E,
		SM_ID_STRAT_ANNE_MODULE_UNICOLOR_SOUTH			= 0x020F,

		SM_ID_STRAT_ANNE_PRISE_MODULE_CENTRE			= 0x0211,

		SM_ID_STRAT_ANNE_RETURN_MODULES_SIDE			= 0x0210,
		SM_ID_STRAT_anne_CYLINDER_DEPOSE_MANAGER		= 0x0211,
		SM_ID_STRAT_ANNE_POS_1_DEPOSE_MODULES_CENTRE	= 0x0212,
		SM_ID_STRAT_ANNE_POS_2_DEPOSE_MODULES_CENTRE	= 0x0213,
		SM_ID_STRAT_ANNE_POS_3_DEPOSE_MODULES_CENTRE	= 0x0214,
		SM_ID_STRAT_ANNE_POS_4_DEPOSE_MODULES_CENTRE	= 0x0215,
		SM_ID_STRAT_ANNE_POS_5_DEPOSE_MODULES_CENTRE 	= 0x0216,
		SM_ID_STRAT_ANNE_POS_6_DEPOSE_MODULES_CENTRE	= 0x0217,
		SM_ID_STRAT_ANNE_GET_IN_DEPOSE_MODULES_CENTRE	= 0x0218,
		SM_ID_STRAT_ANNE_GET_IN_POS_1_DEPOSE_MODULES_CENTRE		= 0x0219,
		SM_ID_STRAT_ANNE_GET_IN_POS_2_DEPOSE_MODULES_CENTRE		= 0x021A,
		SM_ID_STRAT_ANNE_GET_IN_POS_3_DEPOSE_MODULES_CENTRE		= 0x021B,
		SM_ID_STRAT_ANNE_GET_IN_POS_4_DEPOSE_MODULES_CENTRE		= 0x021C,
		SM_ID_STRAT_ANNE_GET_IN_POS_5_DEPOSE_MODULES_CENTRE		= 0x021D,
		SM_ID_STRAT_ANNE_GET_IN_POS_6_DEPOSE_MODULES_CENTRE		= 0x021E,
		SM_ID_STRAT_ANNE_DEPOSE_MODULES_SIDE_INTRO		= 0x021F,
		SM_ID_STRAT_ANNE_DEPOSE_MODULES_SIDE_FIN		= 0x0220,
		SM_ID_STRAT_ANNE_CYLINDER_DEPOSE_MANAGER		= 0x0221,
		SM_ID_STRAT_ANNE_RETURN_MODULES_CENTRE			= 0x0222,
		SM_ID_STRAT_ANNE_RETURN_MODULES_CENTRE_GET_IN 	= 0x0223,
		SM_ID_STRAT_ANNE_HOMOLOGATION_BELGIQUE			= 0x0224,
		SM_ID_ACT_ANNE_RETURN_MODULE					= 0x0225,
		SM_ID_ACT_ANNE_TAKE_ROCKET						= 0x0226,
		SM_ID_ACT_ANNE_MAE_STORE_MODULES				= 0x0227,
		SM_ID_ACT_ANNE_MAE_PREPARE_MODULES_FOR_DISPOSE  = 0x0228,
		SM_ID_ACT_ANNE_MAE_DISPOSE_MODULES				= 0x0229,
		SM_ID_STRAT_ANNE_MANAGER_RETURN_MODULES			= 0x022A,
		SM_ID_STRAT_ANNE_RETURN_MODULES_SIDE_GET_IN		= 0x022B,
		SM_ID_STRAT_ANNE_DISPOSE_MODULES_SIDE			= 0x022C,
		SM_ID_STRAT_ANNE_DEPOSE_CENTRE_MANAGER			= 0x022D,
		SM_ID_STRAT_ANNE_CHOSE_MOONBASE_AND_DISPOSE	 	= 0x022E,
		SM_ID_STRAT_ANNE_EXTRACT_MOONBASE				= 0x022F,
		SM_ID_SUB_ANNE_FROM_OUR_ROCKET_TO_DZONE			= 0x0230,

		SM_ID_STRAT_ANNE_TURN_MODULES_SIDE				= 0x0231,
		SM_ID_STRAT_ANNE_TURN_MODULES_CENTRE			= 0x0232,
		SM_ID_STRAT_ANNE_CHOSE_MOONBASE_AND_TURN	 	= 0x0233,
		//0x200 SUBACTIONS
		SM_ID_SUB_RECALAGE_X							= 0x0301,
		SM_ID_SUB_RECALAGE_Y							= 0x0302,
		SM_ID_STRAT_EVITEMENT_TESTS                     = 0x0303,
		SM_ID_SUB_CROSS_ROCKER							= 0x0304,
		SM_ID_SUB_WAIT_ONE_SEC							= 0x0305,
		SM_ID_SUB_PROTECT_ZONE							= 0x0306,

		//0x300 SUBFUNCTIONS
		SM_ID_BOTH_WAIT									= 0x0400,
		SM_ID_TRY_RUSH									= 0x0401,
		SM_ID_ZONE_TRY_LOCK	 							= 0x0405,
		SM_ID_PATHFIND_TRY_GOING						= 0x0406,
		SM_ID_EXTRACTION_OF_FOE							= 0x0407,
		SM_ID_ASTAR_TRY_GOING							= 0x0408,
		SM_ID_GO_TO_START_ZONE							= 0x0409,
		SM_ID_SELFTEST_ACT								= 0x040A
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

// Sp�cificit� 2017 pour la gestion des machines � �tats r�entrantes (RIGHT et LEFT)
#define CREATE_MAE_ACT(state_machine_id, init_state, ...) \
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
	initialized = TRUE

// La m�me sans verbose afin de pouvoir cr�e rapidement une machine � �tat avec la gestion de l'entrance automatique
// et de pouvoir utiliser ON_LEAVE et RESET_MAE pour am�liorer la lisibilit� du code
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
//    if(ON_LEAVE(SUB_ACTION)){
//		// Action � effectuer lors de la sortie de la sub_action
//	  }
//    break;
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
