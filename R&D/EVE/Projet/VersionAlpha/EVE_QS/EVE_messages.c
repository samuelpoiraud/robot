/*
*	PFE Simulateur QSx86 2011-2012
*	Logiciel EVE
*
*	Fichier : EVE_messages.c
*	Package : EVE_QS
*	Description : Messages utilisés par le logiciel
*	Auteur : Julien Franchineau & François Even
*	Version 20111208
*/

#define EVE_MESSAGES_C

#include "EVE_messages.h"
#include <errno.h>

/**************************** Fonctions relatives aux BAL de communication ****************************/

// Fonction de création de la BAL
Uint32 EVE_create_msg_queue(Uint32 key_queue)
{
	Uint32 result;

	result = msgget(key_queue,IPC_CREAT | 0666);
	if(result == -1)
	{
		EVE_MESSAGES_errors(ERROR_CREATE_QUEUE,"Impossible to create");
		return result;
	}
	return result;
}

// Fonction de destruction de la BAL
Uint16 EVE_destroy_msg_queue(Uint32 queue_id)
{
	if(msgctl(queue_id,IPC_RMID,NULL) != 0)
	{
		return EVE_MESSAGES_errors(ERROR_DESTROY_QUEUE,"Impossible to destroy");
	}
	return END_SUCCESS;
}

// Fonction qui contrôle le nombre de messages disponibles dans la BAL
Uint16 EVE_check_messages_numbers(Uint32 queue_id)
{
	struct msqid_ds message_stats;
	Uint16 error;

	error = msgctl(queue_id,IPC_STAT,&message_stats);
	if(error != 0)
	{
		EVE_MESSAGES_errors(ERROR_CHECK_QUEUE,"Error read queue");
		return -1;
	}

	// On renvoie le nombre de messages lus
	return message_stats.msg_qnum;
}

/********************************* Ecriture/Lecture données QS ********************************************/
// Fonction de lecture de la BAL spécifique aux données QS
Uint16 EVE_read_new_msg_QS(Uint32 queue_id, EVE_QS_data_msg_t* data)
{
	Uint32 error;

	error = msgrcv(queue_id,(struct msgbuf*) *&data, EVE_QS_DATA_MSG_T_SIZE, 0,IPC_NOWAIT);
	if(error == -1)
	{
		return EVE_MESSAGES_errors(ERROR_RECEIVE_QUEUE,"QS data not receive");
	}
	else if(error == 0)
	{
		return EVE_MESSAGES_errors(ERROR_NO_MESSAGE_QUEUE,"No QS data in queue");
	}

	return END_SUCCESS;
}

// Fonction d'écriture de la BAL spécifique aux données QS
Uint16 EVE_write_new_msg_QS(Uint32 queue_id, EVE_QS_data_msg_t data)
{
	data.mtype = QUEUE_QS_DATA_MTYPE;
	if(msgsnd(queue_id,(struct msgbuf*) &data, EVE_QS_DATA_MSG_T_SIZE, IPC_NOWAIT) == -1)
	{
		//Si c'est EAGAIN, le buffer d'envoi est plein => l'interface ne lis pas les messages qu'on envoie, donc c'est normal qu'on ai EAGAIN
		if(errno != EAGAIN)
			return EVE_MESSAGES_errors(ERROR_SEND_QUEUE,"QS data not sent");
	}
	return END_SUCCESS;
}

/********************************* Ecriture/Lecture données UART ******************************************/
// Fonction de lecture de la BAL spécifique aux données UART
Uint16 EVE_read_new_msg_UART(Uint32 queue_id, EVE_UART_msg_t* data)
{
	Uint32 error;

	error = msgrcv(queue_id,(struct msgbuf*) *&data, EVE_UART_MSG_T_SIZE, 0,IPC_NOWAIT);
	if(error == -1)
	{
		return EVE_MESSAGES_errors(ERROR_RECEIVE_QUEUE,"UART msg not receive");
	}
	else if(error == 0)
	{
		return EVE_MESSAGES_errors(ERROR_NO_MESSAGE_QUEUE,"No UART msg in queue");
	}

	return END_SUCCESS;
}

// Fonction d'écriture de la BAL spécifique aux données UART
Uint16 EVE_write_new_msg_UART(Uint32 queue_id, EVE_UART_msg_t data)
{
	data.mtype = QUEUE_UART_MTYPE;
	if(msgsnd(queue_id,(struct msgbuf*) &data, EVE_UART_MSG_T_SIZE, IPC_NOWAIT) == -1)
	{
		return EVE_MESSAGES_errors(ERROR_SEND_QUEUE,"UART msg not sent");
	}
	return END_SUCCESS;
}

