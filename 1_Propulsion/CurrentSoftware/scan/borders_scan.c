#include "borders_scan.h"
#include "scan.h"
#include "../QS/QS_objects.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_maths.h"
#include "../QS/QS_types.h"
#include "../it.h"

#define NB_DATA_POINTS  50
#define BORDER_SCAN_PSEUDO_VARIANCE_SEUIL        10
#define NB_POINTS_MINI  25
#define DISTANCE_MAX    20

typedef struct{
	GEOMETRY_point_t tab[NB_DATA_POINTS];
	float a;
	float b;
	Sint16 xmoy;
	Sint16 ymoy;
	Uint8 nb_points;
	Sint32 num;
	Sint32 den;
}debug_struct_s;

#define NB_RUN_DEBUG			24
static volatile debug_struct_s tabDebug[NB_RUN_DEBUG];
static volatile Uint16 indexDebug = 0;

static volatile bool_e scan_try_proposition = FALSE;

static volatile scan_point_time_t blue_north[NB_DATA_POINTS];
static volatile Uint8 blue_north_index;
static volatile bool_e blue_north_enable;

static volatile scan_point_time_t middle_north[NB_DATA_POINTS];
static volatile Uint8 middle_north_index;
static volatile bool_e middle_north_enable;

static volatile scan_point_time_t yellow_north[NB_DATA_POINTS];
static volatile Uint8 yellow_north_index;
static volatile bool_e yellow_north_enable;

static volatile scan_point_time_t blue_start[NB_DATA_POINTS];
static volatile Uint8 blue_start_index;
static volatile bool_e blue_start_enable;

static volatile scan_point_time_t blue_corner[NB_DATA_POINTS];
static volatile Uint8 blue_corner_index;
static volatile bool_e blue_corner_enable;

static volatile scan_point_time_t yellow_start[NB_DATA_POINTS];
static volatile Uint8 yellow_start_index;
static volatile bool_e yellow_start_enable;

static volatile scan_point_time_t yellow_corner[NB_DATA_POINTS];
static volatile Uint8 yellow_corner_index;
static volatile bool_e yellow_corner_enable;

static volatile scan_point_time_t blue_south[NB_DATA_POINTS];
static volatile Uint8 blue_south_index;
static volatile bool_e blue_south_enable;

static volatile scan_point_time_t yellow_south[NB_DATA_POINTS];
static volatile Uint8 yellow_south_index;
static volatile bool_e yellow_south_enable;

volatile bool_e BORDERS_SCAN_zone_blue_north_enable = FALSE;
volatile bool_e BORDERS_SCAN_zone_middle_north_enable = FALSE;
volatile bool_e BORDERS_SCAN_zone_yellow_north_enable = FALSE;
volatile bool_e BORDERS_SCAN_zone_blue_start_enable = FALSE;
volatile bool_e BORDERS_SCAN_zone_blue_corner_enable = FALSE;
volatile bool_e BORDERS_SCAN_zone_yellow_start_enable = FALSE;
volatile bool_e BORDERS_SCAN_zone_yellow_corner_enable = FALSE;
volatile bool_e BORDERS_SCAN_zone_blue_south_enable = FALSE;
volatile bool_e BORDERS_SCAN_zone_yellow_south_enable = FALSE;

volatile Sint16 xmoy_blue_north = 0;
volatile Sint16 ymoy_blue_north = 0;
volatile Uint16 date_blue_north = 0;
volatile Sint16 xmoy_middle_north = 0;
volatile Sint16 ymoy_middle_north = 0;
volatile Uint16 date_middle_north = 0;
volatile Sint16 xmoy_yellow_north = 0;
volatile Sint16 ymoy_yellow_north = 0;
volatile Uint16 date_yellow_north = 0;
volatile Sint16 xmoy_blue_start = 0;
volatile Sint16 ymoy_blue_start = 0;
volatile Uint16 date_blue_start = 0;
volatile Sint16 xmoy_blue_corner = 0;
volatile Sint16 ymoy_blue_corner = 0;
volatile Uint16 date_blue_corner = 0;
volatile Sint16 xmoy_yellow_start = 0;
volatile Sint16 ymoy_yellow_start = 0;
volatile Uint16 date_yellow_start = 0;
volatile Sint16 xmoy_yellow_corner = 0;
volatile Sint16 ymoy_yellow_corner = 0;
volatile Uint16 date_yellow_corner = 0;
volatile Sint16 xmoy_blue_south = 0;
volatile Sint16 ymoy_blue_south = 0;
volatile Uint16 date_blue_south = 0;
volatile Sint16 xmoy_yellow_south = 0;
volatile Sint16 ymoy_yellow_south = 0;
volatile Uint16 date_yellow_south = 0;


