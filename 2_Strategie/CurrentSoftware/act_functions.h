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
	#include "avoidance.h" //pour l'enum de status de l'op�ration (ACT_function_result_e)

	//Info sur la gestion d'erreur des actionneurs:
	//La carte actionneur g�n�re des resultats et d�tail les erreurs suivant ce qu'elle sait et les envois par message CAN avec ACT_RESULT
	//La fonction ACT_process_result (act_function.c) converti les messages ACT_RESULT en ces valeurs dans act_state_info_t::operationResult et act_state_info_t::recommendedBehavior (environnement.h)
	//La fonction ACT_check_result (act_function.c) converti et g�re les messages act_state_info_t::operationResult et act_state_info_t::recommendedBehavior en information ACT_function_result_e (dans act_function.h) pour �tre ensuite utilis� par le reste du code strat�gie.

	//D�commentez ce param�tre ou mettez le dans Global_config pour emp�cher un actionneur d'�tre d�sactiv�
	//De cette fa�on, si cette constante est d�finie, une demande d'op�ration ne sera jamais refus�e
	#define ACT_NEVER_DISABLE

	//D�commentez ce param�tre ou mettez le dans Global_config pour d�sactiver la gestion d'erreur. (dangereux)
	//De cette fa�on, si cette constante est d�finie, tout renvoi de r�sultat par la carte actionneur sera consid�r� comme une r�ussite. Le timeout de la carte strat n'est pas d�sactiv�.
	//#define ACT_NO_ERROR_HANDLING


	//Resultat de la derni�re op�ration en cours d'ex�cution. Utilis� pour connaitre l'�tat apr�s une erreur surtout.
	//Vous pouvez aussi utiliser l'enum de avoidance (IN_PROGRESS, END_OK, END_WITH_TIMEOUT ou NOT_HANDLED) si vous voulez
	typedef enum {
		ACT_FUNCTION_InProgress = IN_PROGRESS,        //L'op�ration ne s'est pas encore finie
		ACT_FUNCTION_Done = END_OK,                   //L'op�ration s'est finie correctement. C'est aussi la valeur au d�marrage lorsque aucune action n'a encore �t� faite.
		ACT_FUNCTION_ActDisabled = END_WITH_TIMEOUT,  //L'op�ration n'a pas pu se terminer: l'actionneur ne marche plus
		ACT_FUNCTION_RetryLater = NOT_HANDLED         //L'op�ration n'a pas pu se terminer: il faudra retenter plus tard (plus tard = faire autre chose avant de retest, pas juste 3ms, caus� par un position impossible � atteindre, robot adverse qui bloque ? positionnement en mode loto trop pr�s du bord ?)
	} ACT_function_result_e;

	// G�re les messages de retour de la carte actionneur lorsque une action s'est termin� ou � �chou�e
	void ACT_process_result(const CAN_msg_t* msg);

	// R�cup�re le resultat de la derni�re action associ� � une pile (non valable pour ASSER) Cette valeur ne change pas tant qu'aucune op�ration ne commence ou se finie.
	// A utiliser apr�s la detection d'une erreur lors de l'ex�cution d'une pile pour savoir quoi faire par la suite (reporter l'action ou l'annuler)

	// !!!!!!! CETTE FONCTION UTILISE ACT_QUEUE_* AU LIEU DE ACT_STACK_* COMME L'ANCIENNE ACT_get_last_action_result !!!!!!
	ACT_function_result_e ACT_get_last_action_result(queue_id_e act_id);

/* Fonctions empilables */

	////////////////////////////////////////
	//////////////// KRUSTY ////////////////
	////////////////////////////////////////

/* Actionneur associ�: ACT_QUEUE_BallLauncher */
	//Lancer le lanceur de balle � la vitesse indiqu�e (tr/min)
	bool_e ACT_ball_launcher_run(Uint16 speed);
	//Stopper le lanceur de balle
	bool_e ACT_ball_launcher_stop();

