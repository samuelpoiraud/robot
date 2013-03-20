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
	//La carte actionneur g�n�re des resultats et d�tail les erreurs suivant ce qu'elle sait et les envois par message CAN avec ACT_RESULT
	//La fonction ACT_process_result (act_function.c) converti les messages ACT_RESULT en ces valeurs dans act_state_info_t::operationResult et act_state_info_t::recommendedBehavior (environnement.h)
	//La fonction ACT_check_result (act_function.c) converti et g�re les messages act_state_info_t::operationResult et act_state_info_t::recommendedBehavior en information ACT_function_result_e (dans act_function.h) pour �tre ensuite utilis� par le reste du code strat�gie.

	//D�commentez ce param�tre ou mettez le dans Global_config pour emp�cher un actionneur d'�tre d�sactiv�
	//De cette fa�on, si cette constante est d�finie, une demande d'op�ration ne sera jamais refus�e
	//#define ACT_NEVER_DISABLE

	//D�commentez ce param�tre ou mettez le dans Global_config pour d�sactiver la gestion d'erreur. (dangereux)
	//De cette fa�on, si cette constante est d�finie, tout renvoi de r�sultat par la carte actionneur sera consid�r� comme une r�ussite. Le timeout de la carte strat n'est pas d�sactiv�.
	//#define ACT_NO_ERROR_HANDLING


	//Resultat de la derni�re op�ration en cours d'ex�cution. Utilis� pour connaitre l'�tat apr�s une erreur surtout.
	typedef enum {
		ACT_FUNCTION_InProgress,   //L'op�ration ne s'est pas encore finie
		ACT_FUNCTION_Done,         //L'op�ration s'est finie correctement. C'est aussi la valeur au d�marrage lorsque aucune action n'a encore �t� faite.
		ACT_FUNCTION_ActDisabled,  //L'op�ration n'a pas pu se terminer: l'actionneur ne marche plus
		ACT_FUNCTION_RetryLater    //L'op�ration n'a pas pu se terminer: il faudra retenter plus tard (plus tard = faire autre chose avant de retest, pas juste 3ms, caus� par un position impossible � atteindre, robot adverse qui bloque ? positionnement en mode loto trop pr�s du bord ?)
	} ACT_function_result_e;

	// G�re les messages de retour de la carte actionneur lorsque une action s'est termin� ou � �chou�e
	void ACT_process_result(const CAN_msg_t* msg);

	// R�cup�re le resultat de la derni�re action associ� � une pile (non valable pour ASSER) Cette valeur ne change pas tant qu'aucune op�ration ne commence ou se finie.
	// A utiliser apr�s la detection d'une erreur lors de l'ex�cution d'une pile pour savoir quoi faire par la suite (reporter l'action ou l'annuler)

	// !!!!!!! CETTE FONCTION UTILISE ACT_QUEUE_* AU LIEU DE ACT_STACK_* COMME L'ANCIENNE ACT_get_last_action_result !!!!!!
	ACT_function_result_e ACT_get_last_action_result2(queue_id_e act_id);
	ACT_function_result_e ACT_get_last_action_result(stack_id_e act_id);

/* Fonctions empilables */

	//////////////// KRUSTY ////////////////

/* Actionneur associ�: ACT_QUEUE_BallLauncher */
	//Lancer le lanceur de balle � la vitesse indiqu�e (tr/min)
	bool_e ACT_push_ball_launcher_run(Uint16 speed, bool_e run);
	//Stopper le lanceur de balle
	bool_e ACT_push_ball_launcher_stop(bool_e run);

/* Actionneur associ�: ACT_QUEUE_Plate */
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

/* Actionneur associ�: ACT_QUEUE_LiftRight et ACT_QUEUE_LiftLeft */
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

/* Actionneur associ�: ACT_QUEUE_Hammer */
	//Changer la position du bras
	bool_e ACT_push_hammer_goto(Uint16 position, bool_e run);
	//Arreter l'asservissement, en cas de probl�me par exemple, ne devrai pas servir en match.
	//Le bras n'est plus controll� apr�s �a, si la gravit� existe toujours, il tombera.
	bool_e ACT_push_hammer_stop(bool_e run);

/* Actionneur associ�: ACT_QUEUE_BallInflater */
	//Gonfler le ballon pendant duration_sec secondes. Le message de retour n'attend pas la fin du gonflage.
	bool_e ACT_push_ball_inflater_inflate(Uint8 duration_sec, bool_e run);
	//Stopper le gonflage
	bool_e ACT_push_ball_inflater_stop(bool_e run);

#endif /* ndef ACT_FUNCTIONS_H */
