/*
 *	Club Robot ESEO 2010 - 2011
 *	CkeckNorris
 *
 *	Fichier : elements.c
 *	Package : Carte Principale
 *	Description : Traitement des informations sur les éléments de jeu
 *	Auteur : Ronan, Adrien
 *	Version 20110430
 */
 #include "elements.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_CANmsgList.h"

bool_e prop_send_all_triangle = FALSE;

struct{Sint16 x; Sint16 y; Sint16 teta;} objet[3][20];
Uint8 nb_objet[3];

Uint8 try_going_and_rotate_scan(Sint16 startTeta, Sint16 endTeta, Uint8 nb_points, Sint16 x, Sint16 y, Uint8 in_progress, Uint8 success_state, Uint8 fail_state, ASSER_speed_e speed, way_e way, avoidance_type_e avoidance){
	typedef enum
	{
		TRY_GOING,
		BEGIN_SCAN,
		SCAN,
		SCAN_END,
		ERROR
	}state_e;
	static state_e state = TRY_GOING;
	CAN_msg_t msg;
	switch(state){
		case TRY_GOING:
			state = try_going(x, y, TRY_GOING, BEGIN_SCAN, ERROR, speed, way, avoidance);
			break;

		case BEGIN_SCAN:
			msg.sid = ASSER_LAUNCH_SCAN_TRIANGLE;
			msg.data[0] = 0x00;
			msg.data[1] = nb_points;
			msg.data[2] = (Uint8)((startTeta >> 8) & 0x00FF);
			msg.data[3] = (Uint8)(startTeta & 0x00FF);
			msg.data[4] = (Uint8)((endTeta >> 8) & 0x00FF);
			msg.data[5] = (Uint8)(endTeta & 0x00FF);
			msg.size = 6;
			CAN_send(&msg);
			TRIANGLE_init_list();
			state = SCAN;
			break;

		case SCAN:
			if(propulsion_send_triangle()){
				afficher_donnee_triangle();
				state = SCAN_END;
			}
			break;

		case SCAN_END:
			state = TRY_GOING;
			return success_state;

		case ERROR:
			state = TRY_GOING;
			return fail_state;
	}
	return in_progress;
}

void LAUNCH_SCAN_TRIANGLE(){
	CAN_msg_t msg;
	msg.sid = ASSER_LAUNCH_SCAN_TRIANGLE;
	msg.size = 0;
	CAN_send(&msg);
	TRIANGLE_init_list();
}

void TRIANGLE_init_list(){
	Uint8 i, j;
	for(j=0;j<3;j++){
		nb_objet[j] = 0;
		for(i=0;i<20;i++){
			objet[j][i].teta = 0;
			objet[j][i].y = 0;
			objet[j][i].x = 0;
		}
	}
}

void TRIANGLE_add_to_list(CAN_msg_t* msg){
	Uint8 level, number;
	level = (msg->data[0] & 0x60) >> 5;
	number = (msg->data[0] & 0x1F);
	if(level < 3 && number < 20){
		objet[level][number].x = (((Sint16)(msg->data[1]) << 8) & 0xFF00) | ((Sint16)(msg->data[2]) & 0x00FF);
		objet[level][number].y = (((Sint16)(msg->data[3]) << 8) & 0xFF00) | ((Sint16)(msg->data[4]) & 0x00FF);
		objet[level][number].teta = (((Sint16)(msg->data[5]) << 8) & 0xFF00) | ((Sint16)(msg->data[6]) & 0x00FF);
		nb_objet[level] = number+1;
	}
	if(msg->data[0] & 0x80){
		prop_send_all_triangle = TRUE;
	}
}

bool_e propulsion_send_triangle(){
	if(prop_send_all_triangle){
		prop_send_all_triangle = FALSE;
		return TRUE;
	}else
		return FALSE;
}

void afficher_donnee_triangle(){
	Uint8 i, j;
	for(i=0;i<3;i++){
		for(j=0;j<nb_objet[i];j++){
			debug_printf("%d %d  x:%d  y:%d  teta:%d\n", i, j, objet[i][j].x, objet[i][j].y, objet[i][j].teta);
		}
	}
}


#if 0
 #define ELEMENTS_C

 #include "elements.h"
 #include "config_debug.h"


 /* Pour ce programme de gestion des elements cf. ELEMENTS.png
 dans le dossier "Ressources" du projet de la carte strategie */

