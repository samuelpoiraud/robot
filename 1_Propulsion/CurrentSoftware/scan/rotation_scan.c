#include "rotation_scan.h"

#include "../QS/QS_outputlog.h"
#include "../QS/QS_who_am_i.h"
#include "../QS/QS_maths.h"
#include "../QS/QS_measure.h"
#include "../supervisor.h"
#include "../secretary.h"
#include "../corrector.h"
#include "../roadmap.h"
#include "../_Propulsion_config.h"
#include "../odometry.h"
#include "../config/config_use.h"
#include "../QS/QS_watchdog.h"

#define NB_POINT_MAX 200
#define NB_POINTS_MINI_CORNER  25
#define MARGIN 		100
#define DISTANCE_MAX 20
#define CORNER_SCAN_PSEUDO_VARIANCE_SEUIL	10
#define COS_CARRE_SCALAIRE	0.00030458649//pour une erreur d'angle de 1� entre les droites




//------------------------------------------------------------------------------------ D�finition des structures et �numerations

typedef struct{
	position_t robot;
	Uint16 dist;
}scan_result_t;


typedef enum{
	NO_MSG_CAN,
	MSG_CAN_LAUNCH_SCAN
}receve_msg_can_e;

typedef struct{
	color_e color;				// 1 : couleur verte	0 : couleur jaune
	bool_e is_right_sensor;		// 1 : capteur droite   0 : capteur gauche
}proporties_scan;


//------------------------------------------------------------------------------------ Variables Globales

static GEOMETRY_point_t ourCornerColor[NB_POINT_MAX];
static GEOMETRY_point_t ourCornerSouth[NB_POINT_MAX];

static Uint8 nbPointCornerColor = 0;
static Uint8 nbPointCornerSouth = 0;

static bool_e run_calcul,end_scan;
static bool_e error_scan;

static receve_msg_can_e receve_msg_can;
static proporties_scan info_scan;
static position_t robotPosition;
Uint32 compteur_debug=0;


//------------------------------------------------------------------------------------ Prototype des fonctions local

static void scanOnePoint();

//------------------------------------------------------------------------------------ Fonctions


void SCAN_CORNER_init(void)
{
	run_calcul = FALSE;
}

//---------------------------------------------------------------------------------------------------------------
//												MAE Scan Corner
//---------------------------------------------------------------------------------------------------------------

void SCAN_CORNER_process_main(){
	if(run_calcul&&(!error_scan))
		SCAN_CORNER_calculate();
}

void SCAN_CORNER_process_it(){
	typedef enum{
		INIT=0,
		WAIT,
		PROCESS_SCAN,
		SCAN_CALCUL,
		WAIT_CALCULATE,
		SEND_COOR_ROBOT,
		END
	}state_e;

	static state_e state = INIT;

	switch(state){

		case INIT :
			receve_msg_can = NO_MSG_CAN;
			it_printf("\n\aziaoe\n\n");
			state = WAIT;
			break;

		case WAIT :
			switch(receve_msg_can){
				case MSG_CAN_LAUNCH_SCAN:
					it_printf("\n\nhhhhhhhhhhh\n\n");
					state = PROCESS_SCAN;
					break;
				case NO_MSG_CAN :
				default :
					break;
			}
			break;

		case PROCESS_SCAN:
			if(end_scan == FALSE){
				scanOnePoint();
			}else{
				it_printf("et l� tu passes ?\n");
				state = SCAN_CALCUL;
			}
			break;

		case SCAN_CALCUL:
			run_calcul = TRUE;
			it_printf("et l� alors ?\n");
			state = WAIT_CALCULATE;
			break;

		case WAIT_CALCULATE:
			printf("\n\nsrfgunfgjnkfgdfg\n\n");
		//	SCAN_CORNER_calculate();
			if((!run_calcul)||(error_scan))
				state = SEND_COOR_ROBOT;
			break;

		case SEND_COOR_ROBOT:
			SECRETARY_send_robot_position(error_scan, robotPosition.x, robotPosition.y, robotPosition.teta);
			state=END;
			break;

		case END:
			state = INIT;
			return;
			break;
	}
}

