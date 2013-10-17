/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : queue.c
 *	Package : Actionneur
 *	Description : files d'actions actionneurs
 *  Auteurs : Jacen(modifié par Aurélien)
 *  Version 20110225
 */

#include "queue.h"
#include "config_pin.h"
#include "config_debug.h"

#ifndef OUTPUT_LOG_COMPONENT_QUEUE
#  define OUTPUT_LOG_COMPONENT_QUEUE LOG_PRINT_Off
#  warning "OUTPUT_LOG_COMPONENT_QUEUE is not defined, defaulting to Off"
#endif

#define LOG_PREFIX "queue: "
#define LOG_COMPONENT OUTPUT_LOG_COMPONENT_QUEUE
#include "QS/QS_outputlog.h"
#define component_printf_global(log_level, format, ...) component_printf(log_level, "[all] " format, ## __VA_ARGS__)
#define component_printf_queue(log_level, queueId, format, ...) component_printf(log_level, "[%d] " format, queueId, ## __VA_ARGS__)
#define component_printf_sem(log_level, queueId, semId, format, ...) component_printf(log_level, "[%d,s%d] " format, queueId, semId, ## __VA_ARGS__)


/* Structure queue qui comprend:
 * -les actions à effectuer
 * -arguments pour les actions
 * -actionneur utilisé
 * -index de la tête
 * -index de la queue
 * -instant de départ de l'action
 * -état de la queue (utilisée ou non)
 * -si une erreur est survenue lors de l'execution des fonctions dans la queue (erreur indiqué par QUEUE_set_error(queue_id))
 */

typedef struct{
	action_t action[QUEUE_SIZE];
	QUEUE_arg_t arg[QUEUE_SIZE];
	QUEUE_act_e act[QUEUE_SIZE];
	queue_size_t head;
	queue_size_t tail;
	time_t initial_time_of_current_action;
	bool_e used;
	bool_e error_occured;
} queue_t;



/*Sémaphore pour l'utilisation des actionneurs et la synchronisation des files entre elles:
 * -Token utilisé ou non
 * -Semaphore utilisable ou non(Sauf pour les premiers correspondant aux actionneurs)
 */

typedef struct{
	bool_e token;
	bool_e used;
}semaphore_t;

/*files que l'on peut utiliser*/
static queue_t queues[NB_QUEUE];

/*Sémaphore que l'on peut utiliser*/
static semaphore_t sems[NB_ACT+NB_SYNCHRO];

static volatile Uint16 time = 0;

void QUEUE_init()
{
	component_printf_global(LOG_LEVEL_Debug, "Init queues\n");
	static bool_e initialized = FALSE;
	if (initialized)
		return;
	Uint8 i;

	//initialisation des files
	for (i=0;i<NB_QUEUE; i++)
	{
		queues[i].used = FALSE;
	}

	//initialisation des sémaphores actionneurs
	for(i=0;i<NB_ACT;i++)
	{
		sems[i].used = TRUE;
		sems[i].token = TRUE;
	}

	//initialisation des sémaphores de synchronisation
	for(i=0;i<NB_SYNCHRO;i++)
	{
		sems[NB_ACT+i].used = FALSE;
	}

	CLOCK_init();
	component_printf_global(LOG_LEVEL_Debug, "Initialized\n");
	initialized = TRUE;
}


QUEUE_arg_t* QUEUE_get_arg(queue_id_t queue_id)
{
	assert((queue_id < NB_QUEUE)&&(queues[queue_id].used));
	return &(queues[queue_id].arg[queues[queue_id].head]);
}

QUEUE_act_e QUEUE_get_act(queue_id_t queue_id)
{
	assert((queue_id < NB_QUEUE) && (queues[queue_id].used));
	return queues[queue_id].act[queues[queue_id].head];
}

/*Renvoie le moment ou l'action a été initialisée*/
time_t QUEUE_get_initial_time(queue_id_t queue_id) {
	assert((queue_id < NB_QUEUE) && (queues[queue_id].used));
	return queues[queue_id].initial_time_of_current_action;
}

queue_id_t QUEUE_create()
{
	queue_id_t i;

	for(i=0; i<NB_QUEUE; i++)
	{
		if(!(queues[i].used))
			break;
	}

	if(i == NB_QUEUE) {
		component_printf_global(LOG_LEVEL_Warning, "No resource, can't create queue!\n");
		return QUEUE_CREATE_FAILED;
	}

	queues[i].head = 0;
	queues[i].tail = 0;
	queues[i].error_occured = FALSE;
	queues[i].used = TRUE;

	component_printf_queue(LOG_LEVEL_Info, i, "Created\n");

	return i;
}