/* Tableau des cases du terrain (partie constante) */
area_mapping_t const area_mapping_map[NB_AREAS] =
{
//	{SCORING_AREA,			coords : {175,625},		TOP_AREA},			    //1
//	{SCORING_AREA,			coords : {175,975},		TOP_AREA},				//2
//	{SCORING_AREA,			coords : {175,1325},	TOP_AREA},			    //3
//	{SCORING_AREA,			coords : {175,1675},	TOP_AREA},				//4
//	{SCORING_AREA,			coords : {175,2025},	TOP_AREA},			    //5
//	{SCORING_AREA,			coords : {175,2375},	TOP_AREA},		        //6
//	{SCORING_AREA,			coords : {525,625},		LEFT_AREA},				//7
//	{BONUS_SCORING_AREA,	coords : {525,975},		NORMAL_AREA},			//8
//	{SCORING_AREA,			coords : {525,1325},	NORMAL_AREA},			//9
//	{SCORING_AREA,			coords : {525,1675},	NORMAL_AREA},		    //10
//	{BONUS_SCORING_AREA,	coords : {525,2025},	NORMAL_AREA},		    //11
//	{SCORING_AREA,			coords : {525,2375},	RIGHT_AREA},		  	//12
//	{SCORING_AREA,			coords : {875,625},		LEFT_AREA},	    		//13
//	{SCORING_AREA,			coords : {875,975},		NORMAL_AREA},			//14
//	{SCORING_AREA,			coords : {875,1325},	NORMAL_AREA},			//15
//	{SCORING_AREA,			coords : {875,1675},	NORMAL_AREA},			//16
//	{SCORING_AREA,			coords : {875,2025},	NORMAL_AREA},		  	//17
//	{SCORING_AREA,			coords : {875,2375},	RIGHT_AREA},			//18
//	{SCORING_AREA,			coords : {1225,625},	LEFT_AREA},				//19
//	{BONUS_SCORING_AREA,	coords : {1225,975},	NORMAL_AREA},			//20
//	{SCORING_AREA,			coords : {1225,1325},	NORMAL_AREA},			//21
//	{SCORING_AREA,			coords : {1225,1675},	NORMAL_AREA},	    	//22
//	{BONUS_SCORING_AREA,	coords : {1225,2025},	NORMAL_AREA},	    	//23
//	{SCORING_AREA,			coords : {1225,2375},	RIGHT_AREA},		  	//24
//	{SCORING_AREA,			coords : {1575,625},	LEFT_AREA}, 			//25
//	{SCORING_AREA,			coords : {1575,975},	NORMAL_AREA},			//26
//	{SCORING_AREA,			coords : {1575,1325},	NORMAL_AREA},	    	//27
//	{SCORING_AREA,			coords : {1575,1675},	NORMAL_AREA},			//28
//	{SCORING_AREA,			coords : {1575,2025},	NORMAL_AREA},		    //29
//	{SCORING_AREA,			coords : {1575,2375},	RIGHT_AREA},			//30
//	{SECURE_SCORING_AREA,	coords : {1865,625},	BOTTOM_AREA},		    //31
//	{SECURE_SCORING_AREA,	coords : {1865,975},	BOTTOM_AREA},			//32
//	{BONUS_SCORING_AREA,	coords : {1925,1325},	BOTTOM_AREA},			//33
//	{BONUS_SCORING_AREA,	coords : {1925,1675},	BOTTOM_AREA},	    	//34
//	{SECURE_SCORING_AREA,	coords : {1865,2025},	BOTTOM_AREA},			//35
//	{SECURE_SCORING_AREA,	coords : {1865,2375},	BOTTOM_AREA},		    //36
//
//	{INTER_AREA,	coords : {350,450},		TOP_LEFT_AREA},			//37
//	{INTER_AREA,	coords : {350,800},		TOP_AREA},				//38
//	{INTER_AREA,	coords : {350,1150},	TOP_AREA},				//39
//	{INTER_AREA,	coords : {350,1500},	TOP_AREA},				//40
//	{INTER_AREA,	coords : {350,1850},	TOP_AREA},				//41
//	{INTER_AREA,	coords : {350,2200},	TOP_AREA},				//42
//	{INTER_AREA,	coords : {350,2550},	TOP_RIGHT_AREA},		//43
//	{INTER_AREA,	coords : {700,450},		LEFT_AREA},				//44
//	{INTER_AREA,	coords : {700,800},		NORMAL_AREA},			//45
//	{INTER_AREA,	coords : {700,1150},	NORMAL_AREA},			//46
//	{INTER_AREA,	coords : {700,1500},	NORMAL_AREA},			//47
//	{INTER_AREA,	coords : {700,1850},	NORMAL_AREA},			//48
//	{INTER_AREA,	coords : {700,2200},	NORMAL_AREA},			//49
//	{INTER_AREA,	coords : {700,2550},	RIGHT_AREA},			//50
//	{INTER_AREA,	coords : {1050,450},	LEFT_AREA},				//51
//	{INTER_AREA,	coords : {1050,800},	NORMAL_AREA},			//52
//	{INTER_AREA,	coords : {1050,1150},	NORMAL_AREA},			//53
//	{INTER_AREA,	coords : {1050,1500},	NORMAL_AREA},			//54
//	{INTER_AREA,	coords : {1050,1850},	NORMAL_AREA},			//55
//	{INTER_AREA,	coords : {1050,2200},	NORMAL_AREA},			//56
//	{INTER_AREA,	coords : {1050,2550},	RIGHT_AREA},			//57
//	{INTER_AREA,	coords : {1400,450},	LEFT_AREA},				//58
//	{INTER_AREA,	coords : {1400,800},	NORMAL_AREA},			//59
//	{INTER_AREA,	coords : {1400,1150},	NORMAL_AREA},			//60
//	{INTER_AREA,	coords : {1400,1500},	NORMAL_AREA},			//61
//	{INTER_AREA,	coords : {1400,1850},	NORMAL_AREA},			//62
//	{INTER_AREA,	coords : {1400,2200},	NORMAL_AREA},			//63
//	{INTER_AREA,	coords : {1400,2550},	RIGHT_AREA},			//64
//	{INTER_AREA,	coords : {1750,450},	BOTTOM_LEFT_AREA},		//65
//	{INTER_AREA,	coords : {1750,800},	BOTTOM_AREA},			//66
//	{INTER_AREA,	coords : {1750,1150},	BOTTOM_AREA},			//67
//	{INTER_AREA,	coords : {1750,1500},	BOTTOM_AREA},			//68
//	{INTER_AREA,	coords : {1750,1850},	BOTTOM_AREA},			//69
//	{INTER_AREA,	coords : {1750,2200},	BOTTOM_AREA},			//70
//	{INTER_AREA,	coords : {1750,2550},	BOTTOM_RIGHT_AREA},		//71
//
//	{GREEN_AREA,	coords : {690,225},		TOP_AREA},				//72
//	{GREEN_AREA,	coords : {970,225},		NORMAL_AREA},			//73
//	{GREEN_AREA,	coords : {1250,225},	NORMAL_AREA},	  		//74
//	{GREEN_AREA,	coords : {1530,225},	NORMAL_AREA},    		//75
//	{GREEN_AREA,	coords : {1810,225},	BOTTOM_AREA},			//76
//	{GREEN_AREA,	coords : {690,2775},	TOP_AREA},				//77
//	{GREEN_AREA,	coords : {970,2775},	NORMAL_AREA},			//78
//	{GREEN_AREA,	coords : {1250,2775},	NORMAL_AREA},			//79
//	{GREEN_AREA,	coords : {1530,2775},	NORMAL_AREA},			//80
//	{GREEN_AREA,	coords : {1810,2775},	BOTTOM_AREA},			//81
//	{START_AREA,	coords : {200,200},		NORMAL_AREA},			//82
//	{START_AREA,	coords : {200,2800},	NORMAL_AREA}			//83
};
/* Tableau des cases du terrain (partie variable) */
static area_t area_map[NB_AREAS];

static bool_e update_locked;

belonging_e ELEMENTS_belonging_type(Uint8 element_number)
{
	assert(element_number<global.env.nb_elements);
	return area_map[global.env.elements_list[element_number].area_number].belonging;
}
area_type_e ELEMENTS_get_area_type(Uint8 area_number)
{
	return area_mapping_map[area_number].type;
}

belonging_e ELEMENTS_get_area_belonging(Uint8 area_number)
{
	return area_map[area_number].belonging;
}

GEOMETRY_point_t ELEMENTS_get_area_center(Uint8 area_number)
{
	return area_mapping_map[area_number].coords;
}

void ELEMENTS_set_file_already_tried(Uint8 area_number)
{
	area_map[area_number].file_already_tried = TRUE;
}

void ELEMENTS_lock_update()
{
	update_locked = TRUE;
}

void ELEMENTS_unlock_update()
{
	update_locked = FALSE;
}

void ELEMENTS_init()
{
	static bool_e initialized=FALSE;
	if(initialized)
	{
		return;
	}

//	global.env.nb_elements=0;
//
//	for(i=0;i<NB_AREAS;i++)
//	{
//		area_map[i].nb_elements=0;
//
//		if(i<=NB_SCORING_AREA-1)
//		{
//			/* Si le quotient de i divise par 6 est paire */
//			if(((i/6)%2)==0)
//			{
//				if((i%2)==0)
//					area_map[i].belonging=global.env.color==PURPLE?US:ADVERSARY;
//				else
//					area_map[i].belonging=global.env.color==PURPLE?ADVERSARY:US;
//			}
//			else
//			{
//				if(i%2)
//					area_map[i].belonging=global.env.color==PURPLE?US:ADVERSARY;
//				else
//					area_map[i].belonging=global.env.color==PURPLE?ADVERSARY:US;
//			}
//		}
//		else
//		{
//			area_map[i].belonging=NONE;
//		}
//	}
//
//	/* Ajout de l'element central */
//	ELEMENTS_add_or_update(1050,1500,PAWN,FALSE,TRUE);

	update_locked=FALSE;

	initialized=TRUE;
}

