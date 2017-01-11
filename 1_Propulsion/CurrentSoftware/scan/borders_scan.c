#include "borders_scan.h"
#include "scan.h"
#include "../QS/QS_objects.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_maths.h"
#include "../QS/QS_types.h"

#define NB_DATA_POINTS      30
#define NB_POINTS_ANALYSYS  10

static volatile GEOMETRY_point_t middle_north[NB_DATA_POINTS];
static volatile Uint8 middle_north_index;
static volatile bool_e middle_north_enable;
static volatile GEOMETRY_point_t blue_start[NB_DATA_POINTS];
static volatile Uint8 blue_start_index;
static volatile bool_e blue_start_enable;
static volatile GEOMETRY_point_t blue_corner[NB_DATA_POINTS];
static volatile Uint8 blue_corner_index;
static volatile bool_e blue_corner_enable;
static volatile GEOMETRY_point_t yellow_start[NB_DATA_POINTS];
static volatile Uint8 yellow_start_index;
static volatile bool_e yellow_start_enable;
static volatile GEOMETRY_point_t yellow_corner[NB_DATA_POINTS];
static volatile Uint8 yellow_corner_index;
static volatile bool_e yellow_corner_enable;
static volatile GEOMETRY_point_t blue_south[NB_DATA_POINTS];
static volatile Uint8 blue_south_index;
static volatile bool_e blue_south_enable;
static volatile GEOMETRY_point_t yellow_south[NB_DATA_POINTS];
static volatile Uint8 yellow_south_index;
static volatile bool_e yellow_south_enable;

typedef enum{
	BORDER_SCAN_MODE_DEN_X = 0,
	BORDER_SCAN_MODE_DEN_Y,
}border_scan_mode_den_e;

static void addPointZone(volatile GEOMETRY_point_t zone[], volatile Uint8 *zone_index, volatile bool_e *zone_enable, scan_data_t tab[], Uint32 indexAdd);
static void calculeZone(volatile GEOMETRY_point_t zone[], volatile Uint8 *zone_index, volatile bool_e zone_enable, Sint16 * angle_zone, Sint16 * moy_border, border_scan_mode_den_e mode_den);

void BORDERS_SCAN_treatment(scan_data_t tab[]){
    Uint32 index;
    Sint16 angle_middle_north=0;
    Sint16 angle_blue_start=0;
    Sint16 angle_blue_corner=0;
    Sint16 angle_yellow_start=0;
    Sint16 angle_yellow_corner=0;
    Sint16 angle_blue_south=0;
    Sint16 angle_yellow_south=0;
    Sint16 xmoy_middle_north=0;
    Sint16 ymoy_blue_start=0;
    Sint16 ymoy_blue_corner=0;
    Sint16 ymoy_yellow_start=0;
    Sint16 ymoy_yellow_corner=0;
    Sint16 xmoy_blue_south=0;
    Sint16 xmoy_yellow_south=0;



    for(index=0;index<NB_SCAN_DATA;index++){
        if ((tab[index].pos_mesure.x<20)&&(tab[index].pos_mesure.y<1750)&&(tab[index].pos_mesure.y>1250)){
        	addPointZone(middle_north, &middle_north_index, &middle_north_enable, tab, index);
            calculeZone(middle_north, &middle_north_index, middle_north_enable, &angle_middle_north, &xmoy_middle_north, BORDER_SCAN_MODE_DEN_X);
        }
        else if ((tab[index].pos_mesure.y<20)&&(tab[index].pos_mesure.x<650)&&(tab[index].pos_mesure.x>400)){
        	addPointZone(blue_start, &blue_start_index, &blue_start_enable, tab, index);
            calculeZone(blue_start, &blue_start_index, blue_start_enable, &angle_blue_start, &ymoy_blue_start, BORDER_SCAN_MODE_DEN_Y);
        }
        else if ((tab[index].pos_mesure.y<20)&&(tab[index].pos_mesure.x<1980)&&(tab[index].pos_mesure.x>1450)){
        	addPointZone(blue_corner, &blue_corner_index, &blue_corner_enable, tab, index);
            calculeZone(blue_corner, &blue_corner_index, blue_corner_enable, &angle_blue_corner, &ymoy_blue_corner, BORDER_SCAN_MODE_DEN_Y);
        }
        else if ((tab[index].pos_mesure.y>2980)&&(tab[index].pos_mesure.x<650)&&(tab[index].pos_mesure.x>400)){
        	addPointZone(yellow_start, &yellow_start_index, &yellow_start_enable, tab, index);
            calculeZone(yellow_start, &yellow_start_index, yellow_start_enable, &angle_yellow_start, &ymoy_yellow_start, BORDER_SCAN_MODE_DEN_Y);
        }
        else if ((tab[index].pos_mesure.y>2980)&&(tab[index].pos_mesure.x<1980)&&(tab[index].pos_mesure.x>1450)){
        	addPointZone(yellow_corner, &yellow_corner_index, &yellow_corner_enable, tab, index);
            calculeZone(yellow_corner, &yellow_corner_index, yellow_corner_enable, &angle_yellow_corner, &ymoy_yellow_corner, BORDER_SCAN_MODE_DEN_Y);
        }
        else if ((tab[index].pos_mesure.x>1980)&&(tab[index].pos_mesure.y<1250)&&(tab[index].pos_mesure.y>20)){
        	addPointZone(blue_south, &blue_south_index, &blue_south_enable, tab, index);
        	calculeZone(blue_south, &blue_south_index, blue_south_enable, &angle_blue_south, &xmoy_blue_south, BORDER_SCAN_MODE_DEN_X);
        }
        else if ((tab[index].pos_mesure.x>1980)&&(tab[index].pos_mesure.y<2980)&&(tab[index].pos_mesure.y>1750)){
        	addPointZone(yellow_south, &yellow_south_index, &yellow_south_enable, tab, index);
        	calculeZone(yellow_south, &yellow_south_index, yellow_south_enable, &angle_yellow_south, &xmoy_yellow_south, BORDER_SCAN_MODE_DEN_X);
        }
    }
}