typedef enum{
	BORDER_SCAN_MODE_DEN_X = 0,
	BORDER_SCAN_MODE_DEN_Y,
}border_scan_mode_den_e;


static void addPointZone(volatile scan_point_time_t zone[], volatile Uint8 *zone_index, volatile bool_e *zone_enable, GEOMETRY_point_t pos_mesure);
bool_e calculeZone(volatile scan_point_time_t zone[], volatile Uint8 *zone_index, volatile bool_e zone_enable, Sint16 * xmoy_border, Sint16 * ymoy_border, Uint16 *date_point, border_scan_mode_den_e mode_den);


scan_zone_e BORDERS_SCAN_treatment(GEOMETRY_point_t pos_mesure){

	if ((pos_mesure.x<20)&&(pos_mesure.y<1050)&&(pos_mesure.y>50)){
		addPointZone(blue_north, &blue_north_index, &blue_north_enable, pos_mesure);
		//calculeZone(middle_north, &middle_north_index, middle_north_enable, &angle_blue_north, &xmoy_blue_north, BORDER_SCAN_MODE_DEN_X);
		return BLUE_NORTH;
	}
	else if ((pos_mesure.x<20)&&(pos_mesure.y<1750)&&(pos_mesure.y>1250)){
		addPointZone(middle_north, &middle_north_index, &middle_north_enable, pos_mesure);
		//calculeZone(middle_north, &middle_north_index, middle_north_enable, &angle_middle_north, &xmoy_middle_north, BORDER_SCAN_MODE_DEN_X);
		return MIDDLE_NORTH;
	}
	else if ((pos_mesure.x<20)&&(pos_mesure.y>1950)&&(pos_mesure.y<2950)){
		addPointZone(yellow_north, &yellow_north_index, &yellow_north_enable, pos_mesure);
		//calculeZone(yellow_north, &yellow_north_index, yellow_north_enable, &angle_yellow_north, &xmoy_yellow_north, BORDER_SCAN_MODE_DEN_X);
		return YELLOW_NORTH;
	}
	else if ((pos_mesure.y<20)&&(pos_mesure.x<580)&&(pos_mesure.x>50)){
		addPointZone(blue_start, &blue_start_index, &blue_start_enable, pos_mesure);
		//calculeZone(blue_start, &blue_start_index, blue_start_enable, &angle_blue_start, &ymoy_blue_start, BORDER_SCAN_MODE_DEN_Y);
		return BLUE_START;
	}
	else if ((pos_mesure.y<20)&&(pos_mesure.x<1950)&&(pos_mesure.x>1450)){
		addPointZone(blue_corner, &blue_corner_index, &blue_corner_enable, pos_mesure);
		//calculeZone(blue_corner, &blue_corner_index, blue_corner_enable, &angle_blue_corner, &ymoy_blue_corner, BORDER_SCAN_MODE_DEN_Y);
		return BLUE_CORNER;
	}
	else if ((pos_mesure.y>2980)&&(pos_mesure.x<580)&&(pos_mesure.x>50)){
		addPointZone(yellow_start, &yellow_start_index, &yellow_start_enable, pos_mesure);
		//calculeZone(yellow_start, &yellow_start_index, yellow_start_enable, &angle_yellow_start, &ymoy_yellow_start, BORDER_SCAN_MODE_DEN_Y);
		return YELLOW_START;
	}
	else if ((pos_mesure.y>2980)&&(pos_mesure.x<1950)&&(pos_mesure.x>1450)){
		addPointZone(yellow_corner, &yellow_corner_index, &yellow_corner_enable, pos_mesure);
		//calculeZone(yellow_corner, &yellow_corner_index, yellow_corner_enable, &angle_yellow_corner, &ymoy_yellow_corner, BORDER_SCAN_MODE_DEN_Y);
		return YELLOW_CORNER;
	}
	else if ((pos_mesure.x>1980)&&(pos_mesure.y<1200)&&(pos_mesure.y>50)){
		addPointZone(blue_south, &blue_south_index, &blue_south_enable, pos_mesure);
		//calculeZone(blue_south, &blue_south_index, blue_south_enable, &angle_blue_south, &xmoy_blue_south, BORDER_SCAN_MODE_DEN_X);
		return BLUE_SOUTH;
	}
	else if ((pos_mesure.x>1980)&&(pos_mesure.y<2950)&&(pos_mesure.y>1800)){
		addPointZone(yellow_south, &yellow_south_index, &yellow_south_enable, pos_mesure);
		//calculeZone(yellow_south, &yellow_south_index, yellow_south_enable, &angle_yellow_south, &xmoy_yellow_south, BORDER_SCAN_MODE_DEN_X);
		return YELLOW_SOUTH;
	}
	else{
		return OTHER_ZONE;
	}
}