void ELEMENTS_update()
{
//	Uint8 i;
//	bool_e delete;
//	time32_t delta_time;
	if(!global.env.match_started)
	{
		return;
	}

/*	if(update_locked)
	{
		return;
	}

	for(i=0;i<global.env.nb_elements;i++)
	{
		delta_time=global.env.match_time-global.env.elements_list[i].element_add_time;
		if(area_mapping_map[global.env.elements_list[i].area_number].type!=SECURE_SCORING_AREA)
		{
			if(global.env.elements_list[i].type == PAWN)
			{
				switch(area_map[global.env.elements_list[i].area_number].belonging)
				{
					case US : delete = delta_time>=TIME_BEFORE_EXPIRY_OUR_PAWN;
						break;
					case ADVERSARY : delete = delta_time>=TIME_BEFORE_EXPIRY_ADVERSARY_PAWN;
						break;
					default : delete = delta_time>=TIME_BEFORE_EXPIRY_DEFAULT_PAWN;
						break;
				}
			}
			else
			{
				if(global.env.elements_list[i].type == TWO_PAWNS
					|| global.env.elements_list[i].type == QUEEN
					|| global.env.elements_list[i].type == KING
					|| global.env.elements_list[i].type == TOWER_ONE_PAWN_AND_QUEEN
					|| global.env.elements_list[i].type == TOWER_ONE_PAWN_AND_KING
					|| global.env.elements_list[i].type == TOWER_TWO_PAWN_AND_QUEEN
					|| global.env.elements_list[i].type == TOWER_TWO_PAWN_AND_KING)
				{
					switch(area_map[global.env.elements_list[i].area_number].belonging)
					{
						case US : delete = delta_time>=TIME_BEFORE_EXPIRY_OUR_TOWER;
							break;
						case ADVERSARY : delete = delta_time>=TIME_BEFORE_EXPIRY_ADVERSARY_TOWER;
							break;
						default : delete = delta_time>=TIME_BEFORE_EXPIRY_DEFAULT_TOWER;
							break;
					}
				}
				else
				{
					delete = delta_time>=TIME_BEFORE_EXPIRY_DEFAULT;
				}
			}
			if(delete)
			{
				ELEMENTS_delete_with_element_number(i);
				elements_printf("Deleting : %d\r\n",i);
			}
		}

		if(global.env.match_time - global.env.elements_list[i].try_time > TIME_BEFORE_RETRY_ACTION_ON_ELEMENT)
			global.env.elements_list[i].taken_already_tried = FALSE;
	}
*/
}

void ELEMENTS_delete_with_element_number(Uint8 element_number)
{
	Uint8 i;
	area_t* area_pt;

	assert(element_number<global.env.nb_elements);

	area_pt=&(area_map[global.env.elements_list[element_number].area_number]);

	#ifdef USE_ELEMENT_CAN_DEBUG
		CAN_send_element_updated(DELETE,element_number);
	#endif /* def USE_ELEMENT_CAN_DEBUG */

	for(i=0;i<area_pt->nb_elements;i++)
	{
		if(area_pt->elements_number[i] == element_number)
		{
			ELEMENTS_delete_from_areas(area_pt,i);
		}
	}
	ELEMENTS_delete_from_elements_list(element_number);
}

void ELEMENTS_delete_with_coordinates(Sint32 x, Sint32 y)
{
	Uint8 i;
	Sint32 sq_delta_x,sq_delta_y;
	element_t* element_pt;

	if(ELEMENTS_exist_in_elements_list(x,y))
	{
		for(i=0;i<global.env.nb_elements;i++)
		{
			element_pt=&(global.env.elements_list[i]);
			sq_delta_x=(x-element_pt->x)*(x-element_pt->x);
			sq_delta_y=(y-element_pt->y)*(y-element_pt->y);
			if(sq_delta_x+sq_delta_y<=ELEMENTS_SQUARED_RADIUS)
			{
				elements_printf("L'element %d va etre detruit\r\n",i);
				ELEMENTS_delete_with_element_number(i);
			}
		}
	}
}

void ELEMENTS_add_or_update(Sint32 x, Sint32 y, game_element_e type, bool_e build, bool_e update_coords)
{
	Uint8 i,area_number,older_element;
	Sint32 sq_delta_x,sq_delta_y;
	element_t* element_pt;
	area_t* area_pt;
	time32_t max_delta_time=0,delta_time=0;
	bool_e expiry = FALSE;

	if(!global.env.asser.calibrated)
	{
		return;
	}

	if(!ENV_game_zone_filter(x,y,ELEMENTS_RADIUS))
	{
		elements_printf("L'element ajoute est en dehors du terrain !\r\n");
		return;
	}

	/* Si on a plus de place dans le tableau d'elements, on supprime le plus vieux */
/*	if(global.env.nb_elements>=NB_ELEMENTS)
	{
		older_element=0;
		for(i=0;i<global.env.nb_elements&&!expiry;i++)
		{
			delta_time=global.env.match_time-global.env.elements_list[i].element_add_time;

			if(global.env.elements_list[i].type == PAWN)
			{
				switch(area_map[global.env.elements_list[i].area_number].belonging)
				{
					case US : expiry = delta_time>=TIME_BEFORE_EXPIRY_OUR_PAWN;
						break;
					case ADVERSARY : expiry = delta_time>=TIME_BEFORE_EXPIRY_ADVERSARY_PAWN;
						break;
					default : expiry = delta_time>=TIME_BEFORE_EXPIRY_DEFAULT_PAWN;
						break;
				}
			}
			else
			{
				if(global.env.elements_list[i].type == TWO_PAWNS
					|| global.env.elements_list[i].type == QUEEN
					|| global.env.elements_list[i].type == KING
					|| global.env.elements_list[i].type == TOWER_ONE_PAWN_AND_QUEEN
					|| global.env.elements_list[i].type == TOWER_ONE_PAWN_AND_KING
					|| global.env.elements_list[i].type == TOWER_TWO_PAWN_AND_QUEEN
					|| global.env.elements_list[i].type == TOWER_TWO_PAWN_AND_KING)
				{
					switch(area_map[global.env.elements_list[i].area_number].belonging)
					{
						case US : expiry = delta_time>=TIME_BEFORE_EXPIRY_OUR_TOWER;
							break;
						case ADVERSARY : expiry = delta_time>=TIME_BEFORE_EXPIRY_ADVERSARY_TOWER;
							break;
						default : expiry = delta_time>=TIME_BEFORE_EXPIRY_DEFAULT_TOWER;
							break;
					}
				}
				else
				{
					expiry = delta_time>=TIME_BEFORE_EXPIRY_DEFAULT;
				}
			}
			if(delta_time>max_delta_time || expiry)
			{
				max_delta_time=delta_time;
				older_element=i;
			}
		}

		ELEMENTS_delete_with_element_number(older_element);
		elements_printf("Deleting in add_or_update : %d (full)\r\n",older_element);
	}
*/
	/* On recupere le numero de zone */
	area_number=ELEMENTS_conversion(x,y);
	assert(area_number<NB_AREAS);

	for(i=0;i<global.env.nb_elements;i++)
	{
		element_pt = &(global.env.elements_list[i]);
		/* Detection d'un element deja present par une equation
		de cercle (rayon du cercle = diametre d'une element) */
		sq_delta_x=(x-element_pt->x)*(x-element_pt->x);
		sq_delta_y=(y-element_pt->y)*(y-element_pt->y);
		/* NOTE : c'est un exemple typique où on peu se planter
		quand on ne fait pas attention au types ... */
		if(sq_delta_x+sq_delta_y<ELEMENTS_SQUARED_DIAMETER)
		{
			if(build
				&& GEOMETRY_squared_distance((GEOMETRY_point_t){element_pt->x,element_pt->y},(GEOMETRY_point_t){x,y}) < 2500)
			{
				element_pt->type=ELEMENTS_build(element_pt->type,type);
				//assert(element_pt->type<TOWER_TWO_PAWN_AND_KING);
			}
			else if(type>element_pt->type)
			{
				element_pt->type=type;
			}

			/* Si la source est sur on mets a jour les coordonnees et le numero de zone */
			if(update_coords)
			{
				element_pt->x=x;
				element_pt->y=y;
				element_pt->area_number=area_number;
			}

			element_pt->element_add_time=global.env.match_time;

			#ifdef USE_ELEMENT_CAN_DEBUG
				CAN_send_element_updated(UPDATE,i);
			#endif /* def USE_ELEMENT_CAN_DEBUG */

			elements_printf("Updating in add_or_update: %d, coords : (%d, %d), area : %d, type : %d\r\n",
				i,element_pt->x,element_pt->y,element_pt->area_number,element_pt->type);
			return;
		}
	}

	element_pt=&(global.env.elements_list[global.env.nb_elements]);
	element_pt->area_number = area_number;
	area_pt=&(area_map[element_pt->area_number]);

	assert(area_pt->nb_elements<NB_ELEMENTS_MAX_IN_AREA);

	/* global.env.nb_elements etant le nombre d'elements enregistres,
	la dernière valeur correspond donc au numero du nouvel element ajoute */
	area_pt->elements_number[area_pt->nb_elements]=global.env.nb_elements;
	area_pt->nb_elements++;
	global.env.nb_elements++;
	element_pt->x=x;
	element_pt->y=y;
	/* Dans ce cas on ajoute juste le type de l'element */
	element_pt->type=type;
	element_pt->element_add_time=global.env.match_time;
	element_pt->taken_already_tried=FALSE;

	#ifdef USE_ELEMENT_CAN_DEBUG
		CAN_send_element_updated(ADD,global.env.nb_elements-1);
	#endif /* def USE_ELEMENT_CAN_DEBUG */

	elements_printf("Adding in add_or_update: %d, coords : (%d, %d), area : %d, type : %d\r\n",
		global.env.nb_elements-1,element_pt->x,element_pt->y,element_pt->area_number+1,element_pt->type);
}

