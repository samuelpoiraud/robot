/*
 *	Club Robot ESEO 2009 - 2010
 *	CHOMP
 *
 *	Fichier : act_functions.c
 *	Package : Carte Principale
 *	Description : 	Fonctions de gestion de la pile
 *					de l'actionneur
 *	Auteur : Julien et Ronan
 *	Version 20110313
 */

#include "act_functions.h"
#include "act_can.h"

#include "config_debug.h"

#ifndef OUTPUT_LOG_COMPONENT_ACTFUNCTION
#  define OUTPUT_LOG_COMPONENT_ACTFUNCTION LOG_PRINT_Off
#  warning "OUTPUT_LOG_COMPONENT_ACTFUNCTION is not defined, defaulting to Off"
#endif

#define LOG_PREFIX "act_f: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_ACTFUNCTION
#include "QS/QS_outputlog.h"

/* Pile contenant les arguments d'une demande d'op�ration
 * Contient les messages CAN � envoyer � la carte actionneur pour ex�cuter l'action.
 * fallbackMsg contient le message CAN lorsque l'op�ration demand� par le message CAN msg ne peut pas �tre compl�t� (bras bloqu�, robot adverse qui bloque l'actionneur par exemple)
 * On utilise une structure diff�rente de CAN_msg_t pour �conomiser la RAM (voir matrice act_args)
 *
 * Pour chaque fonction appel�e par le code strat�gie pour empiler une action, on remplie les messages CAN � enovoyer et on empile l'action.
 * Un timeout est disponible si jamais la carte actionneur ne renvoie pas de message CAN ACT_RESULT (elle le devrait, mais on sait jamais)
 *
 * Le code de ACT_check_result() g�re le renvoi de message lorsque la carte actionneur indique qu'il n'y avait pas assez de ressource ou l'envoi du message fallback si la position demand� par le message CAN msg n'est pas atteignable.
 * Ce le message fallback se solve par un echec aussi, on indique le code de strat�gie que cet actionneur n'est pas dispo maintenant et de r�essayer plus tard (il faut faire une autre strat pendant ce temps, si c'est un robot qui bloque le bras, il faut que l'environnement du jeu bouge)
 * Si le renvoi du message � cause du manque de ressource cause la m�me erreur, on marque l'actionneur comme inutilisable (ce cas est grave, il y a probablement un probl�me dans le code actionneur ou un flood de demande d'op�ration de la carte strat�gie)
 */


//Info sur la gestion d'erreur des actionneurs:
//La carte actionneur g�n�re des resultats et d�tail les erreurs suivant ce qu'elle sait et les envois par message CAN avec ACT_RESULT
//La fonction ACT_process_result (act_function.c) converti les messages ACT_RESULT en ces valeurs dans act_state_info_t::operationResult et act_state_info_t::recommendedBehavior (environnement.h)
//La fonction ACT_check_result (act_function.c) converti et g�re les messages act_state_info_t::operationResult et act_state_info_t::recommendedBehavior en information ACT_function_result_e (dans act_function.h) pour �tre ensuite utilis� par le reste du code strat�gie.



//FONCTIONS D'ACTIONNEURS


// PIERRE

bool_e ACT_fruit_mouth_goto(ACT_fruit_mouth_cmd_e cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_FRUIT_MOUTH, cmd);
	ACT_arg_set_fallbackmsg(&args, ACT_FRUIT_MOUTH, ACT_FRUIT_MOUTH_STOP);

	debug_printf("Pushing Fruit Run cmd\n");
	return ACT_push_operation(ACT_QUEUE_Fruit, &args);
}

// <editor-fold desc="Krusty">

bool_e ACT_ball_launcher_run(Uint16 speed) {
	QUEUE_arg_t args;

	ACT_arg_init_with_param(&args, ACT_BALLLAUNCHER, ACT_BALLLAUNCHER_ACTIVATE, speed);
	ACT_arg_set_fallbackmsg(&args, ACT_BALLLAUNCHER, ACT_BALLLAUNCHER_STOP);

	debug_printf("Pushing BallLauncher Run cmd, speed: %u\n", speed);
	return ACT_push_operation(ACT_QUEUE_BallLauncher, &args);
}

