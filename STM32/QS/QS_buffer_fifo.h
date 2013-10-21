/*
 *  Club Robot ESEO 2013 - 2014
 *
 *  $Id$
 *
 *  Package : Qualité Soft
 *  Description : Gestion d'un buffer circulaire en mode FIFO
 *  Auteur : amurzeau
 */


#ifndef QS_BUFFER_FIFO_H_
#define QS_BUFFER_FIFO_H_

//Mode de fonctionnement:
//Read before, increment after
//Write before, increment after

#include "QS_all.h"

#ifdef USE_FIFO

#ifdef FIFO_USE_32BITS
#define fifo_size_t Uint32
#else
#define fifo_size_t Uint16
#endif

typedef struct {
	char *buffer;
	fifo_size_t element_size;	//Taille en octet du buffer (buffer_size) = element_count * element_size
	fifo_size_t element_count;
	volatile fifo_size_t read_idx;
	volatile fifo_size_t write_idx;
} FIFO_t;

//ATTENTION !!!!! LES FONCTIONS NE SONT PAS REENTRANTES !!!!!
//Il faut désactiver les IT qui peuvent utiliser la fifo en même temps qu'une autre IT ou le main

void FIFO_init(FIFO_t *fifo,
		void* buffer,
		fifo_size_t element_count,
		fifo_size_t element_size);

bool_e      FIFO_isEmpty          (FIFO_t *fifo);
bool_e      FIFO_isFull           (FIFO_t *fifo);
fifo_size_t FIFO_availableElements(FIFO_t *fifo);
bool_e      FIFO_insertData       (FIFO_t *fifo, const void* data);
void*       FIFO_getData          (FIFO_t *fifo);

#endif /* USE_FIFO */

#endif
