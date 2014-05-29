#ifndef STATE_MACHINE_HELPER_H
#define STATE_MACHINE_HELPER_H

#include "QS/QS_types.h"
#include "queue.h"
#include "avoidance.h"
#include "foreach_preprocessor.h"

//ID des machines � �tats, sur 16 bits.
//0xRSMM: R: num�ro de robot, S: num�ro de strat (strat globale comme faire les cadeaux), MM: un octet pour un num�ro de machine � �tat d'un truc pr�cis...
//R = 0 Pour Krusty, 1 pour Tiny, num�ro suivant pour autre chose ...
//Le num�ro est utilis� que dans le message CAN de changement d'�tat pour reconnaitre la machine � �tat.
typedef enum {
	SM_ID_NOT_RENSEIGNED_YOU_SHOULD_CORRECT_IT = 0x0000,
	SM_ID_KRUSTY_STRAT_ALEXIS_FINALE = 0x0001,
	SM_ID_KRUSTY_STRAT_ALEXIS = 0x0002,

	//0x100 STRATEGIES
	SM_ID_STRAT_COEF_PROPULSION 			= 0x0101,
	SM_ID_STRAT_COEF_ASSER  				= 0x0102,
	SM_ID_STRAT_REGLAGE_ODO_TRANSLATION		= 0x0103,
	SM_ID_STRAT_REGLAGE_ODO_SYMETRIE		= 0x0104,
	SM_ID_STRAT_REGLAGE_COMPUTE_COEFS		= 0x0105,
	SM_ID_STRAT_ROBOT_VIRTUEL_AVOIDANCE		= 0x0106,
	SM_ID_STRAT_TOURNE_EN_ROND				= 0x0107,

	SM_ID_STRAT_PIERRE_INUTILE				= 0x0111,
	SM_ID_STRAT_PIERRE_LANNION				= 0x0112,
	SM_ID_STRAT_PIERRE_TEST_POINT2			= 0x0113,
	SM_ID_STRAT_PIERRE_TEST_POINT			= 0x0114,
	SM_ID_STRAT_PIERRE_TEST_FRESQUE			= 0x0115,
	SM_ID_STRAT_PIERRE_TEST_VIDE			= 0x0116,
	SM_ID_STRAT_PIERRE_TEST_SMALL_ARM		= 0x0117,

	SM_ID_STRAT_GUY_INUTILE					= 0x0120,
	SM_ID_STRAT_GUY_XBEE					= 0x0121,
	SM_ID_STRAT_GUY_DETECTION_TRIANGLE		= 0x0122,
	SM_ID_STRAT_GUY_TEST_WARNER_TRIANGLE	= 0x0123,

	SM_ID_STRAT_PLACEMENT_NET				= 0x0130,


	//0x200 SUBACTIONS
	SM_ID_SUB_PIERRE_INITIALE				= 0x0200,
	SM_ID_SUB_PROTECTED_FIRES 				= 0x0201,
	SM_ID_SUB_MANAGE_FRESCO					= 0x0202,
	SM_ID_SUB_FILE_FRESCO					= 0x0203,
	SM_ID_SUB_LANCE_LAUNCHER				= 0x0204,
	SM_ID_SUB_LANCE_LAUNCHER_ENNEMY			= 0x0205,
	SM_ID_SUB_RECALAGE_X					= 0x0206,
	SM_ID_SUB_RECALAGE_Y					= 0x0207,
	SM_ID_SUB_RECALAGE_BEGIN_ZONE			= 0x0208,
	SM_ID_BELGIUM_STRAT						= 0x0209,
	SM_ID_SUB_PROTECT_NORTH_WAY				= 0x020A,

	SM_ID_SUB_PIERRE_FILE_FRUITS			= 0x0210,
	SM_ID_SUB_PIERRE_MANAGE_FRUIT			= 0x0211,
	SM_ID_SUB_PIERRE_TAKE_TREE1_DOUBLE		= 0x0212,
	SM_ID_SUB_PIERRE_TAKE_TREE2_DOUBLE		= 0x0213,
	SM_ID_SUB_PIERRE_TAKE_TREE1_SIMPLE		= 0x0214,
	SM_ID_SUB_PIERRE_TAKE_TREE2_SIMPLE		= 0x0215,
	SM_ID_STRAT_PIERRE_HOMOLOGATION			= 0x0216,
	SM_ID_SUB_PIERRE_DEPLOY_TORCH			= 0x0217,
	SM_ID_SUB_PIERRE_DO_TORCH				= 0x0218,
	SM_ID_SUB_PIERRE_DO_TRIANGLE_START		= 0x0219,

	SM_ID_SUB_GUY_INITIALE	 				= 0x0220,
	SM_ID_SUB_GUY_DEPLOY_TORCH				= 0x0221,
	SM_ID_SUB_GUY_TEST_ARM					= 0x0222,
	SM_ID_SUB_GUY_DO_TRIANGLES_BETWEEN_TREES = 0x0223,
	SM_ID_SUB_GUY_DO_TRIANGLE_START_ADVERSARY = 0x0224,
	SM_ID_SUB_GUY_DO_TORCH					= 0x0225,
	SM_ID_SUB_GOTO_ADVERSARY_ZONE			= 0x0226,
	SM_ID_SUB_GOTO_ADVERSARY_ZONE_ARM_MGT	= 0x0227,
	SM_ID_SUB_GUY_SCAN_AND_BACK				= 0x0228,
	SM_ID_STRAT_GUY_HOMOLOGATION			= 0x0229,

	SM_ID_SUB_BOTH_WAIT						= 0x0230,

	SM_ID_ZONE_TRY_LOCK	 					= 0x0301,
	SM_ID_PATHFIND_TRY_GOING				= 0x0401,
	SM_ID_ACTIONS_BOTH_2014_TEST_PATHFIND 	= 0x0402,
	SM_ID_EXTRACTION_OF_FOE					= 0x0403

} UTILS_state_machine_id_e;