/* Actionneur associ�: ACT_QUEUE_Plate */
	typedef enum {
		ACT_PLATE_RotateUp = ACT_PLATE_ROTATE_VERTICALLY,
		ACT_PLATE_RotateMid = ACT_PLATE_ROTATE_PREPARE,
		ACT_PLATE_RotateDown = ACT_PLATE_ROTATE_HORIZONTALLY,
		ACT_PLATE_RotateRanger = ACT_PLATE_RotateUp,
		ACT_PLATE_RotateLuckyLuke = ACT_PLATE_RotateUp
	} ACT_plate_rotate_cmd_t;

	//Rotation de la pince � assiette
	bool_e ACT_plate_rotate(ACT_plate_rotate_cmd_t cmd);

	typedef enum {
		ACT_PLATE_PlierOpen = ACT_PLATE_PLIER_OPEN,
		ACT_PLATE_PlierClose = ACT_PLATE_PLIER_CLOSE
	} ACT_plate_plier_cmd_t;

	//Prendre ou lacher l'assiette (avec l'AX12)
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

	//Monter ou descendre l'ascenseur � verres
	bool_e ACT_lift_translate(ACT_lift_pos_t lift_id, ACT_lift_translate_cmd_t cmd);

	typedef enum {
		ACT_LIFT_PlierOpen = ACT_LIFT_PLIER_OPEN,
		ACT_LIFT_PlierClose = ACT_LIFT_PLIER_CLOSE
	} ACT_lift_plier_cmd_t;

	//Lacher ou prendre un verre
	bool_e ACT_lift_plier(ACT_lift_pos_t lift_id, ACT_lift_plier_cmd_t cmd);

/* Actionneur associ�: ACT_QUEUE_BallSorter */
	//Passe a la cerise suivante. La pr�c�dente est envoy�e par le lanceur de balle.
	//A la fin de l'execution de cette action, la couleur de la balle est renvoy�e par message CAN avec le sid ACT_BALLSORTER_RESULT
	bool_e ACT_ball_sorter_next();

	//Comme ACT_ball_sorter_next() mais change la vitesse automatiquement du lanceur de balle apr�s la detection.
	//Cette fonction fait donc automatiquement une demande d'ejection de la cerise prise � la bonne vitesse et en prend une autre.
	//Le message CAN ACT_BALLSORTER_RESULT contenant la couleur de la cerise est quand m�me envoy� � la strat.
	//Lorsque la cerise est detect�e comme pourie (pas blanche) la vitesse utilis� est ball_launcher_speed_white_cherry / 2.
	bool_e ACT_ball_sorter_next_autoset_speed(Uint16 ball_launcher_speed_white_cherry__tr_min);

	////////////////////////////////////////
	///////////////// TINY /////////////////
	////////////////////////////////////////

/* Actionneur associ�: ACT_QUEUE_Hammer */
	//Changer la position du bras
	bool_e ACT_hammer_goto(Uint16 position_en_degre);

	//Soufle une bougie suivant sa couleur (g�r� cot� actionneur). Le robot doit �tre a cot� d'une bougie avec le bras au dessus pret � la taper pour la souffler si elle est de la bonne couleur.
	bool_e ACT_hammer_blow_candle();

	//Arreter l'asservissement, en cas de probl�me par exemple, ne devrai pas servir en match.
	//Le bras n'est plus controll� apr�s �a, si la gravit� existe toujours, il tombera.
	bool_e ACT_hammer_stop();

/* Ces fonctions ne sont pas bloquante et n'utilise pas le syst�me de retour -> vous ne pouvez pas utiliser ACT_get_last_action_result() */
	//Gonfler le ballon pendant duration_sec secondes.
	void ACT_ball_inflater_inflate(Uint8 duration_sec);

	//Stopper le gonflage
	void ACT_ball_inflater_stop(void);

/* Actionneur associ�: ACT_QUEUE_CandleColor */
	typedef enum {
		ACT_CANDLECOLOR_Low = ACT_CANDLECOLOR_GET_LOW,
		ACT_CANDLECOLOR_High = ACT_CANDLECOLOR_GET_HIGH
	} ACT_candlecolor_pos_t;

	//R�cup�re la couleur d'une bougie � l'�tage candle_pos du gateau.
	//La couleur est renvoy�e par le message CAN avec le sid ACT_CANDLECOLOR_RESULT
	bool_e ACT_candlecolor_get_color_at(ACT_candlecolor_pos_t candle_pos);

#endif /* ndef ACT_FUNCTIONS_H */
