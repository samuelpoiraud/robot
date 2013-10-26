/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Qualité Soft
 *  Description : Gestion d'un buffer circulaire en mode FIFO
 *  Auteur : amurzeau
 */

#include "QS_buffer_fifo.h"

#ifdef USE_FIFO

static void FIFO_incReadIndex(FIFO_t *fifo);
static void FIFO_incWriteIndex(FIFO_t *fifo);
static void* FIFO_getWritePtr(FIFO_t *fifo);
static void* FIFO_getReadPtr(FIFO_t *fifo);

//Ne doit pas être preempté par une IT qui utilise aussi la même fifo
void FIFO_init(FIFO_t *fifo, void* buffer, fifo_size_t element_count, fifo_size_t element_size) {
	fifo->buffer = (char*)buffer;
	fifo->element_size = element_size;
	fifo->element_count = element_count;
	fifo->read_idx = 0;
	fifo->write_idx = 0;
}

//Ne doit pas être préempté par une IT qui écrit ou lit dans la FIFO (sinon le nombre d'élement sera invalide)
fifo_size_t FIFO_availableElements(FIFO_t *fifo) {
	fifo_size_t read_idx;
	fifo_size_t write_idx;

	read_idx = fifo->read_idx;
	write_idx = fifo->write_idx;

	if(write_idx < read_idx)
		return fifo->element_count + write_idx - read_idx;
	else
		return write_idx - read_idx;
}

//Ne doit pas être préempté par une IT qui utilise FIFO_getData ou FIFO_incReadIndex
bool_e FIFO_isEmpty(FIFO_t *fifo) {
	return (fifo->read_idx == fifo->write_idx);
}

//Ne doit pas être préempté par une IT qui utilise FIFO_insertData ou FIFO_incWriteIndex
bool_e FIFO_isFull(FIFO_t *fifo) {
	fifo_size_t next_write_idx;

	next_write_idx = fifo->write_idx + 1;
	if(next_write_idx >= fifo->element_count)
		next_write_idx = 0;
	return (fifo->read_idx == next_write_idx);
}

//Ne doit pas être préempté par une IT qui utilise FIFO_insertData ou FIFO_incWriteIndex
bool_e FIFO_insertData(FIFO_t *fifo, const void* data) {
	fifo_size_t i;
	char* element_ptr;

	if(FIFO_isFull(fifo))
		return FALSE;

	//Récupère l'emplacement de la donnée dans le buffer
	element_ptr = (char*)FIFO_getWritePtr(fifo);

	//Copie des données à l'emplacement
	for(i = 0; i < fifo->element_size; i++) {
		element_ptr[i] = ((char*)data)[i];
	}

	FIFO_incWriteIndex(fifo);

	//Si on est empty après avoir ajouté une donnée, il y a un overflow ...
	//On a déjà test si on était plein avant => problème de préemption
	assert(!FIFO_isEmpty(fifo));

	return TRUE;
}

//Ne doit pas être préempté par une IT qui utilise FIFO_getData ou FIFO_incReadIndex
void* FIFO_getData(FIFO_t *fifo) {
	void* dataToReturn;

	if(FIFO_isEmpty(fifo))
		return 0;

	dataToReturn = FIFO_getReadPtr(fifo);
	FIFO_incReadIndex(fifo);

	return dataToReturn;
}

//Ne doit pas être préempté par une IT qui utilise FIFO_getData ou FIFO_incReadIndex
static void FIFO_incReadIndex(FIFO_t *fifo) {
	fifo_size_t current_idx = fifo->read_idx;

	//L'utilisation d'une variable locale permet de ne jamais avoir un index hors de tableau (si preemption)

	current_idx++;
	if(current_idx >= fifo->element_count)
		fifo->read_idx = 0;
	else fifo->read_idx = current_idx;
}

//Ne doit pas être préempté par une IT qui utilise FIFO_insertData ou FIFO_incWriteIndex
static void FIFO_incWriteIndex(FIFO_t *fifo) {
	fifo_size_t current_idx = fifo->write_idx;

	//L'utilisation d'une variable locale permet de ne jamais avoir un index hors de tableau (si preemption)

	current_idx++;
	if(current_idx >= fifo->element_count)
		fifo->write_idx = 0;
	else fifo->write_idx = current_idx;
}

static void* FIFO_getWritePtr(FIFO_t *fifo) {
	return &(fifo->buffer[fifo->write_idx*fifo->element_size]);
}

static void* FIFO_getReadPtr(FIFO_t *fifo) {
	return &(fifo->buffer[fifo->read_idx*fifo->element_size]);
}

#endif /* USE_FIFO */