static void addPointZone(volatile scan_point_time_t zone[], volatile Uint8 *zone_index, volatile bool_e *zone_enable, GEOMETRY_point_t pos_mesure){
	zone[(*zone_index)].scan_point = pos_mesure;
	zone[(*zone_index)].scan_time = IT_get_counter();

	//it_printf("%d\t%d\n",pos_mesure.x,pos_mesure.y);
	(*zone_index)++;
	if((*zone_index) == NB_DATA_POINTS){
		*zone_index=0;
		*zone_enable=TRUE;
	}
}


bool_e calculeZonePublic(scan_zone_e zone){

	if (zone == BLUE_NORTH){
		return calculeZone(blue_north, &blue_north_index, blue_north_enable, &xmoy_blue_north, &ymoy_blue_north, &date_blue_north, BORDER_SCAN_MODE_DEN_Y);
	}
	else if (zone == MIDDLE_NORTH){
		return calculeZone(middle_north, &middle_north_index, middle_north_enable, &xmoy_middle_north, &ymoy_middle_north, &date_middle_north, BORDER_SCAN_MODE_DEN_Y);
	}
	else if (zone == YELLOW_NORTH){
		return calculeZone(yellow_north, &yellow_north_index, yellow_north_enable, &xmoy_yellow_north, &ymoy_yellow_north, &date_yellow_north, BORDER_SCAN_MODE_DEN_Y);
	}
	else if (zone == BLUE_START){
		return calculeZone(blue_start, &blue_start_index, blue_start_enable, &xmoy_blue_start, &ymoy_blue_start, &date_blue_start, BORDER_SCAN_MODE_DEN_X);
	}
	else if (zone == BLUE_CORNER){
		return calculeZone(blue_corner, &blue_corner_index, blue_corner_enable, &xmoy_blue_corner, &ymoy_blue_corner, &date_blue_corner, BORDER_SCAN_MODE_DEN_X);
	}
	else if (zone == YELLOW_START){
		return calculeZone(yellow_start, &yellow_start_index, yellow_start_enable, &xmoy_yellow_start, &ymoy_yellow_start, &date_yellow_start, BORDER_SCAN_MODE_DEN_X);
	}
	else if (zone == YELLOW_CORNER){
		return calculeZone(yellow_corner, &yellow_corner_index, yellow_corner_enable, &xmoy_yellow_corner, &ymoy_yellow_corner, &date_yellow_corner, BORDER_SCAN_MODE_DEN_X);
	}
	else if (zone == BLUE_SOUTH){
		return calculeZone(blue_south, &blue_south_index, blue_south_enable, &xmoy_blue_south, &ymoy_blue_south, &date_blue_south, BORDER_SCAN_MODE_DEN_Y);
	}
	else if (zone == YELLOW_SOUTH){
		return calculeZone(yellow_south, &yellow_south_index, yellow_south_enable, &xmoy_yellow_south, &ymoy_yellow_south, &date_yellow_south, BORDER_SCAN_MODE_DEN_Y);
	}
	else{
		return FALSE;
	}
	return FALSE;
}


