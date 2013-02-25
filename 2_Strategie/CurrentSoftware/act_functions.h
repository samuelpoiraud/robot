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

	//Info sur la gestion d'erreur des actionneurs:
	//La carte actionneur génère des resultats et détail les erreurs suivant ce qu'elle sait et les envois par message CAN avec ACT_RESULT
	//La fonction ACT_process_result (act_function.c) converti les messages ACT_RESULT en ces valeurs dans act_t::operationResult et act_t::recommendedBehavior (environnement.h)
	//La fonction ACT_check_result (act_function.c) converti et gère les messages act_t::operationResult et act_t::recommendedBehavior en information ACT_function_result_e (dans act_function.h) pour être ensuite utilisé par le reste du code stratégie.


	//Resultat de la dernière opération en cours d'exécution. Utilisé pour connaitre l'état après une erreur surtout.
	typedef enum {
		ACT_FUNCTION_InProgress,   //L'opération ne s'est pas encore finie
		ACT_FUNCTION_Done,         //L'opération s'est finie correctement. C'est aussi la valeur au démarrage lorsque aucune action n'a encore été faite.
		ACT_FUNCTION_ActDisabled,  //L'opération n'a pas pu se terminer: l'actionneur ne marche plus
		ACT_FUNCTION_RetryLater    //L'opération n'a pas pu se terminer: il faudra retenter plus tard (plus tard = faire autre chose avant de retest, pas juste 3ms, causé par un position impossible à atteindre, robot adverse qui bloque ? positionnement en mode loto trop près du bord ?)
	} ACT_function_result_e;
	
	//Fonction générique permettant de positionner les actionneurs
	//void ACT_push_set_pos(ACT_id_e act, ACT_position_e position, bool_e run);
	
	//void ACT_set_pos(stack_id_e stack_id, bool_e init);

	// Gère les messages de retour de la carte actionneur lorsque une action s'est terminé ou à échouée
	void ACT_process_result(const CAN_msg_t* msg);

	// Récupère le resultat de la dernière action associé à une pile (non valable pour ASSER) Cette valeur ne change pas tant qu'aucune opération ne commence ou se finie.
	// A utiliser après la detection d'une erreur lors de l'exécution d'une pile pour savoir quoi faire par la suite (reporter l'action ou l'annuler)
	ACT_function_result_e ACT_get_last_action_result(stack_id_e act_id);
	
	//simple envoi du message 
	void ACT_hammer_up(void);
	void ACT_hammer_down(void);
	void ACT_hammer_tidy(void);

	void ACT_ball_grabber_up(void);
	void ACT_ball_grabber_down(void);
	void ACT_ball_grabber_tidy(void);

	/* Fonctions empilables */

	//Actionneur associé: ACT_STACK_BallLauncher
	bool_e ACT_push_ball_launcher_stop(bool_e run);
	bool_e ACT_push_ball_launcher_run(Uint16 speed, bool_e run);

#endif /* ndef ACT_FUNCTIONS_H */
