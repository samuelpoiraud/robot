/*
 *  Club Robot ESEO 2006 - 2007
 *  Game Hoover
 *
 *  Fichier :  gestion_traj.c
 *  Package : Asser
 *  Description : Fonctions qui g�rent les trajectoires
 *  Auteur : Inspir� par Val' 207 - Nirgal 2009
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
						prop_referential_e relative,
						prop_buffer_mode_e now,
						way_e way,
						border_mode_e border_mode,
						prop_multipoint_e multipoint,
						PROP_speed_e speed,
						acknowledge_e acknowledge,
						corrector_e corrector,
						avoidance_e avoidance)
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
	order.avoidance = avoidance;
	order.total_wait_time = 0;

	if(now == PROP_NOW)
	{
		BUFFER_enable(FALSE);	//Ordre � prendre en compte d�s maintenant !
		new_prioritary_order = TRUE;	//L'ordre ajout� est prioritaire ! (le copilot ira donc le chercher d�s que possible)
	}
	else
	{
		BUFFER_enable(TRUE);	//Ordre a ajouter au buffer
	}

	BUFFER_add (&order);
}

void ROADMAP_add_simple_order(order_t order, bool_e add_at_begin, bool_e clean_buffer, bool_e buffer_mode){

	if(clean_buffer){
		BUFFER_init();
	}

	BUFFER_enable(buffer_mode);

	if(add_at_begin){
		BUFFER_add_begin(&order);
	}else{
		BUFFER_add(&order);
	}


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

void ROADMAP_launch_next_order(){
	new_prioritary_order = TRUE;
}
