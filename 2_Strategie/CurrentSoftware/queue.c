/*
 *	Club Robot ESEO 2012 - 2013
 *	Krusty & Tiny
 *
 *	Fichier : queue.c
 *	Package : Carte Principale (Stratégie)
 *	Description : files d'actions actionneurs
 *  Auteur : Alexis (basé sur queue.c/h de la carte actionneur (18/03/2013, Jacen(modifié par Aurélien)))
 *  Version 20130318
 */

#include "queue.h"

//Il y aura toujours une case vide pour pouvoir detecter quand la file est vide ou pleine.
#define REAL_QUEUE_SIZE (QUEUE_SIZE+1)


/* Structure queue qui comprend: 
 * -les actions à effectuer
 * -arguments pour les actions
 * -index de la tête
 * -index de la queue
 * -instant de départ de l'action
 * -si une erreur est survenue lors de l'execution des fonctions dans la queue (erreur indiqué par QUEUE_set_error(queue_id))
 */

//read and increment
//write and increment
typedef struct{
	action_function_t action[REAL_QUEUE_SIZE];
	QUEUE_arg_t arg[REAL_QUEUE_SIZE];
	queue_size_t start_index;   //Début de la file (la ou on ajoute)
	queue_size_t end_index;     //Fin de la file (la ou on execute)
	time32_t initial_time_of_current_action;
	bool_e error_occured;
} queue_t;

/*files que l'on peut utiliser*/
static queue_t queues[NB_QUEUE];

#define INC_WITH_MOD(val,div) (( (val)+1 < (div) )? (val)+1 : 0)
static bool_e QUEUE_is_full(queue_id_e queue_id);
static bool_e QUEUE_is_empty(queue_id_e queue_id);
static void   QUEUE_inc_index(queue_size_t* index);
//static volatile Uint16 time = 0;

void QUEUE_init()
{
	//debug_printf("Init file\n");
	static bool_e initialized = FALSE;
	if (initialized)
		return;
	initialized = TRUE;

	Uint8 i, j;
	
	//initialisation des files
	for (i=0;i<NB_QUEUE; i++) {
		for(j=0; j<REAL_QUEUE_SIZE; j++) {
			queues[i].action[j] = 0;
			queues[i].arg[j] = (QUEUE_arg_t){0};
		}
		queues[i].start_index = 0;
		queues[i].end_index = 0;
		queues[i].error_occured = FALSE;
		queues[i].initial_time_of_current_action = 0;
	}

	CLOCK_init();
	//debug_printf("QUEUE_init\n");
}

static bool_e QUEUE_is_full(queue_id_e queue_id) {
	return (INC_WITH_MOD(queues[queue_id].start_index, REAL_QUEUE_SIZE) == queues[queue_id].end_index);
}

static bool_e QUEUE_is_empty(queue_id_e queue_id) {
	return (queues[queue_id].start_index == queues[queue_id].end_index);
}

static void QUEUE_inc_index(queue_size_t* index) {
	if(index)
		*index = INC_WITH_MOD((*index), REAL_QUEUE_SIZE);
}

QUEUE_arg_t* QUEUE_get_arg(queue_id_e queue_id)
{
	assert(queue_id < NB_QUEUE);
	return &(queues[queue_id].arg[queues[queue_id].end_index]);
}

/*Renvoie le moment ou l'action a été initialisée*/
time32_t QUEUE_get_initial_time(queue_id_e queue_id) {
	assert(queue_id < NB_QUEUE);
	return queues[queue_id].initial_time_of_current_action;
}

void QUEUE_run()
{
	queue_id_e queue_id;
	queue_t* this;
	for (queue_id=0; queue_id < NB_QUEUE; queue_id++)
	{
		//Pour ne pas planter si la file est vide
		if (!QUEUE_is_empty(queue_id))
		{
			this = &(queues[queue_id]);
			(this->action[this->end_index])(queue_id, FALSE);
			//debug_printf("QUEUE%d_run\n", queue_id);
		}
	}
}


bool_e QUEUE_add(queue_id_e queue_id, action_function_t action, QUEUE_arg_t arg)
{
	queue_t* this = &(queues[queue_id]);
	bool_e is_first_action;
	assert(queue_id < NB_QUEUE);

	if(QUEUE_is_full(queue_id))
		return FALSE;

	is_first_action = QUEUE_is_empty(queue_id); //Si oui, on execute directement l'intialisation de l'action ajouté
	
	// on ajoute l'action à la file
	this->action[this->start_index] = action;
	this->arg[this->start_index] = arg;

	//On doit le faire avant d'appeler l'action, sinon si l'action appelle une fonction de ce module, il peut y avoir des problèmes. (bug testé avec un appel à QUEUE_next)

	QUEUE_inc_index(&this->start_index);

	// si l'action est en tête de file
	if(is_first_action)
	{
		//on l'initialise
		//debug_printf("Initialise première action\n");
		this->initial_time_of_current_action = global.env.match_time;
		action(queue_id, TRUE);
	}

	return TRUE;
}

/* Retire la fonction en tete de file et reinitialise la suivante. */
void QUEUE_next(queue_id_e queue_id)
{
	assert(queue_id < NB_QUEUE);
	queue_t* this = &(queues[queue_id]);

	debug_printf("queue: before inc\n");
	QUEUE_inc_index(&this->end_index);
	debug_printf("queue: after inc\n");

	if(!QUEUE_is_empty(queue_id))
	{debug_printf("queue: not empty\n");
		//on initialise l'action suivante
		this->initial_time_of_current_action = global.env.match_time;
		(this->action[this->end_index])(queue_id, TRUE);
		debug_printf("queue: act called\n");
		//debug_printf("Off with his head ! %d\n", queue_id);
	}
}

/* Indique qu'une erreur est survenue lors de l'execution d'une fonction dans la file indiquée. Les fonctions suivant dans la file pourront agir en conséquence. */
void QUEUE_set_error(queue_id_e queue_id, bool_e error) {
	assert(queue_id < NB_QUEUE);

	queues[queue_id].error_occured = error;
	debug_printf("Erreur déclarée dans la file %d\n", queue_id);
}

/* Retourne TRUE si une erreur est survenue lors de l'execution d'un fonction dans la file indiquée. */
bool_e QUEUE_has_error(queue_id_e queue_id) {
	assert(queue_id < NB_QUEUE);

	return queues[queue_id].error_occured;
}

/* vide la file */
void QUEUE_reset(queue_id_e queue_id)
{
	assert(queue_id < NB_QUEUE);

	queues[queue_id].start_index = 0;
	queues[queue_id].end_index = 0;
	queues[queue_id].error_occured = FALSE;

	debug_printf("/!\\ Queue %d flush\n",queue_id);
	//TODO: finish here
}

/* vide toutes les files */
void QUEUE_reset_all()
{
	Uint8 i;
	for (i=0; i<NB_QUEUE; i++)
		QUEUE_reset(i);
}