bool_e calculeZone(volatile scan_point_time_t zone[], volatile Uint8 *zone_index, volatile bool_e zone_enable, Sint16 *xmoy_border, Sint16 *ymoy_border, Uint16 *date_point, border_scan_mode_den_e mode_den){
	Sint32 xmoy=0, ymoy=0, num=0, den=0;
	Uint8 i;
	double a=0;
	Sint32 b=0;
	Uint16 pseudovariance=0;
	Uint8 nb_points_in_zone=0;
	Uint16 distance=0;
	bool_e flag_correction=TRUE;
	bool_e vertical=FALSE;

	bool_e firstLoop = FALSE;

	if(indexDebug < NB_RUN_DEBUG){
		firstLoop = TRUE;
	}

	if(firstLoop){
		for(i=0; i<NB_DATA_POINTS; i++){
			tabDebug[indexDebug].tab[i].x = zone[i].scan_point.x;
			tabDebug[indexDebug].tab[i].y = zone[i].scan_point.y;
		}
	}



	do{

		xmoy = 0;
		ymoy = 0;
		num = 0;
		den = 0;
		pseudovariance = 0;

		flag_correction = TRUE;
		vertical=FALSE;

		nb_points_in_zone = 0;
		for(i=0; i<NB_DATA_POINTS; i++){
			if(zone[i].scan_time>3000){
				zone[i].scan_point.x = 0;
				zone[i].scan_point.y = 0;
			}
			if((zone[i].scan_point.x != 0) || (zone[i].scan_point.y != 0)){		// Si c'est un point valide
				nb_points_in_zone++;
				xmoy += (Sint32) zone[i].scan_point.x;
				ymoy += (Sint32) zone[i].scan_point.y;
			}
			//it_printf("x=%d\ty=%d\txmoy=%d\tymoy=%d\n",zone[i].scan_point.x,zone[i].scan_point.y,xmoy,ymoy);
		}

		if(nb_points_in_zone < NB_POINTS_MINI)
			return FALSE;

		xmoy = xmoy / nb_points_in_zone;
		ymoy = ymoy / nb_points_in_zone;

		//it_printf("\nfalbala xmoy=%d\tymoy=%d\n\n",xmoy,ymoy);

		for(i=0; i<NB_DATA_POINTS; i++){
			if((zone[i].scan_point.x != 0) || (zone[i].scan_point.y != 0)){		// Si c'est un point valide
				num += (zone[i].scan_point.x - xmoy)*(zone[i].scan_point.y - ymoy);
				if(mode_den == BORDER_SCAN_MODE_DEN_X)
					den += (zone[i].scan_point.x - xmoy)*(zone[i].scan_point.x - xmoy);
				else
				    den += (zone[i].scan_point.y - ymoy)*(zone[i].scan_point.y - ymoy);
			}
		}
		if(den != 0){
			a = (double)num / (double)den;
			if(mode_den != BORDER_SCAN_MODE_DEN_X){
				if(a!=0){
					a= -1/a;
				}else{
					vertical=TRUE;
				}
			}

			b = ymoy - a * xmoy;

			if(firstLoop){
				tabDebug[indexDebug].a = a;
				tabDebug[indexDebug].b = b;
				tabDebug[indexDebug].den = den;
				tabDebug[indexDebug].nb_points = nb_points_in_zone;
				tabDebug[indexDebug].num = num;
				tabDebug[indexDebug].xmoy = xmoy;
				tabDebug[indexDebug].ymoy = ymoy;
				it_printf("numero de run : %d\n",indexDebug);

				indexDebug++;
				firstLoop = FALSE;
			}

			//if(mode_den == BORDER_SCAN_MODE_DEN_X)
			//*moy_border = xmoy;
			//else
			//    *moy_border = ymoy;
			if(mode_den == BORDER_SCAN_MODE_DEN_X){
				//it_printf("a=%ld.%3ld\tb=%d\tnbpoints=%d\n", (Uint32)(a), (((Uint32)(a*1000))%1000), b, nb_points_in_zone);
				for(i=0; i<NB_DATA_POINTS; i++){

					if((zone[i].scan_point.x != 0) || (zone[i].scan_point.y != 0)){		// Si c'est un point valide
			//			it_printf("x=%d\ty=%d\n",zone[i].scan_point.x,zone[i].scan_point.y);

						distance = absolute(zone[i].scan_point.y - (a * zone[i].scan_point.x + b));
						//it_printf("yre:%d\tyth:%d\n",zone[i].scan_point.y,(Sint16)(a * zone[i].scan_point.x + b));
						if(distance > DISTANCE_MAX){
							//it_printf("dist:\t%d\n",distance);
							zone[i].scan_point.x = 0;
							zone[i].scan_point.y = 0;
							//printf("cest ici\n");
							flag_correction = FALSE;
						}
						pseudovariance += distance;
					}
				}

			}else{
				//it_printf("a=%ld.%3ld\tb=%d\tnbpoints=%d\n", (Uint32)(a), (((Uint32)(a*1000))%1000), b, nb_points_in_zone);

                for(i=0;i<NB_DATA_POINTS;i++){
					if((zone[i].scan_point.x != 0) || (zone[i].scan_point.y != 0)){		// Si c'est un point valide
		//				it_printf("x=%d\ty=%d\n",zone[i].scan_point.x,zone[i].scan_point.y);

						if((a!=0)&&(vertical==FALSE)){
	                    	distance=absolute(zone[i].scan_point.x-(zone[i].scan_point.y-b)/a);
							//it_printf("xre:%d\txth:%d\n",zone[i].scan_point.x,(Sint16)((zone[i].scan_point.y-b)/a));
	                    }else{
	                    	distance=absolute(zone[i].scan_point.x-xmoy);
	                    }

	                    if(distance > DISTANCE_MAX){
							//it_printf("dist:\t%d\n",distance);

							zone[i].scan_point.x = 0;
							zone[i].scan_point.y = 0;
							//it_printf("cest la %d\n",distance);
							flag_correction = FALSE;
						}
						pseudovariance += distance;
					}
                }
			}
		}else{
			return FALSE;
		}
		//it_printf("\nflag_correc:%d\n\n",flag_correction);
	}while(flag_correction == FALSE);

	pseudovariance = pseudovariance / nb_points_in_zone;

	//it_printf("pseudov%d\n", pseudovariance);
	//it_printf("a=%ld.%3ld\tb=%d\tnbpoints=%d", (Uint32)(a), (((Uint32)(a*1000))%1000), b, nb_points_in_zone);

	if (pseudovariance < BORDER_SCAN_PSEUDO_VARIANCE_SEUIL){
		*xmoy_border = xmoy;
		*ymoy_border = ymoy;
		*date_point=IT_get_counter();
		it_printf("xmoy=%d\tymoy=%d\n",xmoy,ymoy);
		scan_try_proposition = TRUE;
		return TRUE;
	}else{
		for(i=0; i<NB_DATA_POINTS; i++){
			//it_printf("%d\t%d\n", zone[i].scan_point.x, zone[i].scan_point.y);
		}
		return FALSE;
	}
}

