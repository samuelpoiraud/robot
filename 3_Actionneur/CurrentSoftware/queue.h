/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : queue.h
 *	Package : Actionneur
 *	Description : files d'actions actionneurs
 *  Auteurs : Jacen (modifi� par Aur�lien)
 *  Version 20110225
 */

#include "QS/QS_all.h"

#ifndef QUEUE_H
#define QUEUE_H

#include "clock.h"

	//TODO: A deporter dans *Global_vars_types.h
	typedef enum 
	{
		BALL_GRABBER=0,
		HAMMER,
		QUEUE_ACT_BallLauncher,	//avec QUEUE devant, n'importe ou dans le code on sait que �a fait r�f�rence � un actionneur li� � queue.h/c
		QUEUE_ACT_LongHammer,
		QUEUE_ACT_Plate_Rotation,
		QUEUE_ACT_Plate_AX12_Plier,
		NB_ACT
		
	} QUEUE_act_e;
	
typedef Uint8 queue_size_t;
typedef Uint8 queue_id_t;
typedef Sint16 sem_id_t;

#define NB_QUEUE	8 
#define QUEUE_SIZE	64
#define NB_SYNCHRO 8
#define QUEUE_SYNCHRO_TIMEOUT 150 //15 secondes

// Valeur renvoy�e par QUEUE_create si l'op�ration echoue
#define QUEUE_CREATE_FAILED	0xFF

//Valeur renvoy� par QUEUE_sem_create si l'op�ration echoue
#define QUEUE_SEM_CREATE_FAILED	0xFF

/*
toute action contenant des variables statiques doit �tre utilis�e
avec un seul actionneur (prot�g� par s�maphore)
*/
typedef void(*action_t)(queue_id_t this, bool_e init);

void QUEUE_init();

/*Renvoie l'argument associ� � la fonction*/
Sint16 QUEUE_get_arg(queue_id_t queue_id);

/*Renvoie l'actionneur utilis�*/
QUEUE_act_e QUEUE_get_act(queue_id_t queue_id);

/*Renvoie si la file rentr�e en param�tre est disponible*/
bool_e is_available(queue_id_t queue_id);

/*Cr�ation d'une file*/
queue_id_t QUEUE_create();

/*fonction de gestion des files*/
void QUEUE_run();

/*Ajout d'une action dans une file*/
void QUEUE_add(queue_id_t queue_id, action_t action, Sint16 optionnal_arg,QUEUE_act_e optionnal_act);

/* Retire la fonction en tete de file et reinitialise la suivante. */
void QUEUE_behead(queue_id_t queue_id);

/* vide la file et rend la semaphore de l'actionneur associ�*/
void QUEUE_flush(queue_id_t queue_id);

/* vide toutes les files */
void QUEUE_flush_all();

/*Prend l'actionneur (s�maphore)*/
void QUEUE_take_sem(queue_id_t this, bool_e init);

/*Lib�re l'actionneur*/
void QUEUE_give_sem(queue_id_t this, bool_e init);

/*Prend la s�maphore de synchronisation*/
sem_id_t QUEUE_sem_create();

/*Rend la s�maphore de synchronisation*/
void QUEUE_sem_delete(queue_id_t this, bool_e init);

/*Attend la synchronisation*/
void QUEUE_wait_synchro(queue_id_t this, bool_e init);

#endif /* ndef QUEUE_H */