static void addPointZone(volatile GEOMETRY_point_t zone[], volatile Uint8 *zone_index, volatile bool_e *zone_enable, scan_data_t tab[], Uint32 indexAdd){
	zone[(*zone_index)]=tab[indexAdd].pos_mesure;
	(*zone_index)++;
	if((*zone_index) == NB_DATA_POINTS){
		*zone_index=0;
		*zone_enable=TRUE;
	}
}

static void calculeZone(volatile GEOMETRY_point_t zone[], volatile Uint8 *zone_index, volatile bool_e zone_enable, Sint16 * angle_zone, Sint16 * moy_border, border_scan_mode_den_e mode_den){
    Sint64 xmoy=0, ymoy=0, num=0, den=0;
    Uint32 i;

	if(((*zone_index)%NB_POINTS_ANALYSYS)==0){
		if(zone_enable){
			for(i=0;i<NB_DATA_POINTS;i++){
				xmoy+=zone[i].x;
				ymoy+=zone[i].y;
			}
			xmoy=xmoy/NB_DATA_POINTS;
			ymoy=ymoy/NB_DATA_POINTS;
			for(i=0;i<NB_DATA_POINTS;i++){
				num+=(zone[i].x-xmoy)*(zone[i].y-ymoy);
				if(mode_den == BORDER_SCAN_MODE_DEN_X)
					den+=(zone[i].x-xmoy)*(zone[i].x-xmoy);
				else
                    den+=(zone[i].y-ymoy)*(zone[i].y-ymoy);

			}
			*angle_zone=atan4096(num/den);
		}
		else{
			for(i=0;i<(*zone_index);i++){
				xmoy+=zone[i].x;
				ymoy+=zone[i].y;
			}
			xmoy=xmoy/(*zone_index);
			ymoy=ymoy/(*zone_index);
			for(i=0;i<(*zone_index);i++){
				num+=(zone[i].x-xmoy)*(zone[i].y-ymoy);
				den+=(zone[i].x-xmoy)*(zone[i].x-xmoy);
			}
			*angle_zone=atan4096(((float)num)/((float)den));
			if(mode_den == BORDER_SCAN_MODE_DEN_X)
				*moy_border = xmoy;
			else
				*moy_border = ymoy;

		}
	}
}
