#ifndef ACT_CAN_H
#define ACT_CAN_H

#include "QS/QS_all.h"
#include "QS/QS_can.h"
#include "queue.h"
#include "avoidance.h"

//Info sur la gestion d'erreur des actionneurs:
//La carte actionneur g�n�re des resultats et d�tail les erreurs suivant ce qu'elle sait et les envois par message CAN avec ACT_RESULT
//La fonction ACT_process_result (act_function.c) converti les messages ACT_RESULT en ces valeurs dans act_state_info_t::operationResult et act_state_info_t::recommendedBehavior (environnement.h)
//La fonction ACT_check_result (act_function.c) converti et g�re les messages act_state_info_t::operationResult et act_state_info_t::recommendedBehavior en information ACT_function_result_e (dans act_function.h) pour �tre ensuite utilis� par le reste du code strat�gie.

//D�commentez ce param�tre ou mettez le dans Global_config pour emp�cher un actionneur d'�tre d�sactiv�
//De cette fa�on, si cette constante est d�finie, une demande d'op�ration ne sera jamais refus�e
#define ACT_NEVER_DISABLE

//D�commentez ce param�tre ou mettez le dans Global_config pour d�sactiver la gestion d'erreur. (dangereux)
//De cette fa�on, si cette constante est d�finie, tout renvoi de r�sultat par la carte actionneur sera consid�r� comme une r�ussite. Le timeout de la carte strat est d�sactiv�.
//#define ACT_NO_ERROR_HANDLING

//D�commentez ce param�tre ou mettez le dans Global_config pour d�sactiver les actions fallback en cas d'erreur.
//Par exemple si la pince � assiette n'arrive pas a se baisser, elle se remontera automatiquement et retournera RetryLater si elle reussie ou ActDisabled si elle ne reussi pas a remonter le bras.
//Activer ce define fait que aucune action n'est entreprit automatiquement, et en cas de probl�me RetryLater est retourn�
//#define ACT_DONT_TRY_FALLBACK

//Resultat de la derni�re op�ration en cours d'ex�cution.
//Vous pouvez aussi utiliser l'enum de avoidance (IN_PROGRESS, END_OK, END_WITH_TIMEOUT ou NOT_HANDLED) si vous voulez
typedef enum {
	ACT_FUNCTION_InProgress = IN_PROGRESS,        //L'op�ration ne s'est pas encore finie
	ACT_FUNCTION_Done = END_OK,                   //L'op�ration s'est finie correctement. C'est aussi la valeur au d�marrage lorsque aucune action n'a encore �t� faite.
	ACT_FUNCTION_ActDisabled = END_WITH_TIMEOUT,  //L'op�ration n'a pas pu se terminer: l'actionneur ne marche plus
	ACT_FUNCTION_RetryLater = NOT_HANDLED         //L'op�ration n'a pas pu se terminer: il faudra retenter plus tard (plus tard = faire autre chose avant de retest, pas juste 3ms, caus� par une position impossible � atteindre, robot adverse qui bloque ? positionnement en mode loto trop pr�s du bord ?)
} ACT_function_result_e;

typedef enum {
	ACT_RESULT_Idle,	//Etat au d�marage, par la suite ce sera le resultat de la derni�re op�ration effectu�e
	ACT_RESULT_Working,	//L'op�ration n'est pas termin�e
	ACT_RESULT_Ok,		//L'op�ration s'est termin�e correctement
	ACT_RESULT_Failed	//Une erreur est survenue, voir details dans act_error_recommended_behavior_e
} act_result_e;

// R�cup�re le resultat de la derni�re action associ� � une pile (non valable pour PROP) Cette valeur ne change pas tant qu'aucune op�ration ne commence ou se finie.
// A utiliser pour savoir l'�tat d'une ou plusieurs action en cours.
ACT_function_result_e ACT_get_last_action_result(queue_id_e act_id);

Uint32 ACT_get_result_param(queue_id_e act_id);
void ACT_arg_init(QUEUE_arg_t* arg, Uint16 sid, Uint8 cmd);
void ACT_arg_init_with_param(QUEUE_arg_t* arg, Uint16 sid, Uint8 cmd, Uint16 param);
void ACT_arg_init_with_msg(QUEUE_arg_t* arg, CAN_msg_t msg);
void ACT_arg_set_timeout(QUEUE_arg_t* arg, Uint16 timeout);
void ACT_arg_set_fallbackmsg(QUEUE_arg_t* arg, Uint16 sid, Uint8 cmd);
void ACT_arg_set_fallbackmsg_with_param(QUEUE_arg_t* arg, Uint16 sid, Uint8 cmd, Uint16 param);

bool_e ACT_push_operation(queue_id_e act_id, QUEUE_arg_t* arg);

// G�re les messages de retour de la carte actionneur lorsque une action s'est termin�e ou a �chou�e
void ACT_process_result(const CAN_msg_t* msg);

//Impose le r�sultat de l'actionneur (cod� pour les pompes sur la carte strat en 2016)
void ACT_set_result(Uint8 act_id, act_result_e result);

#endif // ACT_CAN_H