void SCAN_CORNER_calculate(){
	Uint8 nb_points_in_zone=0;
	Uint8 i, j;
	Sint16 moyx, moyy;
	Uint16 varx, vary;
	Uint8  indexMinVarx=0;
	Uint8  indexMinVary=0;
	Uint16 minVarx=65535;
	Uint16 minVary=65535;
	Sint32 moyxfinal = 0;
	Sint32 moyyfinal = 0;
	Sint32 num = 0;
	Sint32 den = 0;
	double a_south = 0;
	double a_color = 0;
	Sint32 b_south = 0;
	Sint32 b_color = 0;
	bool_e vertical = FALSE;
	Uint16 distance = 0;
	bool_e flag_correction;
	Uint16 pseudovariance = 0;
	Sint16 ymoy_corner_south = 0;
	Sint16 xmoy_corner_south = 0;
	Sint16 ymoy_corner_color = 0;
	Sint16 xmoy_corner_color = 0;
	double cos_produit_scalaire = 0;
	Sint16 offset_x = 0;
	Sint16 offset_y = 0;
	Sint16 offset_angle = 0;
	Sint16 new_x = 0;
	Sint16 new_y = 0;
	Sint16 new_angle = 0;


	it_printf("erhkdfgbhkdg\n");
	do{
		moyx=0;
		moyy=0;
		varx=65535;
		vary=65535;
		vertical = FALSE;
		distance = 0;
		flag_correction = TRUE;
		pseudovariance = 0;
		for(i=0; i<NB_POINT_MAX; i++){
			if((ourCornerSouth[i].x != 0) || (ourCornerSouth[i].y != 0)){		// Si c'est un point valide
				nb_points_in_zone++;
				moyx += (Sint32) (ourCornerSouth[i].x);
				moyy += (Sint32) (ourCornerSouth[i].y);
			}
					//it_printf("x=%d\ty=%d\txmoy=%d\tymoy=%d\n",zone[i].scan_point.x,zone[i].scan_point.y,xmoy,ymoy);
		}
		if(nb_points_in_zone > NB_POINTS_MINI_CORNER){

			moyxfinal = (moyx<<10) / nb_points_in_zone;
			moyx = moyxfinal>>10;
			moyyfinal = (moyy<<10) / nb_points_in_zone;
			moyy = moyyfinal>>10;
			for(i=0; i<NB_POINT_MAX; i++){
				if((ourCornerSouth[i].x != 0) || (ourCornerSouth[i].y != 0)){		// Si c'est un point valide
					num += (ourCornerSouth[i].x - moyx)*(ourCornerSouth[i].y - moyy);
					den += (ourCornerSouth[i].y - moyy)*(ourCornerSouth[i].y - moyy);
				}
			}
			if(den != 0){
				a_south = (double)num / (double)den;
				if(a_south!=0){
					a_south= -1/a_south;
				}else{
					vertical=TRUE;
				}
				b_south = moyy - a_south * moyx;
				for(i=0;i<NB_POINT_MAX;i++){
					if((ourCornerSouth[i].x != 0) || (ourCornerSouth[i].y != 0)){		// Si c'est un point valide
		//				it_printf("x=%d\ty=%d\n",zone[i].scan_point.x,zone[i].scan_point.y);

						if((a_south!=0)&&(vertical==FALSE)){
							distance = absolute(ourCornerSouth[i].x-(ourCornerSouth[i].y - b_south) / a_south);
							//it_printf("xre:%d\txth:%d\n",zone[i].scan_point.x,(Sint16)((zone[i].scan_point.y-b)/a));
						}else{
							distance = absolute(ourCornerSouth[i].x-moyx);
						}
						if(distance > DISTANCE_MAX){
							//it_printf("dist:\t%d\n",distance);

							ourCornerSouth[i].x = 0;
							ourCornerSouth[i].y = 0;
							//it_printf("cest la %d\n",distance);
							flag_correction = FALSE;
						}
						pseudovariance += distance;
					}
				}
			}else{
				error_scan = TRUE;
				return;
			}
		}else{
			error_scan = TRUE;
			return;
		}
	}while(flag_correction == FALSE);
	pseudovariance = pseudovariance / nb_points_in_zone;
	if (pseudovariance < CORNER_SCAN_PSEUDO_VARIANCE_SEUIL){
		xmoy_corner_south = moyxfinal;
		//display(xmoy);
		//display(ymoy);
		ymoy_corner_south = moyyfinal;
		//it_printf("xmoy=%d\tymoy=%d\n",xmoy,ymoy);
//		return TRUE;
	}else{
		for(i=0; i<NB_POINT_MAX; i++){
			//it_printf("%d\t%d\n", zone[i].scan_point.x, zone[i].scan_point.y);
		}
		error_scan = TRUE;
		return;
	}
	do{
		moyx = 0;
		moyy = 0;
		moyxfinal = 0;
		moyyfinal = 0;
		num = 0;
		den = 0;
		pseudovariance = 0;

		flag_correction = TRUE;
		vertical=FALSE;
		nb_points_in_zone = 0;
		for(i=0; i<NB_POINT_MAX; i++){
			if((ourCornerColor[i].x != 0) || (ourCornerColor[i].y != 0)){		// Si c'est un point valide
				nb_points_in_zone++;
				moyx += (Sint32) ourCornerColor[i].x;
				moyy += (Sint32) ourCornerColor[i].y;
			}
			//it_printf("x=%d\ty=%d\txmoy=%d\tymoy=%d\n",zone[i].scan_point.x,zone[i].scan_point.y,xmoy,ymoy);
		}
		if(nb_points_in_zone > NB_POINTS_MINI_CORNER)
		{
			moyxfinal = (moyx<<10) / nb_points_in_zone;
			moyx = moyyfinal>>10;
			moyyfinal = (moyy<<10) / nb_points_in_zone;
			moyy = moyyfinal>>10;
			for(i=0; i<NB_POINT_MAX; i++){
				if((ourCornerColor[i].x != 0) || (ourCornerColor[i].y != 0)){		// Si c'est un point valide
					num += (ourCornerColor[i].x - moyx)*(ourCornerColor[i].y - moyy);
					den += (ourCornerColor[i].x - moyy)*(ourCornerColor[i].x - moyx);
				}
			}
			if(den != 0){
				a_color = (double)num / (double)den;
				b_south = moyy - a_color * moyx;

										//it_printf("a=%ld.%3ld\tb=%d\tnbpoints=%d\n", (Uint32)(a), (((Uint32)(a*1000))%1000), b, nb_points_in_zone);
				for(i=0; i<NB_POINT_MAX; i++){

					if((ourCornerColor[i].x != 0) || (ourCornerColor[i].y != 0)){		// Si c'est un point valide
									//			it_printf("x=%d\ty=%d\n",zone[i].scan_point.x,zone[i].scan_point.y);
						distance = absolute(ourCornerColor[i].y - (a_color * ourCornerColor[i].x + b_south));
												//it_printf("yre:%d\tyth:%d\n",zone[i].scan_point.y,(Sint16)(a * zone[i].scan_point.x + b));
						if(distance > DISTANCE_MAX){
												//it_printf("dist:\t%d\n",distance);
							ourCornerColor[i].x = 0;
							ourCornerColor[i].y = 0;
													//printf("cest ici\n");
							flag_correction = FALSE;
						}
						pseudovariance += distance;
					}
				}
			}else{
				error_scan = TRUE;
				return;
			}
							//it_printf("\nflag_correc:%d\n\n",flag_correction);
		}else{
			error_scan = TRUE;
			return;
		}
	}while(flag_correction == FALSE);
	pseudovariance = pseudovariance / nb_points_in_zone;

		//it_printf("pseudov%d\n", pseudovariance);
		//it_printf("a=%ld.%3ld\tb=%d\tnbpoints=%d", (Uint32)(a), (((Uint32)(a*1000))%1000), b, nb_points_in_zone);

	if (pseudovariance < CORNER_SCAN_PSEUDO_VARIANCE_SEUIL){
		xmoy_corner_color = moyxfinal;
			//display(xmoy);
			//display(ymoy);
		ymoy_corner_color = moyyfinal;
			//it_printf("xmoy=%d\tymoy=%d\n",xmoy,ymoy);
				//return TRUE;
	}else{
		for(i=0; i<NB_POINT_MAX; i++){
				//it_printf("%d\t%d\n", zone[i].scan_point.x, zone[i].scan_point.y);
		}
		error_scan = TRUE;
		return;
	}
	if(vertical){
		cos_produit_scalaire=(a_color * a_color)/(a_color * a_color + 1);
	}else{
		cos_produit_scalaire=((a_color * a_south + 1) * (a_color * a_south + 1))/((a_color * a_color + 1) * (a_south * a_south + 1));
	}
	if((COS_CARRE_SCALAIRE<cos_produit_scalaire)||(-COS_CARRE_SCALAIRE>cos_produit_scalaire)){
		error_scan = TRUE;
		return; // l'angle les droites n'est pas droit
	}
	if(info_scan.color == YELLOW){
		if(vertical){
			offset_x = b_south - 2000;
			offset_y = (Sint32)(a_color * offset_x + b_color);
			offset_y -= 3000;
		}else{
			num = b_color - b_south;
			den = a_south - a_color;
			offset_x = ((double)(num))/((double)(den));
			offset_angle = atan4096(a_color);
			offset_y = (Sint32)(a_color * offset_x + b_color);
			offset_x += 3000*sin4096(offset_angle)-2000*cos4096(offset_angle);
			offset_y += -3000*cos4096(offset_angle)-2000*sin4096(offset_angle);
	#warning 'A TESTER'
		}
	}else{
		if(vertical){
			offset_x = b_south - 2000;
			offset_y = (Sint32)(a_south * offset_x + b_south);
		}else{
			num = b_color - b_south;
			den = a_south - a_color;
			offset_x = ((double)(num))/((double)(den));
			offset_y = (Sint32)(a_south * offset_x + b_south);
			offset_angle = atan4096(a_color);
			offset_x -= 2000*cos4096(offset_angle);
			offset_y -= 2000*sin4096(offset_angle);
#warning 'A TESTER'
		}
	}
	display(offset_x);
	display(offset_y);
	display(offset_angle);

	if((offset_angle != 0)||(offset_x != 0)||(offset_y != 0)){
		Sint16 cosinus, sinus;
		COS_SIN_4096_get(offset_angle, &cosinus, &sinus);
		//notre nouvelle abscisse
		new_x = (cosinus * (global.position.x-offset_x) - sinus * (global.position.y-offset_x))/4096;
		//la nouvelle ordonn�e
		new_y = (sinus * (global.position.x-offset_y) + cosinus * (global.position.y-offset_y))/4096;
		//et enfin le petit dernier : l'angle
		new_angle = global.position.teta - offset_angle;
	}
	run_calcul = FALSE;
}


