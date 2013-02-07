/*
 *	Club Robot ESEO 2010 - 2011
 *	Check Norris
 *
 *	Fichier : queue.c
 *	Package : Actionneur
 *	Description : files d'actions actionneurs
 *  Auteurs : Jacen(modifi� par Aur�lien)
 *  Version 20110225
 */

#define QUEUE_C
#include "queue.h"


/* Structure queue qui comprend: 
 * -les actions � effectuer
 * -arguments pour les actions
 * -actionneur utilis�
 * -index de la t�te
 * -index de la queue
 * -instant de d�part de l'action
 * -�tat de la queue (utilis�e ou non)
 */
 
typedef struct{
	action_t action[QUEUE_SIZE];
	Sint16 arg[QUEUE_SIZE];
	act_e act[QUEUE_SIZE];
	queue_size_t head;
	queue_size_t tail;
	time_t initial_time_of_current_action;
	bool_e used;
} queue_t;



/*S�maphore pour l'utilisation des actionneurs et la synchronisation des files entre elles:
 * -Token utilis� ou non
 * -Semaphore utilisable ou non(Sauf pour les premiers correspondant aux actionneurs)
 */

typedef struct{
	bool_e token;
	bool_e used;
}semaphore_t;

/*files que l'on peut utiliser*/
static queue_t queues[NB_QUEUE];

/*S�maphore que l'on peut utiliser*/
static semaphore_t sems[NB_ACT+NB_SYNCHRO];
 
static volatile Uint16 time = 0;

void QUEUE_init()
{
	//debug_printf("Init file\r\n");
	static bool_e initialized = FALSE;
	if (initialized)
		return;
	Uint8 i;
	
	//initialisation des files
	for (i=0;i<NB_QUEUE; i++)
	{
		queues[i].used = FALSE;
	}
	
	//initialisation des s�maphores actionneurs
	for(i=0;i<NB_ACT;i++)
	{
		sems[i].used = TRUE;
		sems[i].token = TRUE;
	}
	
	//initialisation des s�maphores de synchronisation
	for(i=0;i<NB_SYNCHRO;i++)
	{
		sems[NB_ACT+i].used = FALSE;
	}

	CLOCK_init();
	//debug_printf("QUEUE_init\r\n");
	initialized = TRUE;
}


Sint16 QUEUE_get_arg(queue_id_t queue_id)
{
	assert((queue_id < NB_QUEUE)&&(queues[queue_id].used));
	return queues[queue_id].arg[queues[queue_id].head];
}		

act_e QUEUE_get_act(queue_id_t queue_id)
{
	assert((queue_id < NB_QUEUE) && (queues[queue_id].used));
	return queues[queue_id].act[queues[queue_id].head];
}

queue_id_t QUEUE_create()
{
	queue_id_t i;

	for(i=0; i<NB_QUEUE; i++)
	{
		if(!(queues[i].used))
			break;
	}

	if(i == NB_QUEUE) return QUEUE_CREATE_FAILED;

	queues[i].head = 0;
	queues[i].tail = 0;
	queues[i].used = TRUE;

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
			//debug_printf("QUEUE%d_run\r\n", queue_id);
		}
	}
}


void QUEUE_add(queue_id_t queue_id, action_t action, Sint16 optionnal_arg,act_e optionnal_act)
{
	queue_t* this=&(queues[queue_id]);
	//la file doit �re affect�e
	assert((queue_id < NB_QUEUE)&&(queues[queue_id].used));
	// la file ne doit pas etre pleine
	assert((this->tail)< QUEUE_SIZE);
	
	
	// on ajoute l'action � la file
	(this->action[this->tail]) = action;
	(this->arg[this->tail])= optionnal_arg;
	(this->act[this->tail])= optionnal_act;
	
	// si l'action est en t�te de file
	if (this->tail == this->head)
	{
		//on l'initialise
		//debug_printf("Initialise premi�re action\n\r");
		action(queue_id,TRUE);
	}
	
	this->tail++;
}