Uint8 ELEMENTS_conversion(Sint16 x, Sint16 y)
{
	Uint8 case_number=0;

	area_mapping_t const* area_pt=0;
	Sint16 x_top, x_bottom, y_left, y_right;

	while(case_number<NB_AREAS)
	{
		area_pt=&area_mapping_map[case_number];
		switch(area_pt->type)
		{
			case SCORING_AREA:
			case BONUS_SCORING_AREA:
				x_top=
					(area_pt->mode==TOP_AREA)?CASE_SIZE_X/2:SCORING_AREA_SIZE_X/2;
				x_bottom=
					(area_pt->mode==BOTTOM_AREA)?CASE_SIZE_X/2:SCORING_AREA_SIZE_X/2;

				if(x >= area_pt->coords.x-x_top
					&& x <= area_pt->coords.x+x_bottom
					&& y >= area_pt->coords.y-SCORING_AREA_SIZE_Y/2
					&& y <= area_pt->coords.y+SCORING_AREA_SIZE_Y/2)
				{
					return case_number;
				}
				break;

			case SECURE_SCORING_AREA:
				if(x >= area_pt->coords.x-115
					&& x <= area_pt->coords.x+115
					&& y >= area_pt->coords.y-SCORING_AREA_SIZE_Y/2
					&& y <= area_pt->coords.y+SCORING_AREA_SIZE_Y/2)
				{
					return case_number;
				}
				break;

			case INTER_AREA:
				x_top=
					(area_pt->mode==TOP_AREA
					|| area_pt->mode==TOP_LEFT_AREA
					|| area_pt->mode==TOP_RIGHT_AREA)?CASE_SIZE_X:CASE_SIZE_X/2;
				x_bottom=
					(area_pt->mode==BOTTOM_AREA
					|| area_pt->mode==BOTTOM_LEFT_AREA
					|| area_pt->mode==BOTTOM_RIGHT_AREA)?CASE_SIZE_X:CASE_SIZE_X/2;
				y_left=
					(area_pt->mode==LEFT_AREA
					|| area_pt->mode==TOP_LEFT_AREA
					|| area_pt->mode==BOTTOM_LEFT_AREA)?0:CASE_SIZE_Y/2;
				y_right=
					(area_pt->mode==RIGHT_AREA
					|| area_pt->mode==TOP_RIGHT_AREA
					|| area_pt->mode==BOTTOM_RIGHT_AREA)?0:CASE_SIZE_Y/2;

				if(x >= area_pt->coords.x-x_top
					&& x <= area_pt->coords.x+x_bottom
					&& y >= area_pt->coords.y-y_left
					&& y <= area_pt->coords.y+y_right)
				{
					return case_number;
				}
				break;

			case GREEN_AREA:
				x_top=
					(area_pt->mode==TOP_AREA)?290:GREEN_AREA_SIZE_X/2;
				x_bottom=
					(area_pt->mode==BOTTOM_AREA)?290:GREEN_AREA_SIZE_X/2;

				if(x >= area_pt->coords.x-x_top
					&& x <= area_pt->coords.x+x_bottom
					&& y >= area_pt->coords.y-GREEN_AREA_SIZE_Y/2
					&& y <= area_pt->coords.y+GREEN_AREA_SIZE_Y/2)
				{
					return case_number;
				}
				break;

			case START_AREA:
				if(x >= area_pt->coords.x-START_AREA_SIZE_X/2
					&& x <= area_pt->coords.x+START_AREA_SIZE_X/2
					&& y >= area_pt->coords.y-START_AREA_SIZE_Y/2
					&& y <= area_pt->coords.y+START_AREA_SIZE_Y/2)
				{
					return case_number;
				}
				break;

			default:
				break;
		}
		case_number++;
	}
	return NB_AREAS;
}

static bool_e ELEMENTS_exist_in_elements_list(Sint16 x, Sint16 y)
{
	Uint8 i;
	Sint32 sq_delta_x,sq_delta_y;
	element_t* element_pt;

	for(i=0;i<global.env.nb_elements;i++)
	{
		element_pt=&(global.env.elements_list[i]);
		sq_delta_x=(x-element_pt->x)*(x-element_pt->x);
		sq_delta_y=(y-element_pt->y)*(y-element_pt->y);

		if(sq_delta_x+sq_delta_y<ELEMENTS_SQUARED_RADIUS)
		{
			return TRUE;
		}
	}
	return FALSE;
}

static bool_e ELEMENTS_exist_in_area(Sint16 x, Sint16 y)
{
	Uint8 i;
	Sint32 sq_delta_x,sq_delta_y;
	element_t* element_pt;
	area_t* area_pt = &(area_map[ELEMENTS_conversion(x,y)]);

	for(i=0;i<area_pt->nb_elements;i++)
	{
		element_pt=&(global.env.elements_list[area_pt->elements_number[i]]);
		sq_delta_x=(x-element_pt->x)*(x-element_pt->x);
		sq_delta_y=(y-element_pt->y)*(y-element_pt->y);

		if(sq_delta_x+sq_delta_y<ELEMENTS_SQUARED_RADIUS)
		{
			return TRUE;
		}
	}
	return FALSE;
}

static void ELEMENTS_delete_from_elements_list(Uint8 element_number)
{
	Uint8 i;
	area_t* area_pt;

	assert(element_number<global.env.nb_elements);

	if(element_number!=global.env.nb_elements-1)
	{
		/* On remplace l'element supprime par le dernier element */
		global.env.elements_list[element_number]=global.env.elements_list[global.env.nb_elements-1];
		area_pt=&(area_map[global.env.elements_list[element_number].area_number]);

		/* Permet de repercuter le deplacement dans le tableau des zones */
		for(i=0;i<(*area_pt).nb_elements;i++)
		{
			if((*area_pt).elements_number[i]==global.env.nb_elements-1)
			{
				(*area_pt).elements_number[i]=element_number;
			}
		}
	}

	global.env.nb_elements--;
}

