/*  Club Robot ESEO 2012 - 2013
 *	Krusty
 *
 *	Fichier : Ball_sorter.h
 *	Package : Carte actionneur
 *	Description : Gestion de la detection des balles cerises et de leur envoi une par une.
 *  Auteur : Alexis
 *  Version 20130315
 *  Robot : Krusty
 */

#ifndef KBALL_SORTER_H
#define	KBALL_SORTER_H

#include "../QS/QS_all.h"
#ifdef I_AM_ROBOT_KRUSTY

#include "../queue.h"

//Etape d'un passage de cerise (demand� par la strat)
//A mettre dans le param dans la queue
typedef enum {
	BALLSORTER_CS_CheckLauncherSpeed = 0, //Verifie que le lanceur de balle � atteint sa vitesse (avant d'envoyer la cerise)
	BALLSORTER_CS_EjectCherry = 1,        //Ejecte la cerise prise
	BALLSORTER_CS_GotoNextCherry = 2,     //Va prendre une nouvelle cerise dans le bac
	BALLSORTER_CS_TakeCherry = 3,         //Met la cerise prise devant le capteur
	BALLSORTER_CS_DetectCherry = 4        //Detecte la cerise pour savoir si elle est blanche ou pas et renvoi le resultat � la strat
} BALLSORTER_command_state_e;

/** Initialisation du gestionnaire de l'assiette.
 *
 * Configure l'AX12 qui sert � prendre les cerises une par une
 */
void BALLSORTER_init();

/** Stoppe l'actionneur.
 *
 * Cette fonction est appel�e en fin de match
 */
void BALLSORTER_stop();

/** G�re les messages CAN li�s � la gestion des cerises � envoyer.
 *
 * Cette fonction s'occupe toute seule de g�rer la queue et d'agir en cons�quence.
 * @param msg le message CAN
 * @return TRUE si le message CAN a �t� g�r� par cet actionneur, FALSE sinon
 */
bool_e BALLSORTER_CAN_process_msg(CAN_msg_t* msg);

/** G�re les commandes demand�es.
 *
 * Fonction � mettre sur la file pour demander une action.
 */
void BALLSORTER_run_command(queue_id_t queueId, bool_e init);

#endif  /* I_AM_ROBOT_KRUSTY */
#endif	/* KBALL_SORTER_H */
