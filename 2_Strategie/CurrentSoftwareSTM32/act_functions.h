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
#include "act_can.h"

//Voir aussi act_can.h et state_machine_helper.h

/*
 * Exemple de code pour faire une action et l'attendre:
 * case MON_ETAT:
 *                    // On demande qu'une seule fois de faire l'action. Les actions du m�me actionneur peuvent
 *      if(entrance)  // �tre empil�e et seront execut�es les unes apr�s les autres
 *           ACT_fruit_mouth_goto(ACT_FRUIT_Mid);
 *
 *      //Apr�s la demande, on attend la fin de l'action (ou des actions si plusieurs ont �t� empil�es)
 *      //Pour la liste des actionneurs (comme ACT_QUEUE_Fruit), voir l'enum queue_e
 *      state = check_act_status(ACT_QUEUE_Fruit, MON_ETAT, ETAT_OK, ETAT_ERREUR);
 *
 *      //check_act_status est d�finie dans state_machine_helper.h/c
 *      break;
 */

/* Fonctions empilables */

////////////////////////////////////////
//////////////// PIERRE ////////////////
////////////////////////////////////////

//ACT_truc_enum = MSG_CAN,
// ...
typedef enum {
	ACT_FRUIT_Open = ACT_FRUIT_MOUTH_OPEN,
	ACT_FRUIT_Close = ACT_FRUIT_MOUTH_CLOSE,
	ACT_FRUIT_Canceled = ACT_FRUIT_MOUTH_CANCELED,
	ACT_FRUIT_Stop = ACT_FRUIT_MOUTH_STOP
} ACT_fruit_mouth_cmd_e;

typedef enum {
	ACT_Lance_1 =  ACT_LANCELAUNCHER_RUN,
	ACT_Lance_2 = ACT_LANCELAUNCHER_RUN_2,
	ACT_Lance_3 = ACT_LANCELAUNCHER_RUN_3,
	ACT_Lance_4 = ACT_LANCELAUNCHER_RUN_4,
	ACT_Lance_5 = ACT_LANCELAUNCHER_RUN_5,
	ACT_Lance_6 = ACT_LANCELAUNCHER_RUN_6
} ACT_lance_launcher_cmd_e;

typedef enum {
	ACT_Filet_Idle = ACT_FILET_IDLE,
	ACT_Filet_Launched = ACT_FILET_LAUNCHED,
	ACT_Filet_Stop = ACT_FILET_STOP
} ACT_filet_cmd_e;

bool_e ACT_fruit_mouth_goto(ACT_fruit_mouth_cmd_e cmd);
bool_e ACT_lance_launcher_run(ACT_lance_launcher_cmd_e cmd);
bool_e ACT_filet_launch(ACT_filet_cmd_e cmd);
void FILET_process_1ms(void);
void FILET_process_main(void);


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

bool_e ACT_plate_manual_rotate(Uint16 angle);

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
bool_e ACT_ball_sorter_next_autoset_speed(Uint16 ball_launcher_speed_white_cherry__tr_min, bool_e keep_white_ball);

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

/* Actionneur associ�: ACT_QUEUE_Plier */
//Ouvre les pinces AX12 de tiny pour prendre des verres
bool_e ACT_plier_open();

//Ferme les pinces
bool_e ACT_plier_close();

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