static void ELEMENTS_delete_from_areas(area_t* area_pt, Uint8 element_number_in_area)
{
	assert(element_number_in_area<NB_ELEMENTS_MAX_IN_AREA);

	if(element_number_in_area!=area_pt->nb_elements-1)
	{
		area_pt->elements_number[element_number_in_area]=area_pt->elements_number[area_pt->nb_elements];
	}

	area_pt->nb_elements--;
}

static void ELEMENTS_clear_area(Uint8 area_number)
{
	area_t* area_pt;

	assert(area_number<NB_AREAS);

	area_pt = &(area_map[area_number]);

	/* Suppression de tous les elements */
	while(area_pt->nb_elements>0)
	{
		/*	En supprimant l'élément d'indice la première case
		*	du tableau de nombre d'éléments de la zone...
		*/
		ELEMENTS_delete_from_elements_list(area_pt->elements_number[0]);
		/* ...puis en supprimant cette première case, on réalise
		*	la suppression de tous les éléments à chaque boucle.
		*/
		ELEMENTS_delete_from_areas(area_pt,0);
	}
}

game_element_e ELEMENTS_type_element(Sint16 x, Sint16 y)
{
	Uint8 i;
	Sint32 sq_delta_x,sq_delta_y;
	element_t* element_pt;

	for(i=0;i<global.env.nb_elements;i++)
	{
		element_pt=&(global.env.elements_list[i]);

		sq_delta_x=(x-element_pt->x)*(x-element_pt->x);
		sq_delta_y=(y-element_pt->y)*(y-element_pt->y);

		if(sq_delta_x+sq_delta_y<ELEMENTS_SQUARED_RADIUS)
		{
			/* On renvoie le type de l'element correspondant */
			return element_pt->type;
		}
	}
	/* Element inconnu, on ne connait pas cet element */
	return BLACK_CD;//PAWN;
}

game_element_e ELEMENTS_build(game_element_e element_1, game_element_e element_2)
{
	// element_1 : élement sur le terrain
	// element_2 : élement dans la pince

	/* On retourne un élément construit inconnu */
	return element_1 + element_2;
}



Uint8 ELEMENTS_nb_points_us()
{
	Uint8 nb_points=0,i;
	element_t *element_pt;

	for(i=0;i<global.env.nb_elements;i++)
	{
		element_pt=&global.env.elements_list[i];

		if(area_map[(element_pt->area_number)].belonging==US)
		{
			nb_points+=element_pt->type;
		}
	}

	return nb_points;
}

Uint8 ELEMENTS_nb_points_foe()
{
	Uint8 nb_points = 0, i;
	element_t *element_pt;

	for(i=0;i<global.env.nb_elements;i++)
	{
		element_pt = &global.env.elements_list[i];

		if(area_map[(element_pt->area_number)].belonging==ADVERSARY)
		{
			nb_points+=element_pt->type;
		}
	}

	return nb_points;
}

Uint8 ELEMENTS_furthest_foe(game_element_e type, belonging_e belonging)
{
	element_t* element_pt;
	Uint8 i,furthest_foe_element_number=NB_ELEMENTS;
	Uint32 distance,max_distance=0;
	GEOMETRY_point_t foe_position={global.env.foe[FOE_1].x,global.env.foe[FOE_1].y}, element_position;

	if(global.env.nb_elements==0)
	{
		return NB_ELEMENTS;
	}

	for(i=0;i<global.env.nb_elements;i++)
	{
		element_pt=&global.env.elements_list[i];
		if((type==UNDEFINED_ELEMENT || type==element_pt->type)
		&& (belonging==UNDEFINED_BELONGING || belonging==area_map[element_pt->area_number].belonging)
		&& area_mapping_map[element_pt->area_number].type!=SECURE_SCORING_AREA)
		{
			element_position=(GEOMETRY_point_t){element_pt->x,element_pt->y};
			distance=GEOMETRY_squared_distance(foe_position,element_position);

			if(distance>max_distance)
			{
				max_distance=distance;
				furthest_foe_element_number=i;
			}
		}
	}
	return furthest_foe_element_number;
}


void ELEMENTS_display()
{
	Uint8 i;
	element_t* element_pt;

	elements_printf("\r\n--------------------------------------------------\r\n");
	for(i=0;i<global.env.nb_elements;i++)
	{
		element_pt=&global.env.elements_list[i];
		debug_printf("| X = %.4d -- Y = %.4d -- TYPE : %.2d -- AREA : %.2d |\r\n",
			element_pt->x,element_pt->y,element_pt->type,element_pt->area_number+1);
	}
	elements_printf("--------------------------------------------------\r\n");
}

#ifdef USE_ELEMENT_CAN_DEBUG

#include "QS/QS_CANmsgList.h"

/* Envoi d'un message CAN lors d'une mise a jour en debug */
static void CAN_send_element_updated(event_type_e event, Uint8 element_number)
{
	CAN_msg_t msg;

	msg.sid=DEBUG_ELEMENT_UPDATED;
	msg.data[0]=event;
	msg.data[1]=global.env.elements_list[element_number].type;
	msg.data[2]=HIGHINT(global.env.elements_list[element_number].x);
	msg.data[3]=LOWINT(global.env.elements_list[element_number].x);
	msg.data[4]=HIGHINT(global.env.elements_list[element_number].y);
	msg.data[5]=LOWINT(global.env.elements_list[element_number].y);
	msg.size=6;
	CAN_send(&msg);
}

#endif /* def USE_ELEMENT_CAN_DEBUG */

