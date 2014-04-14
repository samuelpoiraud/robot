/*
 *  Club Robot ESEO 2006 - 2007
 *  Game Hoover
 *
 *  Fichier :  gestion_traj.c
 *  Package : Asser
 *  Description : Fonctions qui gèrent les trajectoires
 *  Auteur : Inspiré par Val' 207 - Nirgal 2009
 *  Version 200904
 */
 
#define _ROADMAP_C

#include "roadmap.h"
#include "buffer.h"

volatile bool_e new_prioritary_order = FALSE;

void ROADMAP_init(void)
{
	BUFFER_init();
}
//permet d'ajouter une trajectoire au buffer.
void ROADMAP_add_order(	trajectory_e trajectory,
						Sint16 x,
						Sint16 y,
						Sint16 teta,
						relative_e relative,
						now_e now,
						way_e way,
						border_mode_e border_mode,
						multipoint_e multipoint,
						ASSER_speed_e speed,
						acknowledge_e acknowledge,
						corrector_e corrector)
{
	order_t order;

	order.trajectory = trajectory;
	order.relative = relative;
	order.border_mode = border_mode;
	order.way = way;
	order.multipoint = multipoint;
	order.x = x;
	order.y = y;
	order.teta = teta;
	order.speed = speed;
	order.acknowledge = acknowledge;
	order.corrector = corrector;

	if(now == NOW)
	{
		BUFFER_enable(FALSE);	//Ordre à prendre en compte dès maintenant !
		new_prioritary_order = TRUE;	//L'ordre ajouté est prioritaire ! (le copilot ira donc le chercher dès que possible)
	}
	else
	{
		BUFFER_enable(TRUE);	//Ordre a ajouter au buffer
	}

	BUFFER_add (&order);
}

bool_e ROADMAP_exists_prioritary_order(void)
{
	return new_prioritary_order;
}


bool_e ROADMAP_get_next(order_t * order)
{
	if(BUFFER_is_empty())
		return FALSE;
	BUFFER_get_next(order);
	new_prioritary_order = FALSE;	//S'il y avait un ordre prioritaire, on vient de le livrer. Sinon, ca change rien !
	return TRUE;
}


void ROADMAP_add_order_begin(order_t * order)
{
	BUFFER_add(order);
}
