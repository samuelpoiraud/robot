#ifndef ACT_CAN_H
#define ACT_CAN_H

#include "QS/QS_types.h"
#include "queue.h"
#include "avoidance.h"

//Info sur la gestion d'erreur des actionneurs:
//La carte actionneur génère des resultats et détail les erreurs suivant ce qu'elle sait et les envois par message CAN avec ACT_RESULT
//La fonction ACT_process_result (act_function.c) converti les messages ACT_RESULT en ces valeurs dans act_state_info_t::operationResult et act_state_info_t::recommendedBehavior (environnement.h)
//La fonction ACT_check_result (act_function.c) converti et gère les messages act_state_info_t::operationResult et act_state_info_t::recommendedBehavior en information ACT_function_result_e (dans act_function.h) pour être ensuite utilisé par le reste du code stratégie.

//Décommentez ce paramêtre ou mettez le dans Global_config pour empêcher un actionneur d'être désactivé
//De cette façon, si cette constante est définie, une demande d'opération ne sera jamais refusée
#define ACT_NEVER_DISABLE

//Décommentez ce paramêtre ou mettez le dans Global_config pour désactiver la gestion d'erreur. (dangereux)
//De cette façon, si cette constante est définie, tout renvoi de résultat par la carte actionneur sera considéré comme une réussite. Le timeout de la carte strat est désactivé.
//#define ACT_NO_ERROR_HANDLING

//Décommentez ce paramètre ou mettez le dans Global_config pour désactiver les actions fallback en cas d'erreur.
//Par exemple si la pince à assiette n'arrive pas a se baisser, elle se remontera automatiquement et retournera RetryLater si elle reussie ou ActDisabled si elle ne reussi pas a remonter le bras.
//Activer ce define fait que aucune action n'est entreprit automatiquement, et en cas de problème RetryLater est retourné
//#define ACT_DONT_TRY_FALLBACK

//Resultat de la dernière opération en cours d'exécution.
//Vous pouvez aussi utiliser l'enum de avoidance (IN_PROGRESS, END_OK, END_WITH_TIMEOUT ou NOT_HANDLED) si vous voulez
typedef enum {
	ACT_FUNCTION_InProgress = IN_PROGRESS,        //L'opération ne s'est pas encore finie
	ACT_FUNCTION_Done = END_OK,                   //L'opération s'est finie correctement. C'est aussi la valeur au démarrage lorsque aucune action n'a encore été faite.
	ACT_FUNCTION_ActDisabled = END_WITH_TIMEOUT,  //L'opération n'a pas pu se terminer: l'actionneur ne marche plus
	ACT_FUNCTION_RetryLater = NOT_HANDLED         //L'opération n'a pas pu se terminer: il faudra retenter plus tard (plus tard = faire autre chose avant de retest, pas juste 3ms, causé par une position impossible à atteindre, robot adverse qui bloque ? positionnement en mode loto trop près du bord ?)
} ACT_function_result_e;

// Récupère le resultat de la dernière action associé à une pile (non valable pour ASSER) Cette valeur ne change pas tant qu'aucune opération ne commence ou se finie.
// A utiliser pour savoir l'état d'une ou plusieurs action en cours.
ACT_function_result_e ACT_get_last_action_result(queue_id_e act_id);

void ACT_arg_init(QUEUE_arg_t* arg, Uint16 sid, Uint8 cmd);
void ACT_arg_init_with_param(QUEUE_arg_t* arg, Uint16 sid, Uint8 cmd, Uint16 param);
void ACT_arg_set_timeout(QUEUE_arg_t* arg, Uint16 timeout);
void ACT_arg_set_fallbackmsg(QUEUE_arg_t* arg, Uint16 sid, Uint8 cmd);
void ACT_arg_set_fallbackmsg_with_param(QUEUE_arg_t* arg, Uint16 sid, Uint8 cmd, Uint16 param);

bool_e ACT_push_operation(queue_id_e act_id, QUEUE_arg_t* arg);

// Gère les messages de retour de la carte actionneur lorsque une action s'est terminée ou a échouée
void ACT_process_result(const CAN_msg_t* msg);

#endif // ACT_CAN_H