#ifdef USE_ELEMENT_DISPLAY

	#define WRITE_ON_BLUE 		44
	#define WRITE_ON_YELLOW 	43
	#define WRITE_ON_GREEN 		42
	#define WRITE_ON_RED 		41
	#define WRITE_ON_BLACK 		40
	void ELEMENTS_display_ascii()
	{
		const char displayer[][512] =
			{
				{"\E[%d;%dm_____\E[%d;%dm_\E[%d;%dm___\E[33;43m_\E[%d;%dm_\E[%dm%c\E[%dm_"
					"\E[%d;%dm_\E[%d;%dm_\E[%dm%c\E[%dm_\E[33;43m_\E[%d;%dm_\E[%dm%c\E[%dm_\E[%d;%dm_"
					"\E[%d;%dm_\E[%dm%c\E[%dm_\E[33;43m_\E[%d;%dm___\E[%d;%dm_\E[%d;%dm_____\E[40m\r\n"},
				{"\E[%d;%dm__\E[30m%d\E[%dm__\E[33;43m_\E[%d;%dm_\E[30m%d\E[%dm_\E[33;43m_\E[%d;%dm_"
					"\E[30m%d\E[%dm_\E[33;43m_\E[%d;%dm_\E[30m%d\E[%dm_\E[33;43m_\E[%d;%dm_\E[30m%d"
					"\E[%dm_\E[33;43m_\E[%d;%dm_\E[30m%d\E[%dm_\E[33;43m_\E[%d;%dm_\E[30m%d\E[%dm_"
					"\E[33;43m_\E[%d;%dm__\E[30m%d\E[%dm__\E[40m\r\n"},
				{"\E[39;%dm_____\E[30;43m%d\E[39m___\E[30m%d\E[39m___\E[30m%d\E[39m___\E[30m%d\E[39m___\E[30m%d\E[39m___\E[30m%d\E[39m___\E[30m%d\E[39;%dm_____\E[40m\r\n"},
				{"\E[39;42m_____\E[40m_________\E[41m___\E[43m_\E[44m___\E[40m_________\E[42m_____\E[40m\r\n"}
			};
		static Sint8 line=-1;
		Uint8 left_color=WRITE_ON_BLACK;
		Uint8 right_color=WRITE_ON_BLACK;
		Uint8 line_color=WRITE_ON_BLACK;
		Uint8 even_color=0,odd_color=WRITE_ON_BLACK;
		Uint8 bonus_color[4];
		Uint8 values[8]={0};
		Uint8 display=3;
		Uint8 bonus_area,bonus_area2;

		for(;line<19;line++)
		{
			switch(line)
			{
				case -1:
					/*retour en haut du terminal*/
					debug_printf("\E[40m\E[2J");
					line = 0;
				case 0:
					debug_printf("\E[0;0H");
				case 2:	display	= 0;
					left_color = WRITE_ON_RED;
					right_color = WRITE_ON_BLUE;
					line_color = WRITE_ON_YELLOW;
					odd_color = WRITE_ON_BLUE;
					even_color = WRITE_ON_RED;
					break;

				case 1:
					display	= 1;
					left_color = WRITE_ON_RED;
					right_color = WRITE_ON_BLUE;
					line_color = WRITE_ON_YELLOW;
					odd_color = WRITE_ON_BLUE;
					even_color = WRITE_ON_RED;
					values[0]=area_map[80].nb_elements;
					values[1]=area_map[0].nb_elements;
					values[2]=area_map[1].nb_elements;
					values[3]=area_map[2].nb_elements;
					values[4]=area_map[3].nb_elements;
					values[5]=area_map[4].nb_elements;
					values[6]=area_map[5].nb_elements;
					values[7]=area_map[81].nb_elements;
					break;

				case 3:
					display	= 2;
					left_color = WRITE_ON_RED;
					right_color = WRITE_ON_BLUE;
					line_color = WRITE_ON_YELLOW;
					values[0]=area_map[36].nb_elements;
					values[1]=area_map[37].nb_elements;
					values[2]=area_map[38].nb_elements;
					values[3]=area_map[39].nb_elements;
					values[4]=area_map[40].nb_elements;
					values[5]=area_map[41].nb_elements;
					values[6]=area_map[42].nb_elements;
					break;

				case 4:
					display	= 1;
					left_color = WRITE_ON_GREEN;
					right_color = WRITE_ON_GREEN;
					line_color = WRITE_ON_YELLOW;
					odd_color = WRITE_ON_RED;
					even_color = WRITE_ON_BLUE;
					values[0]=area_map[71].nb_elements;
					values[1]=area_map[6].nb_elements;
					values[2]=area_map[7].nb_elements;
					values[3]=area_map[8].nb_elements;
					values[4]=area_map[9].nb_elements;
					values[5]=area_map[10].nb_elements;
					values[6]=area_map[11].nb_elements;
					values[7]=area_map[76].nb_elements;
					break;

				case 5:
				case 11:
					display	= 0;
					left_color = WRITE_ON_GREEN;
					right_color = WRITE_ON_GREEN;
					line_color = WRITE_ON_YELLOW;
					odd_color = WRITE_ON_RED;
					even_color = WRITE_ON_BLUE;
					break;

				case 6:
					display	= 2;
					left_color = WRITE_ON_GREEN;
					right_color = WRITE_ON_GREEN;
					line_color = WRITE_ON_YELLOW;
					values[0]=area_map[43].nb_elements;
					values[1]=area_map[44].nb_elements;
					values[2]=area_map[45].nb_elements;
					values[3]=area_map[46].nb_elements;
					values[4]=area_map[47].nb_elements;
					values[5]=area_map[48].nb_elements;
					values[6]=area_map[49].nb_elements;
					break;

				case 7:
					display	= 1;
					left_color = WRITE_ON_GREEN;
					right_color = WRITE_ON_GREEN;
					line_color = WRITE_ON_YELLOW;
					odd_color = WRITE_ON_BLUE;
					even_color = WRITE_ON_RED;
					values[0]=area_map[72].nb_elements;
					values[1]=area_map[12].nb_elements;
					values[2]=area_map[13].nb_elements;
					values[3]=area_map[14].nb_elements;
					values[4]=area_map[15].nb_elements;
					values[5]=area_map[16].nb_elements;
					values[6]=area_map[17].nb_elements;
					values[7]=area_map[77].nb_elements;
					break;

				case 8:
				case 14:
					display	= 0;
					left_color = WRITE_ON_GREEN;
					right_color = WRITE_ON_GREEN;
					line_color = WRITE_ON_YELLOW;
					odd_color = WRITE_ON_BLUE;
					even_color = WRITE_ON_RED;
					break;

				case 9:
					display	= 2;
					left_color = WRITE_ON_GREEN;
					right_color = WRITE_ON_GREEN;
					line_color = WRITE_ON_YELLOW;
					values[0]=area_map[50].nb_elements;
					values[1]=area_map[51].nb_elements;
					values[2]=area_map[52].nb_elements;
					values[3]=area_map[53].nb_elements;
					values[4]=area_map[54].nb_elements;
					values[5]=area_map[56].nb_elements;
					values[6]=area_map[57].nb_elements;
					break;

				case 10:
					display	= 1;
					left_color = WRITE_ON_GREEN;
					right_color = WRITE_ON_GREEN;
					line_color = WRITE_ON_YELLOW;
					odd_color = WRITE_ON_RED;
					even_color = WRITE_ON_BLUE;
					values[0]=area_map[73].nb_elements;
					values[1]=area_map[18].nb_elements;
					values[2]=area_map[19].nb_elements;
					values[3]=area_map[20].nb_elements;
					values[4]=area_map[21].nb_elements;
					values[5]=area_map[22].nb_elements;
					values[6]=area_map[23].nb_elements;
					values[7]=area_map[78].nb_elements;
					break;

				case 12:
					display	= 2;
					left_color = WRITE_ON_GREEN;
					right_color = WRITE_ON_GREEN;
					line_color = WRITE_ON_YELLOW;
					values[0]=area_map[58].nb_elements;
					values[1]=area_map[59].nb_elements;
					values[2]=area_map[60].nb_elements;
					values[3]=area_map[61].nb_elements;
					values[4]=area_map[62].nb_elements;
					values[5]=area_map[63].nb_elements;
					values[6]=area_map[64].nb_elements;
					break;

				case 13:
					display	= 1;
					left_color = WRITE_ON_GREEN;
					right_color = WRITE_ON_GREEN;
					line_color = WRITE_ON_YELLOW;
					odd_color = WRITE_ON_BLUE;
					even_color = WRITE_ON_RED;
					values[0]=area_map[74].nb_elements;
					values[1]=area_map[24].nb_elements;
					values[2]=area_map[25].nb_elements;
					values[3]=area_map[26].nb_elements;
					values[4]=area_map[27].nb_elements;
					values[5]=area_map[28].nb_elements;
					values[6]=area_map[29].nb_elements;
					values[7]=area_map[79].nb_elements;
					break;

				case 15:
					display	= 2;
					left_color = WRITE_ON_GREEN;
					right_color = WRITE_ON_GREEN;
					line_color = WRITE_ON_YELLOW;
					values[0]=area_map[65].nb_elements;
					values[1]=area_map[66].nb_elements;
					values[2]=area_map[67].nb_elements;
					values[3]=area_map[68].nb_elements;
					values[4]=area_map[69].nb_elements;
					values[5]=area_map[70].nb_elements;
					values[6]=area_map[71].nb_elements;
					break;

				case 16:
					display	= 1;
					left_color = WRITE_ON_GREEN;
					right_color = WRITE_ON_GREEN;
					line_color = WRITE_ON_YELLOW;
					odd_color = WRITE_ON_RED;
					even_color = WRITE_ON_BLUE;
					values[0]=area_map[75].nb_elements;
					values[1]=area_map[30].nb_elements;
					values[2]=area_map[31].nb_elements;
					values[3]=area_map[32].nb_elements;
					values[4]=area_map[33].nb_elements;
					values[5]=area_map[34].nb_elements;
					values[6]=area_map[35].nb_elements;
					values[7]=area_map[80].nb_elements;
					break;

				case 17:
					display	= 0;
					left_color = WRITE_ON_GREEN;
					right_color = WRITE_ON_GREEN;
					line_color = WRITE_ON_BLACK;
					odd_color = WRITE_ON_RED;
					even_color = WRITE_ON_BLUE;
					break;

				case 18: display=3;
					break;
				default:
					break;
			}

			bonus_area=(line==5||line==11)?'B':'_';
			bonus_area2=(line==17)?'B':'_';

			bonus_color[0]=(line==5||line==11)?30:even_color-10;
			bonus_color[1]=(line==17)?30:odd_color-10;
			bonus_color[2]=(line==17)?30:even_color-10;
			bonus_color[3]=(line==5||line==11)?30:odd_color-10;

			switch(display)
			{
				case 0:debug_printf(displayer[0],left_color-10,left_color,line_color-10,line_color,odd_color-10,odd_color,even_color-10,even_color,
							bonus_color[0],bonus_area,even_color-10,line_color-10,line_color,odd_color-10,odd_color,bonus_color[1],bonus_area2,odd_color-10,
							even_color-10,even_color,bonus_color[2],bonus_area2,even_color-10,line_color-10,line_color,odd_color-10,odd_color,
							bonus_color[3],bonus_area,odd_color-10,even_color-10,even_color,line_color-10,line_color,right_color-10,right_color);
					break;
				case 1:debug_printf(displayer[1],left_color-10,left_color,values[0],left_color-10,odd_color-10,odd_color,values[1],odd_color-10,
							even_color-10,even_color,values[2],even_color-10,odd_color-10,odd_color,values[3],odd_color-10,even_color-10,even_color,
							values[4],even_color-10,odd_color-10,odd_color,values[5],odd_color-10,even_color-10,even_color,values[6],even_color-10,
							right_color-10,right_color,values[7],right_color-10);
					break;
				case 2:debug_printf(displayer[2],left_color,values[0],values[1],values[2],values[3],values[4],values[5],values[6],right_color);
					break;
				case 3:debug_printf(displayer[3]);
					break;
				default:
					break;
			}
		}
		line = 0;
	}
