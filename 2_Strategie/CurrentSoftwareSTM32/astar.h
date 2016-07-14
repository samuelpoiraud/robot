/*
 *	Club Robot ESEO 2015 - 2017
 *
 *
 *	Fichier : astar.c
 *	Package : Stratégie
 *	Description : 	Fonctions de génération des trajectoires
 *					par le biais d'un algo de type A*
 *	Auteurs : Valentin BESNARD
 *	Version 2
 */

	#include "config/config_use.h"


	#ifdef USE_ASTAR

	#ifndef _ASTAR_H_
		#define _ASTAR_H_

	#include "QS/QS_who_am_i.h"
	#include "avoidance.h"



//--------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------- Fonctions importantes de l'algo A* ------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

void ASTAR_init();
Uint8 ASTAR_try_going(Uint16 x, Uint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, PROP_speed_e speed, way_e way, avoidance_type_e avoidance, PROP_end_condition_e end_condition);

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------- Fonctions d'affichage ------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

void ASTAR_print_opened_list();
void ASTAR_print_closed_list();
void ASTAR_print_nodes(bool_e with_neighbors);
void ASTAR_print_obstacles();

//--------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------- Accesseurs -----------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------

void ASTAR_enable_polygon(Uint8 polygon_number);
void ASTAR_disable_polygon(Uint8 polygon_number);




	#endif /* ndef _ASTAR_H_ */
#endif /* def USE_ASTAR */