void QUEUE_run()
{
	queue_id_t queue_id;
	queue_t* this;
	for (queue_id=0; queue_id<NB_QUEUE; queue_id++)
	{
		//Pour ne pas planter si la file est vide
		if (queues[queue_id].used && (queues[queue_id].head != queues[queue_id].tail))
		{
			this=&(queues[queue_id]);
			(this->action[this->head])(queue_id, FALSE);
			component_printf_queue(LOG_LEVEL_Debug, queue_id, "Run\n");
		}
	}
}


void QUEUE_add(queue_id_t queue_id, action_t action, QUEUE_arg_t optionnal_arg, QUEUE_act_e optionnal_act)
{
	queue_t* this=&(queues[queue_id]);
	//la file doit êre affectée
	assert((queue_id < NB_QUEUE)&&(queues[queue_id].used));
	// la file ne doit pas etre pleine
	assert((this->tail)< QUEUE_SIZE);

	component_printf_queue(LOG_LEVEL_Debug, queue_id, "Add\n");

	// on ajoute l'action à la file
	(this->action[this->tail]) = action;
	(this->arg[this->tail])= optionnal_arg;
	(this->act[this->tail])= optionnal_act;

	//On doit le faire avant d'appeler l'action, sinon si l'action appelle une fonction de ce module, il peut y avoir des problèmes. (bug testé avec un appel à QUEUE_behead)
	this->tail++;

	// si l'action est en tête de file
	if ((this->tail - 1) == this->head)
	{
		//on l'initialise
		component_printf_queue(LOG_LEVEL_Debug, queue_id, "Init action\n");
		this->initial_time_of_current_action = CLOCK_get_time();
		action(queue_id,TRUE);
	}

}

/* Retire la fonction en tete de file et reinitialise la suivante. */
void QUEUE_behead(queue_id_t queue_id)
{
	assert((queue_id < NB_QUEUE)&&(queues[queue_id].used));
	queue_t* this=&(queues[queue_id]);
	component_printf_queue(LOG_LEVEL_Debug, queue_id, "Next\n");

	this->head++;

	if(this->tail != this->head)
	{
		//on initialise l'action suivante
		component_printf_queue(LOG_LEVEL_Debug, queue_id, "Init action\n");
		this->initial_time_of_current_action = CLOCK_get_time();
		(this->action[this->head])(queue_id,TRUE);
		component_printf_queue(LOG_LEVEL_Debug, queue_id, "Queue empty\n");
	}
	else
	{
		//on supprime la file
		component_printf_queue(LOG_LEVEL_Info, queue_id, "Deleting\n");
		this->used = FALSE;
	}
}

/* Appelle la fonction OperationFinishedCallback et passe à la fonction suivante. Voir en haut du .h */
void QUEUE_next(queue_id_t queue_id, Uint11 act_sid, Uint8 result, Uint8 error_code, Uint16 param) {
	assert((queue_id < NB_QUEUE)&&(queues[queue_id].used));

	//Si il n'y a pas de fonction callback ou qu'elle retourne TRUE, on passe a l'action suivante. Sinon on indique une erreur dans la file
	if(QUEUE_get_arg(queue_id)->callback != NULL &&
	  !(QUEUE_get_arg(queue_id)->callback)(queue_id, act_sid, result, error_code, param))
	{
		QUEUE_set_error(queue_id);
	}

	QUEUE_behead(queue_id);
}

/* Indique qu'une erreur est survenue lors de l'execution d'une fonction dans la file indiquée. Les fonctions suivant dans la file pourront agir en conséquence. */
void QUEUE_set_error(queue_id_t queue_id) {
	assert(queue_id < NB_QUEUE);

	LED_ERROR = 1;

	if(queues[queue_id].used) {
		queues[queue_id].error_occured = TRUE;
		component_printf_queue(LOG_LEVEL_Warning, queue_id, "Error declared\n");
	} else {
		component_printf_queue(LOG_LEVEL_Error, queue_id, "Error in unused queue !\n");
	}
}

/* Retourne TRUE si une erreur est survenue lors de l'execution d'un fonction dans la file indiquée. */
bool_e QUEUE_has_error(queue_id_t queue_id) {
	assert(queue_id < NB_QUEUE);

	if(queues[queue_id].used) {
		return queues[queue_id].error_occured;
	}

	return FALSE;
}