static void scanOnePoint(){
	GEOMETRY_point_t pos;
	pos = SCAN_get_pos_mesure();
	display(pos.x);
	display(pos.y);
	display(nbPointCornerSouth);
	display(nbPointCornerColor);

	if(nbPointCornerSouth < NB_POINT_MAX){
		if((pos.x < (2000 + MARGIN)) && (pos.x > (2000 - MARGIN)) && (pos.y > MARGIN) && (pos.y < (1100 - MARGIN)) && (info_scan.color == BLUE)){
			ourCornerSouth[nbPointCornerSouth] = pos;
			display(ourCornerSouth[nbPointCornerSouth].x);
			display(ourCornerSouth[nbPointCornerSouth].y);
			display(nbPointCornerSouth);
			nbPointCornerSouth++;
		}else if((pos.x < (2000 + MARGIN)) && (pos.x > (2000 - MARGIN)) && (pos.y < (3000 - MARGIN)) && (pos.y < (1900 + MARGIN)) && (info_scan.color == YELLOW)){
			it_printf("south!\n");
			ourCornerSouth[nbPointCornerSouth] = pos;
			display(ourCornerSouth[nbPointCornerSouth].x);
			display(ourCornerSouth[nbPointCornerSouth].y);
			display(nbPointCornerSouth);
			nbPointCornerSouth++;
		}
	}else{
		debug_printf("D�passement de capacit� du tableau de sauvegarde des points de scan_rotation\n");
	}
	if(nbPointCornerColor < NB_POINT_MAX){
		if((pos.x < (2000 - MARGIN)) && (pos.x > (1500 + MARGIN)) && (pos.y < MARGIN) && (pos.y > (-MARGIN)) && (info_scan.color == BLUE)){
			ourCornerColor[nbPointCornerColor] = pos;
			display(ourCornerColor[nbPointCornerColor].x);
			display(ourCornerColor[nbPointCornerColor].y);
			display(nbPointCornerColor);
			nbPointCornerColor++;
		}else if((pos.x < (2000 - MARGIN)) && (pos.x > (1500 + MARGIN)) && (pos.y < (3000 + MARGIN)) && (pos.y > (3000 - MARGIN)) && (info_scan.color == YELLOW)){
			it_printf("color!\n");
			ourCornerColor[nbPointCornerColor] = pos;
			display(ourCornerColor[nbPointCornerColor].x);
			display(ourCornerColor[nbPointCornerColor].y);
			display(nbPointCornerColor);
			nbPointCornerColor++;
		}
	}else{
		debug_printf("D�passement de capacit� du tableau de sauvegarde des points de scan_rotation\n");
	}
}


//---------------------------------------------------------------------------------------------------------------
//											Traitement messages CAN
//---------------------------------------------------------------------------------------------------------------

void SCAN_CORNER_canMsg(CAN_msg_t *msg){
	if(msg->data.strat_ask_corner_scan.startScan){
		receve_msg_can = MSG_CAN_LAUNCH_SCAN;
		end_scan = FALSE;
		it_printf("pourquoi ?\n");

		info_scan.is_right_sensor = msg->data.strat_ask_corner_scan.isRightSensor;

	}else{
		receve_msg_can = NO_MSG_CAN;
		end_scan = TRUE;
		it_printf("tu passes l� ?\n");
	}
	info_scan.color = msg->data.strat_ask_corner_scan.color;
}