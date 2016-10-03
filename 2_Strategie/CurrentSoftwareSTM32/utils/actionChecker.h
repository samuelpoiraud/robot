#ifndef ACTION_CHECKER_H
#define ACTION_CHECKER_H

#include "../QS/QS_all.h"
#include "../actuator/queue.h"
#include "../propulsion/movement.h"

//V�rifie l'�tat d'un actionneur: action en cours, action termin�e correctement ou erreur. S'utilise comme try_going pour les �tats.
//Voir act_function.h pour les fonctions des actions et pour un exemple de code.
Uint8 check_act_status(queue_id_e act_queue_id, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state);

//V�rifie l'�tat d'une microstrat: microstrat en cours, microstrat termin�e correctement ou microstrat termin�e avec une erreur. S'utilise comme try_going pour les �tats.
Uint8 check_sub_action_result(error_e sub_action, Uint8 in_progress_state, Uint8 success_state, Uint8 failed_state);

#endif // ACTION_CHECKER_H