#define STATE_CONVERT_TO_STRING(val) #val


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

// La m�me sans verbose afin de pouvoir cr�e rapidement une machine � �tat avec la gestion de l'entrance automatique
// et de pouvoir utiliser ON_LEAVING et RESET_MAE pour am�liorer la lisibilit� du code
#define CREATE_MAE(state_machine_id, init_state, ...) \
	enum state_e { init_state = 0, __VA_ARGS__ }; \
	static enum state_e state = init_state; \
	static enum state_e last_state = init_state; \
	static enum state_e last_state_for_check_entrance = init_state; \
	static bool_e initialized = FALSE; \
	bool_e entrance = last_state_for_check_entrance != state || !initialized; \
	if(entrance) last_state = last_state_for_check_entrance; \
	last_state_for_check_entrance = state; \
	initialized = TRUE

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

// R�initialise une machine � �tat d�clar�e avec CREATE_MAE_WITH_VERBOSE ou CREATE_MAE
// 0 est la premi�re valeur d'une �numeration sans donner explicitement une autre valeur
#define RESET_MAE() \
	{state = last_state = last_state_for_check_entrance = 0;  \
	initialized = FALSE;}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//D�clare seulement l'enum state_e et le tableau de correspondance �tat <-> repr�sentation textuelle
//Utilis� quand CREATE_MAE_WITH_VERBOSE ne peut l'�tre (machines � �tats sp�ciales)
#define DECLARE_STATES(...) \
	enum state_e { __VA_ARGS__ }; \
	static const char * const state_str[] = { FOREACH(STATE_CONVERT_TO_STRING, __VA_ARGS__) };

// Cr�e une typedef enum de nom 'enum_name' et une tableau de chaine de caract�re de nom 'string_name' contenant toutes les valeurs qui suivent
#define TYPEDEF_ENUM_WITH_STRING(enum_name, string_name, init_state, ...) \
	typedef enum { init_state = 0, __VA_ARGS__ } enum_name; \
	static const char * const string_name[] = { FOREACH(STATE_CONVERT_TO_STRING, init_state, __VA_ARGS__) }

#define STATECHANGE_log(state_define, old_state_name, old_state_id, new_state_name, new_state_id) \
	UTILS_LOG_state_changed(#state_define, state_define, old_state_name, old_state_id, new_state_name, new_state_id)

void UTILS_LOG_state_changed(const char* sm_name, UTILS_state_machine_id_e sm_id, const char* old_state_name, Uint8 old_state_id, const char* new_state_name, Uint8 new_state_id);
void UTILS_CAN_send_state_changed(Uint16 state_machine_id, Uint8 old_state, Uint8 new_state, Uint8 nb_params, ...);

//V�rifie l'�tat d'un actionneur: action en cours, action termin�e correctement ou erreur. S'utilise comme try_going pour les �tats.
//Voir act_function.h pour les fonctions des actions et pour un exemple de code.
Uint8 check_act_status(queue_id_e act_queue_id, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state);

//V�rifie l'�tat d'une microstrat: microstrat en cours, microstrat termin�e correctement ou microstrat termin�e avec une erreur. S'utilise comme try_going pour les �tats.
Uint8 check_sub_action_result(error_e sub_action, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state);



#endif // STATE_MACHINE_HELPER_H