/* Retire la fonction en tete de file et reinitialise la suivante. */
void QUEUE_behead(queue_id_t queue_id)
{
	assert((queue_id < NB_QUEUE)&&(queues[queue_id].used));
	queue_t* this=&(queues[queue_id]);

	this->head++;

	if(this->tail != this->head)
	{
		//on initialise l'action suivante
		(this->action[this->head])(queue_id,TRUE);
		//debug_printf("Off with his head ! %d\n", queue_id);
	}
	else
	{
		//on supprime la file
		debug_printf("Suppression file %d\r\n",queue_id);
		this->used = FALSE;
	}
}

/* vide la file */
void QUEUE_flush(queue_id_t queue_id)
{
	assert(queue_id < NB_QUEUE);
	//On flush seulement les files utilis�es
	if(queues[queue_id].used)
	{
		debug_printf("Queue %d flush\r\n",queue_id);
		sems[QUEUE_get_act(queue_id)].token= TRUE;	//On rend la s�maphore utilis�e par l'actionneur
		queues[queue_id].used = FALSE; //On rend la file
	}
	else
	{
		debug_printf("File %d non utilis�e \r\n",queue_id);
	}
}

/* vide toutes les files */
void QUEUE_flush_all()
{
	Uint8 i;
	for (i=0; i<NB_QUEUE; i++)
		QUEUE_flush(i);
}
void QUEUE_take_sem(queue_id_t this, bool_e init)
{
	if(!init)
	{
		//S�maphore pour actionneur
		if(QUEUE_get_arg(this)==0)
		{
			if((sems[QUEUE_get_act(this)].token))
			{
				debug_printf("Prise s�maphore actionneur %d\n\r",QUEUE_get_act(this));
				sems[QUEUE_get_act(this)].token = FALSE;
				QUEUE_behead(this);
			}	
			else
				debug_printf("Actuator %d already used\r\n",this);
		}
		else//S�maphore de synchronisation
		{
			if((sems[QUEUE_get_arg(this)].token))
			{
				debug_printf("Prise s�maphore synchro %d\n\r",QUEUE_get_arg(this));
				sems[QUEUE_get_arg(this)].token = FALSE;
				QUEUE_behead(this);
			}	
			else
			{
				debug_printf("Synchro %d already used",QUEUE_get_arg(this));
			}
		}
	}
}
void QUEUE_give_sem(queue_id_t this, bool_e init)
{
	if(!init)
	{
		//S�maphore pour file
		if(QUEUE_get_arg(this)==0)
		{
				debug_printf("S�maphore actionneur %d rendue\n\r",QUEUE_get_act(this));
				sems[QUEUE_get_act(this)].token = TRUE;
				QUEUE_behead(this);
		}
		else//S�maphore de synchronisation
		{
				debug_printf("S�maphore synchro %d rendue\n\r",QUEUE_get_arg(this));
				sems[QUEUE_get_arg(this)].token = TRUE;
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
	assert(i>=NB_ACT); //Pour ne pas prendre les s�maphores r�serv�es aux actionneurs
	sems[i].used = TRUE;
	sems[i].token = TRUE;
	debug_printf("Creation synchro %d \r\n",i);
		
	return (i!=NB_ACT+NB_SYNCHRO)?i:QUEUE_SEM_CREATE_FAILED;
}

void QUEUE_sem_delete(queue_id_t this, bool_e init)
{
	if(!init)
	{
		sems[QUEUE_get_arg(this)].used = FALSE;
		QUEUE_behead(this);
	}
}

void QUEUE_wait_synchro(queue_id_t this, bool_e init)
{
	static time_t initial_time;
	if(init)
	{
		debug_printf("D�part Attente synchro\r\n");
		initial_time = CLOCK_get_time();
	}
	else
	{
		debug_printf("Attente synchro\r\n");
		if ((sems[QUEUE_get_arg(this)].used && sems[QUEUE_get_arg(this)].token)) //jeton rendu ou timout
		{	
			debug_printf("Synchro finie\r\n");
			QUEUE_behead(this);
		}
		if(CLOCK_get_time() - initial_time > QUEUE_SYNCHRO_TIMEOUT)
		{
			debug_printf("Synchro timeout\r\n");
			//On supprime la synchro qu'on attend
			sems[QUEUE_get_arg(this)].used = FALSE;
			sems[QUEUE_get_arg(this)].token = TRUE;
			QUEUE_flush(this);
		}
	}	
}
