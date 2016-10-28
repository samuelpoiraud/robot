/*
 *  Club Robot ESEO 2006 - 2007
 *  Game Hoover, Phoboss, Archi'Tech, Chomp
 *
 *  Fichier : buffer.c
 *  Package : Asser
 *  Description : Fonctions de buffer de trajectoire
 *  Auteur : Val' && Thomas MOESSE alias Ketchom && Arnaud AUMONT alias Koubi
 *  MAJ 2009 : reprogrammation du buffer 200904 par Nirgal
 *  Version 200904
 */

#define BUFFER_C

#include "buffer.h"
#include "roadmap.h"
//Le buffer de trajectoires repose sur deux index + le nombre d'�l�ments.
//  1 index d'�criture
//  1 index de lecture
// Activer le buffer signifie enregistrer � la suite les trajectoires
// D�sactiver le buffer signifie �craser la trajectoire m�moriser si une autre est �crite (une seule case...)

// ATTENTION :
// toutes les op�rations sur le buffer doivent se faire dans la t�che de fond, afin d'�viter toute collision/probl�me de ressource commune.
// en effet, si l'IT vient par exemple appeler une fonction du buffer en m�me temps que le main, on peut aboutir � des situations �tranges...



//Buffer de trajectoires
volatile order_t buffer[BUFFER_SIZE];

//Nombre de trajectoires dans le buffer
volatile Uint8 buffer_nb;

//Position de la prochaine trajectoire � lire si Buffer_nb>0
volatile Uint8 buffer_read;

//Position de la prochaine trajectoire � �crire
volatile Uint8 buffer_write;

//Buffer activ� ou non...
volatile bool_e buffer_enable;


void BUFFER_init()
{
	buffer_nb = 0;
	buffer_read = 0;
	buffer_write = 0;
	BUFFER_enable(TRUE);
}


void BUFFER_enable(bool_e state)
{	// TRUE : Active le buffer au nombre max de cases
	// FALSE : D�sactive le buffer : une seule trajectoire possible est m�moris�e !
	// Dans ce mode, toute trajectoire ajout�e �crasera les trajectoires en m�moire !!!

	//ATTENTION : ce mode peut g�n�rer des erreurs,  puisque le robot lanc�, on stoppe tout ordre pour
	//envoyer un nouveau -> il peut partir en erreur... La carte P est responsable de ses actes !
	buffer_enable = state;
}


void BUFFER_add(order_t * order)
{

	if (buffer_enable == FALSE)
	{	//Si le buffer est d�sactiv�, on le vide pour ne remplir que la premiere case !!!
		BUFFER_init();
	}
	if (BUFFER_is_full() == FALSE)
	{
		// on ajoute une trajectoire dans le buffer
		buffer[buffer_write] = *order;
		buffer_nb++;

		//M�j Buffer_write
		buffer_write = (buffer_write + 1) % BUFFER_SIZE;

	}
}

void BUFFER_add_begin(order_t * order)
{


	if (BUFFER_is_full() == FALSE)
	{
		//M�j Buffer_read
		//Si le buffer n'est pas d�sactiv� on ajoute la trajectoire � Buffer_read-1
		//sinon on ajoute la valeur dans la premi�re case du buffer
		//et on incr�mente Buffer_write dans le cas o� on repasse en mode Buffer_ON ==TRUE pour pas �crire par dessus une trajectoire

		// retour � la derni�re position du buffer
		if(buffer_read == 0)
			buffer_read = BUFFER_SIZE-1;
		else
			buffer_read--;


		// on ajoute une trajectoire dans le buffer
		buffer[buffer_read] = *order;
		buffer_nb++;
	}
}

bool_e BUFFER_is_full(void)
{
	// TRUE : buffer plein, FALSE : buffer non plein
	if (buffer_nb == BUFFER_SIZE)
		return TRUE;
	else
		return FALSE;
}

bool_e BUFFER_is_empty(void)
{
	// TRUE : buffer vide, FALSE : buffer non vide
	if (buffer_nb == 0)
		return TRUE;
	else
		return FALSE;
}

void BUFFER_get_next(order_t * order)
{
	//On lit une trajectoire dans le buffer
	*order = (buffer[buffer_read]);
	buffer_nb--;

	//M�j Buffer_read
	buffer_read = (buffer_read + 1) % BUFFER_SIZE;
}

void BUFFER_check_next(order_t * order){
	if(BUFFER_is_empty() == FALSE)
		*order = (buffer[buffer_read]);
	else
		*order = (order_t){TRAJECTORY_NONE, 0, 0, 0, PROP_ABSOLUTE, PROP_END_AT_POINT, PROP_NO_BORDER_MODE, ANY_WAY, FAST, NO_ACKNOWLEDGE, CORRECTOR_ENABLE, AVOID_DISABLED, 0, 0, 0};
}
