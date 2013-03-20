/*
 *	Club Robot ESEO 2009 - 2010
 *	CHOMP
 *
 *	Fichier : act_functions.h
 *	Package : Carte Principale
 *	Description : 	Fonctions de gestion de la pile
 *					de l'actionneur
 *	Auteur : Julien et Ronan
 *	Version 20110313
 */
 
 

#include "QS/QS_all.h"

#ifndef ACT_FUNCTIONS_H
	#define ACT_FUNCTIONS_H

	#include "queue.h"
	#include "QS/QS_CANmsgList.h"

	//Info sur la gestion d'erreur des actionneurs:
	//La carte actionneur génère des resultats et détail les erreurs suivant ce qu'elle sait et les envois par message CAN avec ACT_RESULT
	//La fonction ACT_process_result (act_function.c) converti les messages ACT_RESULT en ces valeurs dans act_state_info_t::operationResult et act_state_info_t::recommendedBehavior (environnement.h)
	//La fonction ACT_check_result (act_function.c) converti et gère les messages act_state_info_t::operationResult et act_state_info_t::recommendedBehavior en information ACT_function_result_e (dans act_function.h) pour être ensuite utilisé par le reste du code stratégie.

	//Décommentez ce paramêtre ou mettez le dans Global_config pour empêcher un actionneur d'être désactivé
	//De cette façon, si cette constante est définie, une demande d'opération ne sera jamais refusée
	//#define ACT_NEVER_DISABLE

	//Décommentez ce paramêtre ou mettez le dans Global_config pour désactiver la gestion d'erreur. (dangereux)
	//De cette façon, si cette constante est définie, tout renvoi de résultat par la carte actionneur sera considéré comme une réussite. Le timeout de la carte strat n'est pas désactivé.
	//#define ACT_NO_ERROR_HANDLING


	//Resultat de la dernière opération en cours d'exécution. Utilisé pour connaitre l'état après une erreur surtout.
	typedef enum {
		ACT_FUNCTION_InProgress,   //L'opération ne s'est pas encore finie
		ACT_FUNCTION_Done,         //L'opération s'est finie correctement. C'est aussi la valeur au démarrage lorsque aucune action n'a encore été faite.
		ACT_FUNCTION_ActDisabled,  //L'opération n'a pas pu se terminer: l'actionneur ne marche plus
		ACT_FUNCTION_RetryLater    //L'opération n'a pas pu se terminer: il faudra retenter plus tard (plus tard = faire autre chose avant de retest, pas juste 3ms, causé par un position impossible à atteindre, robot adverse qui bloque ? positionnement en mode loto trop près du bord ?)
	} ACT_function_result_e;

	// Gère les messages de retour de la carte actionneur lorsque une action s'est terminé ou à échouée
	void ACT_process_result(const CAN_msg_t* msg);

	// Récupère le resultat de la dernière action associé à une pile (non valable pour ASSER) Cette valeur ne change pas tant qu'aucune opération ne commence ou se finie.
	// A utiliser après la detection d'une erreur lors de l'exécution d'une pile pour savoir quoi faire par la suite (reporter l'action ou l'annuler)

	// !!!!!!! CETTE FONCTION UTILISE ACT_QUEUE_* AU LIEU DE ACT_STACK_* COMME L'ANCIENNE ACT_get_last_action_result !!!!!!
	ACT_function_result_e ACT_get_last_action_result2(queue_id_e act_id);
	ACT_function_result_e ACT_get_last_action_result(stack_id_e act_id);

/* Fonctions empilables */

	//////////////// KRUSTY ////////////////

/* Actionneur associé: ACT_QUEUE_BallLauncher */
	//Lancer le lanceur de balle à la vitesse indiquée (tr/min)
	bool_e ACT_push_ball_launcher_run(Uint16 speed, bool_e run);
	//Stopper le lanceur de balle
	bool_e ACT_push_ball_launcher_stop(bool_e run);

/* Actionneur associé: ACT_QUEUE_Plate */
	typedef enum {
		ACT_PLATE_RotateUp = ACT_PLATE_ROTATE_VERTICALLY,
		ACT_PLATE_RotateMid = ACT_PLATE_ROTATE_PREPARE,
		ACT_PLATE_RotateDown = ACT_PLATE_ROTATE_HORIZONTALLY,
		ACT_PLATE_RotateRanger = ACT_PLATE_RotateUp,
		ACT_PLATE_RotateLuckyLuke = ACT_PLATE_RotateUp
	} ACT_plate_rotate_cmd_t;

	typedef enum {
		ACT_PLATE_PlierOpen = ACT_PLATE_PLIER_OPEN,
		ACT_PLATE_PlierClose = ACT_PLATE_PLIER_CLOSE
	} ACT_plate_plier_cmd_t;

	bool_e ACT_plate_rotate(ACT_plate_rotate_cmd_t cmd);
	bool_e ACT_plate_plier(ACT_plate_plier_cmd_t cmd);

/* Actionneur associé: ACT_QUEUE_LiftRight et ACT_QUEUE_LiftLeft */
	typedef enum {
		ACT_LIFT_Left = ACT_LIFT_LEFT,
		ACT_LIFT_Right = ACT_LIFT_RIGHT
	} ACT_lift_pos_t;

	typedef enum {
		ACT_LIFT_TranslateUp = ACT_LIFT_GO_UP,
		ACT_LIFT_TranslateMid = ACT_LIFT_GO_MID,
		ACT_LIFT_TranslateDown = ACT_LIFT_GO_DOWN
	} ACT_lift_translate_cmd_t;

	typedef enum {
		ACT_LIFT_PlierOpen = ACT_LIFT_PLIER_OPEN,
		ACT_LIFT_PlierClose = ACT_LIFT_PLIER_CLOSE
	} ACT_lift_plier_cmd_t;

	bool_e ACT_lift_translate(ACT_lift_pos_t lift_id, ACT_lift_translate_cmd_t cmd);
	bool_e ACT_lift_plier(ACT_lift_pos_t lift_id, ACT_lift_plier_cmd_t cmd);

	///////////////// TINY /////////////////

/* Actionneur associé: ACT_QUEUE_Hammer */
	//Changer la position du bras
	bool_e ACT_push_hammer_goto(Uint16 position, bool_e run);
	//Arreter l'asservissement, en cas de problème par exemple, ne devrai pas servir en match.
	//Le bras n'est plus controllé après ça, si la gravité existe toujours, il tombera.
	bool_e ACT_push_hammer_stop(bool_e run);

/* Actionneur associé: ACT_QUEUE_BallInflater */
	//Gonfler le ballon pendant duration_sec secondes. Le message de retour n'attend pas la fin du gonflage.
	bool_e ACT_push_ball_inflater_inflate(Uint8 duration_sec, bool_e run);
	//Stopper le gonflage
	bool_e ACT_push_ball_inflater_stop(bool_e run);

#endif /* ndef ACT_FUNCTIONS_H */