#endif /* ndef USE_ELEMENT_DISPLAY */

void sort_elements(element_t elements_list[],Uint8 num_elements[],Uint8 size_tab,bool_e (*compare) (element_t a,element_t b))
{
	Uint8 i,change=1,tmp;

	for(i=0;i<size_tab;i++)
	{
		num_elements[i] = i;
	}

	while(change>0 && global.env.nb_elements>0)
	{
		change = 0;

		for(i=0;i<size_tab-1;i++)
		{
			if(compare(elements_list[num_elements[i]],elements_list[num_elements[i+1]]))
			{
				tmp = num_elements[i];
				num_elements[i] = num_elements[i+1];
				num_elements[i+1] = tmp;
				change++;
			}
		}
	}
}

static bool_e compare_distance(element_t current_element,element_t next_element)
{
	GEOMETRY_point_t pos_robot = {global.env.pos.x,global.env.pos.y};
	GEOMETRY_point_t pt_current_element = {current_element.x,current_element.y};
	GEOMETRY_point_t pt_next_element = {next_element.x,next_element.y};
	Uint16 dist_current_element = GEOMETRY_distance(pt_current_element,pos_robot);
	Uint16 dist_next_element = GEOMETRY_distance(pt_next_element,pos_robot);
	if(dist_current_element > dist_next_element)
		return TRUE;
	else
		return FALSE;
}

static bool_e compare_types(element_t current_element,element_t next_element)
{
	if(current_element.type < next_element.type)
		return TRUE;
	else
		return FALSE;
}

static bool_e compare_foe_position(element_t current_element,element_t next_element)
{
	GEOMETRY_point_t pos_adversary = {global.env.foe[FOE_1].x,global.env.foe[FOE_1].y};
	GEOMETRY_point_t pt_current_element = {current_element.x,current_element.y};
	GEOMETRY_point_t pt_next_element = {next_element.x,next_element.y};
	Uint16 dist_current_element = GEOMETRY_distance(pt_current_element,pos_adversary);
	Uint16 dist_next_element = GEOMETRY_distance(pt_next_element,pos_adversary);
	if(dist_current_element > dist_next_element)
		return TRUE;
	else
		return FALSE;
}

static bool_e compare_area(area_mapping_t area_a,area_mapping_t area_b)
{
	GEOMETRY_point_t pos_robot = {global.env.pos.x,global.env.pos.y};
	GEOMETRY_point_t pt_area_a = area_a.coords;
	GEOMETRY_point_t pt_area_b = area_b.coords;
	Uint16 dist_area_a = GEOMETRY_distance(pt_area_a,pos_robot);
	Uint16 dist_area_b = GEOMETRY_distance(pt_area_b,pos_robot);
	if(dist_area_a < dist_area_b)
		return TRUE;
	else
		return FALSE;
}