void afficheDebug(){
	Uint16 i, j;

	for(i=0; i<NB_RUN_DEBUG; i++){
		debug_printf("Run %d\n", i);
		for(j=0; j<NB_DATA_POINTS; j++){

			printf("x=\t%d\ty=\t%d\n",tabDebug[i].tab[j].x,tabDebug[i].tab[j].y);
			//display(tabDebug[i].tab[j].x);
			//display(tabDebug[i].tab[j].y);
		}
		display_float(tabDebug[i].a);
		display(tabDebug[i].b);
		display(tabDebug[i].num);
		display(tabDebug[i].den);
		display(tabDebug[i].nb_points);
		display(tabDebug[i].xmoy);
		display(tabDebug[i].ymoy);


	}
}

void BORDERS_SCAN_process_main(){
	Uint8 counter_north_border = 0;
	Uint16 date_north_border = 0;
	Uint16 date_blue_border = 0;
	Uint16 date_yellow_border = 0;
	Uint16 date_south_border = 0;
	double a_north = 0;
	double a_south = 0;
	double a_blue = 0;
	double a_yellow = 0;
	Uint32 b_north = 0;
	Uint32 b_south = 0;
	Uint32 b_blue = 0;
	Uint32 b_yellow = 0;
	bool_e yellow_north_zone_enable = FALSE;
	bool_e middle_north_zone_enable = FALSE;
	bool_e blue_north_zone_enable = FALSE;
	bool_e north_zone_enable = FALSE;
	bool_e south_zone_enable = FALSE;
	bool_e blue_zone_enable = FALSE;
	bool_e yellow_zone_enable = FALSE;
	Sint32 cos_produit_scalaire = 0;
	Sint32 new_intersection_x = 0;
	Sint32 new_intersection_y = 0;
	Sint32 new_correction_angle = 0;

	Sint32 new_point_x = 0;
	Sint32 new_point_y = 0;
	Sint32 new_angle = 0;

	if(scan_try_proposition){
		if(date_blue_north > (IT_get_counter()-3000)){
			counter_north_border++;
		}
		if(date_middle_north > (IT_get_counter()-3000)){
			counter_north_border++;
		}
		if(date_yellow_north > (IT_get_counter()-3000)){
			counter_north_border++;
		}
		if(counter_north_border >= 2){
			north_zone_enable = TRUE;
			if((date_yellow_north < date_middle_north)&&(date_yellow_north < date_blue_north)){
				middle_north_zone_enable = TRUE;
				blue_north_zone_enable = TRUE;
				if(date_middle_north < date_blue_border){
					date_north_border = date_middle_north;
				}else{
					date_north_border = date_blue_north;
				}
			}else if((date_yellow_north > date_middle_north)&&(date_yellow_north > date_blue_north)){
				if(date_middle_north > date_blue_border){
					middle_north_zone_enable = TRUE;
					yellow_north_zone_enable = TRUE;
					date_north_border = date_middle_north;
				}else{
					blue_north_zone_enable = TRUE;
					yellow_north_zone_enable = TRUE;
					date_north_border = date_blue_north;
				}
			}else{
				date_north_border = date_yellow_north;
				if(date_middle_north < date_blue_border){
					blue_north_zone_enable = TRUE;
					yellow_north_zone_enable = TRUE;
				}else{
					middle_north_zone_enable = TRUE;
					yellow_north_zone_enable = TRUE;
				}
			}
		}
		if((date_blue_start > (IT_get_counter()-3000))||(date_blue_corner > (IT_get_counter()-3000))){
			blue_zone_enable = TRUE;
			if(date_blue_start > date_blue_corner){
				date_blue_border = date_blue_corner;

			}else{
				date_blue_border = date_blue_start;
			}
		}
		if((date_yellow_start > (IT_get_counter()-3000))||(date_yellow_corner > (IT_get_counter()-3000))){
			yellow_zone_enable = TRUE;
			if(date_yellow_start > date_yellow_corner){
				date_yellow_border = date_yellow_corner;
			}else{
				date_yellow_border = date_yellow_start;
			}
		}
		if((date_blue_south > (IT_get_counter()-3000))&&(date_yellow_south > (IT_get_counter()-3000))){
			south_zone_enable = TRUE;
			if(date_blue_south > date_yellow_south){
				date_south_border = date_yellow_south;
			}else{
				date_south_border = date_blue_south;
			}
		}

		if((date_south_border < date_north_border)&&(north_zone_enable)){ //c'est parti on se fait chier pour le nord (c'était un mauvaise idée cette troisième bordure
			if(yellow_north_zone_enable && blue_north_zone_enable){
				a_north = (double)(ymoy_yellow_north - ymoy_blue_north)/(double)(xmoy_yellow_north - xmoy_blue_north);
				b_north = ymoy_yellow_north - a_yellow * xmoy_yellow_north;
			}else if(yellow_north_zone_enable && middle_north_zone_enable){
				a_north = (double)(ymoy_yellow_north - ymoy_middle_north)/(double)(xmoy_yellow_north - xmoy_middle_north);
				b_north = ymoy_yellow_north - a_yellow * xmoy_yellow_north;
			}else{
				a_north = (double)(ymoy_middle_north - ymoy_blue_north)/(double)(xmoy_middle_north - xmoy_blue_north);
				b_north = ymoy_middle_north - a_yellow * xmoy_middle_north;
			}
			if((date_blue_border < date_yellow_border)&&(yellow_zone_enable)){
				a_yellow = (double)(ymoy_yellow_start - ymoy_yellow_corner)/(double)(xmoy_yellow_start - xmoy_yellow_corner);
				b_yellow = ymoy_yellow_start - a_yellow * xmoy_yellow_start;
				//on accepte une erreur d'angle entre les droites de 1° cos²(89°)=   en PI4096
				cos_produit_scalaire=((a_yellow * a_north + 1) * (a_yellow * a_north + 1))/((a_yellow * a_yellow + 1) * (a_north * a_north + 1));
				if((5110>cos_produit_scalaire)||(-5110<cos_produit_scalaire)){
					return;// l'angle les droites n'est pas droit
				}
				new_intersection_x = (double)(b_yellow - b_north)/(double)(a_north - a_yellow);
				new_intersection_y = a_north * new_intersection_x + b_north ;
				new_correction_angle = atan4096(a_yellow);

			}else if (blue_zone_enable){
				a_blue = (double)(ymoy_blue_start - ymoy_blue_corner)/(double)(xmoy_blue_start - xmoy_blue_corner);
				b_blue = ymoy_blue_start - a_blue * xmoy_blue_start;
				cos_produit_scalaire=((a_blue * a_north + 1) * (a_blue * a_north + 1))/((a_blue * a_blue + 1) * (a_north * a_north + 1));
				if((5110>cos_produit_scalaire)||(-5110<cos_produit_scalaire)){
					return; // l'angle les droites n'est pas droit
				}
				new_intersection_x = (double)(b_blue - b_north)/(double)(a_north - a_blue);
				new_intersection_y = a_north * new_intersection_x + b_north ;
				new_correction_angle = atan4096(a_blue);

			}
		}else if (south_zone_enable){
			a_south = (double)(ymoy_blue_south-ymoy_yellow_south)/(double)(xmoy_blue_south-xmoy_yellow_south);
			b_south = ymoy_yellow_south - a_yellow * xmoy_yellow_south;

			if((date_blue_border < date_yellow_border)&&(yellow_zone_enable)){
				a_yellow = (double)(ymoy_yellow_start-ymoy_yellow_corner)/(double)(xmoy_yellow_start-xmoy_yellow_corner);
				b_yellow = ymoy_yellow_start - a_yellow * xmoy_yellow_start;
				cos_produit_scalaire=((a_yellow * a_south + 1) * (a_yellow * a_south + 1))/((a_yellow * a_yellow + 1) * (a_south * a_south + 1));
				if((5110>cos_produit_scalaire)||(-5110<cos_produit_scalaire)){
					return; // l'angle les droites n'est pas droit
				}
				new_intersection_x = (double)(b_yellow - b_south)/(double)(a_south - a_yellow);
				new_intersection_y = a_south * new_intersection_x + b_south;
				new_correction_angle = atan4096(a_yellow);

			}else if(blue_zone_enable){
				a_blue = (double)(ymoy_blue_start-ymoy_blue_corner)/(double)(xmoy_blue_start-xmoy_blue_corner);
				b_blue = ymoy_blue_start - a_blue * xmoy_blue_start;
				cos_produit_scalaire=((a_blue * a_south + 1) * (a_blue * a_south + 1))/((a_blue * a_blue + 1) * (a_south * a_south + 1));
				if((5110>cos_produit_scalaire)||(-5110<cos_produit_scalaire)){
					return; // l'angle les droites n'est pas droit
				}
				new_intersection_x = (double)(b_blue - b_south)/(double)(a_south - a_blue);
				new_intersection_y = a_south * new_intersection_x + b_south;
				new_correction_angle = atan4096(a_blue);

			}
		}//eh ben je peux rien faire pour toi
		if((new_correction_angle != 0)||(new_intersection_x != 0)||(new_intersection_y != 0)){
			Sint16 cosinus, sinus;
			COS_SIN_4096_get(new_correction_angle, &cosinus, &sinus);
#warning 'le offset ne doit vraiment pas être mis avant la rotation'
			new_point_x = cosinus * global.position.x - sinus * global.position.y + new_intersection_x;
			new_point_y = sinus * global.position.x + cosinus * global.position.y + new_intersection_y;
			new_angle = global.position.teta;
		}
	}
}