/* vide la file */
void QUEUE_flush(queue_id_t queue_id)
{
	assert(queue_id < NB_QUEUE);
	//On flush seulement les files utilisées
	if(queues[queue_id].used)
	{
		component_printf_queue(LOG_LEVEL_Debug, queue_id, " Queue reset\n");
		sems[QUEUE_get_act(queue_id)].token = TRUE;	//On rend la sémaphore utilisée par l'actionneur
		queues[queue_id].used = FALSE; //On rend la file
	}
	else
	{
		component_printf_queue(LOG_LEVEL_Debug, queue_id, " Reset unused queue\n");
	}
}

/* vide toutes les files */
void QUEUE_flush_all()
{
	Uint8 i;
	component_printf_global(LOG_LEVEL_Info, "Reseting all queues\n");
	for (i=0; i<NB_QUEUE; i++)
		QUEUE_flush(i);
}
void QUEUE_take_sem(queue_id_t this, bool_e init)
{
	if(!init)
	{
		//Sémaphore pour actionneur
		if(QUEUE_get_arg(this)->param == 0)
		{
			if((sems[QUEUE_get_act(this)].token))
			{
				component_printf_sem(LOG_LEVEL_Info, this, QUEUE_get_act(this), "Acquiring act semaphore\n");
				sems[QUEUE_get_act(this)].token = FALSE;
				QUEUE_behead(this);
			}
			else
			{
				component_printf_sem(LOG_LEVEL_Debug, this, QUEUE_get_act(this), "Act semaphore locked\n");
			}
		}
		else//Sémaphore de synchronisation
		{
			if((sems[QUEUE_get_arg(this)->param].token))
			{
				component_printf_sem(LOG_LEVEL_Info, this, QUEUE_get_arg(this)->param, "Acquiring sync semaphore\n");
				sems[QUEUE_get_arg(this)->param].token = FALSE;
				QUEUE_behead(this);
			}
			else
			{
				component_printf_sem(LOG_LEVEL_Debug, this, QUEUE_get_arg(this)->param, "Sync semaphore locked\n");
			}
		}
	}
}
void QUEUE_give_sem(queue_id_t this, bool_e init)
{
	if(!init)
	{
		//Sémaphore pour file
		if(QUEUE_get_arg(this)->param == 0)
		{
				component_printf_sem(LOG_LEVEL_Info, this, QUEUE_get_act(this), "Releasing act semaphore\n");
				sems[QUEUE_get_act(this)].token = TRUE;
				QUEUE_behead(this);
		}
		else//Sémaphore de synchronisation
		{
				component_printf_sem(LOG_LEVEL_Info, this, QUEUE_get_arg(this)->param, "Releasing sync semaphore\n");
				sems[QUEUE_get_arg(this)->param].token = TRUE;
				QUEUE_behead(this);
		}
	}
}

sem_id_t QUEUE_sem_create()
{
	Uint8 i;

	for(i=0;i<NB_ACT+NB_SYNCHRO;i++)
	{
		if(!(sems[i].used))
			break;
	}
	assert(i>=NB_ACT); //Pour ne pas prendre les sémaphores réservées aux actionneurs
	sems[i].used = TRUE;
	sems[i].token = TRUE;
	debug_printf("Creation synchro %d\n",i);

	return (i!=NB_ACT+NB_SYNCHRO)?i:QUEUE_SEM_CREATE_FAILED;
}

void QUEUE_sem_delete(queue_id_t this, bool_e init)
{
	if(!init)
	{
		sems[QUEUE_get_arg(this)->param].used = FALSE;
		QUEUE_behead(this);
	}
}

void QUEUE_wait_synchro(queue_id_t this, bool_e init)
{
	static time_t initial_time;
	if(init)
	{
		debug_printf("Départ Attente synchro\n");
		initial_time = CLOCK_get_time();
	}
	else
	{
		debug_printf("Attente synchro\n");
		if ((sems[QUEUE_get_arg(this)->param].used && sems[QUEUE_get_arg(this)->param].token)) //jeton rendu ou timout
		{
			debug_printf("Synchro finie\n");
			QUEUE_behead(this);
		}
		if(CLOCK_get_time() - initial_time > QUEUE_SYNCHRO_TIMEOUT)
		{
			debug_printf("Synchro timeout\n");
			//On supprime la synchro qu'on attend
			sems[QUEUE_get_arg(this)->param].used = FALSE;
			sems[QUEUE_get_arg(this)->param].token = TRUE;
			QUEUE_flush(this);
		}
	}
}