static void sort_empty_area(Uint8 area_number_list[], Uint8* nb_empty_area, bool_e (*compare) (area_mapping_t area_a,area_mapping_t area_b))
{
//	Uint8 i,tmp;
//	Uint8 current_pos_list = 0;
//	Uint8 nb_area_bonus_empty = 0;
//	bool_e end_sort;
//
//	nb_empty_area = 0;
//
//	if(area_map[bonus_area_number[0]].belonging == US)
//	{
//		if(area_map[bonus_area_number[0]].nb_elements == 0 && area_map[bonus_area_number[2]].nb_elements != 0)
//		{
//			area_number_list[current_pos_list] = bonus_area_number[0];
//			current_pos_list++;
//			nb_area_bonus_empty = 1;
//			nb_empty_area += 1;
//		}
//		else if(area_map[bonus_area_number[2]].nb_elements == 0 && area_map[bonus_area_number[0]].nb_elements != 0)
//		{
//			area_number_list[current_pos_list] = bonus_area_number[2];
//			current_pos_list++;
//			nb_area_bonus_empty = 1;
//			nb_empty_area += 1;
//		}
//		else if(area_map[bonus_area_number[0]].nb_elements == 0 && area_map[bonus_area_number[2]].nb_elements == 0)
//		{
//			if(compare(area_mapping_map[bonus_area_number[0]], area_mapping_map[bonus_area_number[2]]))// compare zone a plus proche que zone b
//			{
//				area_number_list[current_pos_list] = bonus_area_number[0];
//				area_number_list[current_pos_list+1] = bonus_area_number[2];
//				nb_area_bonus_empty = 2;
//				nb_empty_area += 2;
//			}
//			else
//			{
//				area_number_list[current_pos_list] = bonus_area_number[2];
//				area_number_list[current_pos_list+1] = bonus_area_number[0];
//				nb_area_bonus_empty = 2;
//				nb_empty_area += 2;
//			}
//			current_pos_list += 2;
//		}
//	}
//	else
//	{
//		if(area_map[bonus_area_number[1]].nb_elements == 0 && area_map[bonus_area_number[3]].nb_elements != 0)
//		{
//			area_number_list[current_pos_list] = bonus_area_number[1];
//			current_pos_list++;
//			nb_area_bonus_empty = 1;
//			nb_empty_area += 1;
//		}
//		else if(area_map[bonus_area_number[3]].nb_elements == 0 && area_map[bonus_area_number[1]].nb_elements != 0)
//		{
//			area_number_list[current_pos_list] = bonus_area_number[3];
//			current_pos_list++;
//			nb_area_bonus_empty = 1;
//			nb_empty_area += 1;
//		}
//		else if(area_map[bonus_area_number[1]].nb_elements == 0 && area_map[bonus_area_number[3]].nb_elements == 0)
//		{
//			if(compare(area_mapping_map[bonus_area_number[1]], area_mapping_map[bonus_area_number[3]]))// compare zone a plus proche que zone b
//			{
//				area_number_list[current_pos_list] = bonus_area_number[1];
//				area_number_list[current_pos_list+1] = bonus_area_number[3];
//				nb_area_bonus_empty = 2;
//				nb_empty_area += 2;
//			}
//			else
//			{
//				area_number_list[current_pos_list] = bonus_area_number[3];
//				area_number_list[current_pos_list+1] = bonus_area_number[1];
//				nb_area_bonus_empty = 2;
//				nb_empty_area += 2;
//			}
//			current_pos_list += 2;
//		}
//	}
//
//	// calcul du nombre de zones vides à nous
//	for(i=0;i<NB_AREAS;i++)// 82 = nbre de zones
//	{
//		if(area_mapping_map[i].type == SCORING_AREA)
//		{
//			if(area_map[i].belonging == US && area_map[i].nb_elements == 0)
//			{
//				if(	i != bonus_area_number[0] &&
//					i != bonus_area_number[1] &&
//					i != bonus_area_number[2] &&
//					i != bonus_area_number[3]
//					)
//				{
//					area_number_list[current_pos_list] = i;
//					current_pos_list++;
//					nb_empty_area += 1;
//				}
//			}
//		}
//	}
//	if(nb_area_bonus_empty == 0)
//	{
//		do{
//			end_sort = TRUE;
//			for(i=0;i<*nb_empty_area-1;i++)
//			{
//				if(!compare(area_mapping_map[area_number_list[i]],area_mapping_map[area_number_list[i+1]]))//ok a + proche b
//				{
//					tmp = area_number_list[i];
//					area_number_list[i+1] = area_number_list[i];
//					area_number_list[i] = tmp;
//					end_sort = FALSE;
//				}
//			}
//		}while(!end_sort);
//	}
//	else if(nb_area_bonus_empty == 1)
//	{
//		do{
//			end_sort = TRUE;
//			for(i=1;i<*nb_empty_area-1;i++)
//			{
//				if(!compare(area_mapping_map[area_number_list[i]],area_mapping_map[area_number_list[i+1]]))//ok a + proche b
//				{
//					tmp = area_number_list[i];
//					area_number_list[i+1] = area_number_list[i];
//					area_number_list[i] = tmp;
//					end_sort = FALSE;
//				}
//			}
//		}while(!end_sort);
//	}
//	else if(nb_area_bonus_empty == 2)
//	{
//		do{
//			end_sort = TRUE;
//			for(i=2;i<*nb_empty_area-1;i++)
//			{
//				if(!compare(area_mapping_map[area_number_list[i]],area_mapping_map[area_number_list[i+1]]))//ok a + proche b
//				{
//					tmp = area_number_list[i];
//					area_number_list[i+1] = area_number_list[i];
//					area_number_list[i] = tmp;
//					end_sort = FALSE;
//				}
//			}
//		}while(!end_sort);
//	}
}
/* Fonction qui indique si l'adversaire est proche du point passé en paramètre
* TRUE = adversaire trop proche
* FALSE = adversaire assez loin
*/
bool_e ELEMENT_foe_near_of_point(GEOMETRY_point_t target_point, Uint32 minimal_distance)
{
	GEOMETRY_point_t foe_pos;

	foe_pos.x = global.env.foe[FOE_1].x;
	foe_pos.y = global.env.foe[FOE_1].y;

	if(GEOMETRY_squared_distance(foe_pos,target_point) > (1UL*minimal_distance*minimal_distance))
	{
		// adversaire assez loin !
		debug_printf("Adversaire assez loin !\n");
		return FALSE;
	}
	else
	{
		// adversaire trop proche
		debug_printf("Adversaire trop proche !\n");
		return TRUE;
	}
}

/* Fonction qui indique si un nouvel élement a été détecté
* TRUE = nouvel élément détecté
* FALSE = pas de nouvel élement détecté
*/
bool_e ELEMENT_new_element_detected(Uint8 old_element_number, element_t last_element_saved)
{
	if((global.env.nb_elements > old_element_number)
		|| ((global.env.elements_list[global.env.nb_elements].x == last_element_saved.x)
			&& global.env.elements_list[global.env.nb_elements].y == last_element_saved.y))
	{
		debug_printf("Nouveaux éléments ajoutés\n");
		return TRUE;
	}
	debug_printf("Pas de nouvel élément détecté\n");
	return FALSE;
}

/* Fonction qui retourne un élément, du plus proche au plus loin, selon la position de l'adversaire */
Uint8 ELEMENT_near_element_compute_foe()
{
	Uint8 element_number_sort_by_distance[NB_ELEMENTS], i;

	// on trie notre tableau d'éléments
	sort_elements(global.env.elements_list,element_number_sort_by_distance,global.env.nb_elements,compare_distance);

	for(i=0;i<global.env.nb_elements;i++)
	{
		if(ELEMENTS_belonging_type(element_number_sort_by_distance[i]) != US)
		{
			debug_printf("Element %d ne nous appartient pas\n",element_number_sort_by_distance[i]);
			// on va tester si il est pas trop proche de l'adversaire
			if(ELEMENT_foe_near_of_point((GEOMETRY_point_t)
				{global.env.elements_list[element_number_sort_by_distance[i]].x,global.env.elements_list[element_number_sort_by_distance[i]].y},
					MINIMAL_ELEMENT_FOE_DISTANCE)==FALSE)
			{
				debug_printf("Element %d assez éloigné de l'adversaire !\n",element_number_sort_by_distance[i]);
				return element_number_sort_by_distance[i];
			}
		}
		else
		{
			debug_printf("Element %d nous appartient\n",element_number_sort_by_distance[i]);
		}
	}
	return NB_ELEMENTS;
}

/* Fonction qui recherche la case vide la plus proche et assez loin de l'adversaire */
Uint8 ELEMENT_near_empty_area_compute_foe()
{
	Uint8 area_number_sort_by_distance[NB_SCORING_AREA], i;
	Uint8 nb_areas_sort = NB_SCORING_AREA;

	// on trie nos cases vides de la plus proche à la plus éloignée, Merci Patrick !
	sort_empty_area(area_number_sort_by_distance,&nb_areas_sort, compare_area);

	for(i=0;i<nb_areas_sort;i++)
	{
		// On va regarder si la case est assez loin de l'adversaire
		if(ELEMENT_foe_near_of_point(ELEMENTS_get_area_center(area_number_sort_by_distance[i]),MINIMAL_AREA_FOE_DISTANCE)==FALSE)
		{
			debug_printf("La case %d est assez éloignée de l'adversaire\n",area_number_sort_by_distance[i]+1);
			return area_number_sort_by_distance[i];
		}
	}

	debug_printf("Aucune case de trouvée !\n");
	return NB_SCORING_AREA;
}
#endif