bool_e ACT_ball_launcher_stop() {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_BALLLAUNCHER, ACT_BALLLAUNCHER_STOP);

	debug_printf("Pushing BallLauncher Stop cmd\n");
	return ACT_push_operation(ACT_QUEUE_BallLauncher, &args);
}

bool_e ACT_plate_rotate(ACT_plate_rotate_cmd_t cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_PLATE, cmd);

	//Si on ne peut pas aller en position, revenir en vertical et la strat pourra passer � autre chose (au lieu de d�foncer le d�cor)
	if(cmd != ACT_PLATE_RotateUp)
		ACT_arg_set_fallbackmsg(&args, ACT_PLATE, ACT_PLATE_ROTATE_VERTICALLY);

	debug_printf("Pushing Plate rotate cmd: %d\n", cmd);
	return ACT_push_operation(ACT_QUEUE_Plate, &args);
}

bool_e ACT_plate_manual_rotate(Uint16 angle) {
	QUEUE_arg_t args;

	ACT_arg_init_with_param(&args, ACT_PLATE, ACT_PLATE_ROTATE_ANGLE, angle);

	ACT_arg_set_fallbackmsg(&args, ACT_PLATE, ACT_PLATE_ROTATE_VERTICALLY);

	debug_printf("Pushing Plate rotate angle: %d\n", angle);
	return ACT_push_operation(ACT_QUEUE_Plate, &args);
}

bool_e ACT_plate_plier(ACT_plate_plier_cmd_t cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_PLATE, cmd);

	//Si on ne peut pas aller en position, fermer la pince et la strat pourra passer � autre chose (au lieu de d�foncer le robot en remontant la pince en �tant ouverte (�a passe pas))
	if(cmd != ACT_PLATE_PlierClose)
		ACT_arg_set_fallbackmsg(&args, ACT_PLATE, ACT_PLATE_PLIER_CLOSE);

	debug_printf("Pushing Plate plier cmd: %d\n", cmd);
	return ACT_push_operation(ACT_QUEUE_Plate, &args);
}

bool_e ACT_lift_translate(ACT_lift_pos_t lift_id, ACT_lift_translate_cmd_t cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, lift_id, cmd);

	if(lift_id == ACT_LIFT_Left) {
		debug_printf("Pushing left Lift translate cmd: %d\n", cmd);
		return ACT_push_operation(ACT_QUEUE_LiftLeft, &args);
	} else {
		debug_printf("Pushing right Lift translate cmd: %d\n", cmd);
		return ACT_push_operation(ACT_QUEUE_LiftRight, &args);
	}
}

bool_e ACT_lift_plier(ACT_lift_pos_t lift_id, ACT_lift_plier_cmd_t cmd) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, lift_id, cmd);

	if(lift_id == ACT_LIFT_Left) {
		debug_printf("Pushing left Lift plier cmd: %d\n", cmd);
		return ACT_push_operation(ACT_QUEUE_LiftLeft, &args);
	} else {
		debug_printf("Pushing right Lift plier cmd: %d\n", cmd);
		return ACT_push_operation(ACT_QUEUE_LiftRight, &args);
	}
}

bool_e ACT_ball_sorter_next() {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_BALLSORTER, ACT_BALLSORTER_TAKE_NEXT_CHERRY);
	ACT_arg_set_timeout(&args, 5000);

	debug_printf("Pushing BallSorter next cmd\n");
	return ACT_push_operation(ACT_QUEUE_BallSorter, &args);
}