/********************************* Ecriture/Lecture données CAN *******************************************/
// Fonction de lecture de la BAL spécifique aux données CAN
Uint16 EVE_read_new_msg_CAN(Uint32 queue_id, EVE_CAN_msg_t* data)
{
	Uint32 error;

	error = msgrcv(queue_id,(struct msgbuf*) *&data, EVE_CAN_MSG_T_SIZE, 0,IPC_NOWAIT);
	if(error == -1)
	{
		return EVE_MESSAGES_errors(ERROR_RECEIVE_QUEUE,"CAN msg not receive");
	}
	else if(error == 0)
	{
		return EVE_MESSAGES_errors(ERROR_NO_MESSAGE_QUEUE,"No CAN msg in queue");
	}

	return END_SUCCESS;
}

// Fonction d'écriture de la BAL spécifique aux données CAN
Uint16 EVE_write_new_msg_CAN(Uint32 queue_id, EVE_CAN_msg_t data)
{
	data.mtype = QUEUE_CAN_MTYPE;
	if(msgsnd(queue_id,(struct msgbuf*) &data, EVE_CAN_MSG_T_SIZE, IPC_NOWAIT) == -1)
	{
		return EVE_MESSAGES_errors(ERROR_SEND_QUEUE,"CAN msg not sent");
	}
	return END_SUCCESS;
}


/*********************** Fonctions relatives à la gestion de la mémoire partagée **********************/

// Fonction de création  et de liaison à la mémoire partagée
Uint16 EVE_create_and_bound_global_clock_memory(char** memory_reference)
{
	Uint32 memory_id;

	// 0666 indique qu'on donne les droits de lecture et écriture à tout le monde
	// Identifiant MEMORY_GLOBAL_CLOCK est unique et commun à tout le monde
	memory_id = shmget(MEMORY_GLOBAL_CLOCK, sizeof(EVE_clock_memory_t), 0666 | IPC_CREAT);
	if(memory_id == -1)
	{
		return EVE_MESSAGES_errors(ERROR_SHMGET,"Impossible to create segment");
	}

	*memory_reference = shmat (memory_id,NULL,0);
	// on vérifie que le pointeur n'est pas null et qu'il n'y a pas de retour d'erreur (-1)
	if((memory_reference == NULL))// || (memory_reference == -1))
	{
		return EVE_MESSAGES_errors(ERROR_SHMAT,"Echec");
	}

	return EVE_MESSAGES_errors(END_SUCCESS,"Creation and bound memory");
}

// Fonction permettant de se détacher d'un segment mémoire 
Uint16 EVE_unbound_global_clock_memory(char* memory_reference)
{
	if(shmdt(memory_reference)!=0)
	{
		return EVE_MESSAGES_errors(ERROR_SHMDT,"Unbound fail");
	}

	return EVE_MESSAGES_errors(END_SUCCESS,"Unbound success");
}

// Fonction de lecture de la mémoire partagée de l'horloge
EVE_time_t EVE_get_global_clock(char* memory_reference)
{
	return ((EVE_clock_memory_t*) memory_reference)->global_time_memory;
}

/********************************** GESTION DES ERREURS **********************************/
// Gestion des retours d'erreurs
static Uint16 EVE_MESSAGES_errors(Uint16 error_id, char* commentary)
{
	switch(error_id)
	{
		case END_SUCCESS:
			printf("END_SUCCESS : %s\n",commentary);
			break;

		case ERROR_SHMGET:
			printf("ERROR_SHMGET : %s, errno = %d\n",commentary, errno);
			break;

		case ERROR_SHMAT:
			printf("ERROR_SHMAT : %s, errno = %d\n",commentary, errno);
			perror("shmat");
			break;

		case ERROR_SHMDT:
			printf("ERROR_SHMDT : %s, errno = %d\n",commentary, errno);
			break;

		case ERROR_CREATE_QUEUE:
			printf("ERROR_CREATE_QUEUE : %s, errno = %d\n",commentary, errno);
			break;

		case ERROR_DESTROY_QUEUE:
			printf("ERROR_DESTROY_QUEUE : %s, errno = %d\n",commentary, errno);
			break;

		case ERROR_SEND_QUEUE:
			printf("ERROR_SEND_QUEUE : %s, errno = %d\n",commentary, errno);
			break;

		case ERROR_RECEIVE_QUEUE:
			printf("ERROR_RECEIVE_QUEUE : %s, errno = %d\n",commentary, errno);
			break;

		case ERROR_NO_MESSAGE_QUEUE:
			printf("ERROR_NO_MESSAGE_QUEUE : %s, errno = %d\n",commentary, errno);
			break;

		case ERROR_CHECK_QUEUE:
			printf("ERROR_CHECK_QUEUE : %s, errno = %d\n",commentary, errno);
			break;

		default:
			printf("error_id inconnu : %d, %s, errno = %d\n",error_id,commentary, errno);
			break;
	}

	return error_id;
}