bool_e ACT_ball_sorter_next_autoset_speed(Uint16 ball_launcher_speed_white_cherry__tr_min, bool_e keep_white_ball) {
	QUEUE_arg_t args;

	ACT_arg_init_with_param(&args, ACT_BALLSORTER, ACT_BALLSORTER_TAKE_NEXT_CHERRY, ( (ball_launcher_speed_white_cherry__tr_min & 0x7FFF) | ( ((Uint16)(keep_white_ball != 0)) << 15 ) ) );
	ACT_arg_set_timeout(&args, 5000);

	debug_printf("Pushing BallSorter next autoset speed cmd\n");
	return ACT_push_operation(ACT_QUEUE_BallSorter, &args);
}

// </editor-fold>

// <editor-fold desc="Tiny">
bool_e ACT_hammer_goto(Uint16 position) {
	QUEUE_arg_t args;

	ACT_arg_init_with_param(&args, ACT_HAMMER, ACT_HAMMER_MOVE_TO, position);

	//Que faire si on ne peut pas bouger le bras ... (rien ici)

	debug_printf("Pushing Hammer goto cmd\n");
	return ACT_push_operation(ACT_QUEUE_Hammer, &args);
}

bool_e ACT_hammer_blow_candle() {
	QUEUE_arg_t args;

	ACT_arg_init_with_param(&args, ACT_HAMMER, ACT_HAMMER_BLOW_CANDLE, global.env.color);

	debug_printf("Pushing Hammer blow candle cmd\n");
	return ACT_push_operation(ACT_QUEUE_Hammer, &args);
}

bool_e ACT_hammer_stop() {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_HAMMER, ACT_HAMMER_STOP);

	debug_printf("Pushing Hammer stop asser cmd\n");
	return ACT_push_operation(ACT_QUEUE_Hammer, &args);
}

bool_e ACT_plier_open() {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_PLIER, ACT_PLIER_OPEN);

	debug_printf("Pushing Plier open cmd\n");
	return ACT_push_operation(ACT_QUEUE_Plier, &args);
}

bool_e ACT_plier_close() {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_PLIER, ACT_PLIER_CLOSE);

	debug_printf("Pushing Plier close cmd\n");
	return ACT_push_operation(ACT_QUEUE_Plier, &args);
}

void ACT_ball_inflater_inflate(Uint8 duration_sec)
{
	CAN_msg_t msg;
	msg.sid = ACT_BALLINFLATER;
	msg.data[0] = ACT_BALLINFLATER_START;
	msg.data[1] = duration_sec;	//[sec]
	msg.size = 2;
	CAN_send(&msg);
}

void ACT_ball_inflater_stop(void)
{
	CAN_msg_t msg;
	msg.sid = ACT_BALLINFLATER;
	msg.data[0] = ACT_BALLINFLATER_STOP;
	msg.size = 1;
	CAN_send(&msg);
}
/*
//Le gonflage du ballon doit se produire � la fin du match : les fonctionnalit�s de QUEUE n'y sont plus op�rationelles !
// -> voir T_BALLINFLATER_start() !
bool_e ACT_ball_inflater_inflate(Uint8 duration_sec) {
	QUEUE_arg_t args;

	ACT_arg_init_with_param(&args, ACT_BALLINFLATER, ACT_BALLINFLATER_START, duration_sec);
	ACT_arg_set_fallbackmsg(&args, ACT_BALLINFLATER, ACT_BALLINFLATER_STOP);

	debug_printf("Pushing BallInflater inflate cmd\n");
	return ACT_push_operation(ACT_QUEUE_BallInflater, &args);
}

bool_e ACT_ball_inflater_stop() {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_BALLINFLATER, ACT_BALLINFLATER_STOP);

	debug_printf("Pushing BallInflater stop cmd\n");
	return ACT_push_operation(ACT_QUEUE_BallInflater, &args);
}
*/
bool_e ACT_candlecolor_get_color_at(ACT_candlecolor_pos_t candle_pos) {
	QUEUE_arg_t args;

	ACT_arg_init(&args, ACT_CANDLECOLOR, candle_pos);

	debug_printf("Pushing CandleColor get color at %d cmd\n", candle_pos);
	return ACT_push_operation(ACT_QUEUE_CandleColor, &args);
}

// </editor-fold>
