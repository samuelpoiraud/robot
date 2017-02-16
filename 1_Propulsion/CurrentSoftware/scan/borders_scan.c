#include "borders_scan.h"
#include "scan.h"
#include "../QS/QS_objects.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_maths.h"
#include "../QS/QS_types.h"

#define NB_DATA_POINTS  50
#define OK_SEUIL        10
#define NB_POINTS_MINI  25
#define DISTANCE_MAX    20


static volatile GEOMETRY_point_t blue_north[NB_DATA_POINTS];
static volatile Uint8 blue_north_index;
static volatile bool_e blue_north_enable;
static volatile GEOMETRY_point_t middle_north[NB_DATA_POINTS];
static volatile Uint8 middle_north_index;
static volatile bool_e middle_north_enable;
static volatile GEOMETRY_point_t yellow_north[NB_DATA_POINTS];
static volatile Uint8 yellow_north_index;
static volatile bool_e yellow_north_enable;
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


static void addPointZone(volatile GEOMETRY_point_t zone[], volatile Uint8 *zone_index, volatile bool_e *zone_enable, GEOMETRY_point_t pos_mesure);
bool_e calculeZone(volatile GEOMETRY_point_t zone[], volatile Uint8 *zone_index, volatile bool_e zone_enable, Sint16 * angle_zone, Sint16 * moy_border, border_scan_mode_den_e mode_den);


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
}

static void addPointZone(volatile GEOMETRY_point_t zone[], volatile Uint8 *zone_index, volatile bool_e *zone_enable, GEOMETRY_point_t pos_mesure){
    zone[(*zone_index)]=pos_mesure;
    //it_printf("%d\t%d\n",pos_mesure.x,pos_mesure.y);
	(*zone_index)++;
	if((*zone_index) == NB_DATA_POINTS){
		*zone_index=0;
        *zone_enable=TRUE;
	}
}


bool_e calculeZonePublic(scan_zone_e zone){

    Sint16 xmoy_blue_north=0;
    Sint16 ymoy_blue_north=0;
    Sint16 xmoy_middle_north=0;
    Sint16 ymoy_middle_north=0;
    Sint16 xmoy_yellow_north=0;
    Sint16 ymoy_yellow_north=0;
    Sint16 xmoy_blue_start=0;
    Sint16 ymoy_blue_start=0;
    Sint16 xmoy_blue_corner=0;
    Sint16 ymoy_blue_corner=0;
    Sint16 xmoy_yellow_start=0;
    Sint16 ymoy_yellow_start=0;
    Sint16 xmoy_yellow_corner=0;
    Sint16 ymoy_yellow_corner=0;
    Sint16 xmoy_blue_south=0;
    Sint16 ymoy_blue_south=0;
    Sint16 xmoy_yellow_south=0;
    Sint16 ymoy_yellow_south=0;



    if (zone == BLUE_NORTH){
        return calculeZone(blue_north, &blue_north_index, blue_north_enable, &xmoy_blue_north, &ymoy_blue_north, BORDER_SCAN_MODE_DEN_X);
    }
    else if (zone == MIDDLE_NORTH){
        return calculeZone(middle_north, &middle_north_index, middle_north_enable, &xmoy_middle_north, &ymoy_middle_north, BORDER_SCAN_MODE_DEN_X);
    }
    else if (zone == YELLOW_NORTH){
        return calculeZone(yellow_north, &yellow_north_index, yellow_north_enable, &xmoy_yellow_north, &ymoy_yellow_north, BORDER_SCAN_MODE_DEN_X);
    }
    else if (zone == BLUE_START){
        return calculeZone(blue_start, &blue_start_index, blue_start_enable, &xmoy_blue_start, &ymoy_blue_start, BORDER_SCAN_MODE_DEN_Y);
    }
    else if (zone == BLUE_CORNER){
        return calculeZone(blue_corner, &blue_corner_index, blue_corner_enable, &xmoy_blue_corner, &ymoy_blue_corner, BORDER_SCAN_MODE_DEN_Y);
    }
    else if (zone == YELLOW_START){
        return calculeZone(yellow_start, &yellow_start_index, yellow_start_enable, &xmoy_yellow_start, &ymoy_yellow_start, BORDER_SCAN_MODE_DEN_Y);
    }
    else if (zone == YELLOW_CORNER){
        return calculeZone(yellow_corner, &yellow_corner_index, yellow_corner_enable, &xmoy_yellow_corner, &ymoy_yellow_corner, BORDER_SCAN_MODE_DEN_Y);
    }
    else if (zone == BLUE_SOUTH){
        return calculeZone(blue_south, &blue_south_index, blue_south_enable, &xmoy_blue_south, &ymoy_blue_south, BORDER_SCAN_MODE_DEN_X);
    }
    else if (zone == YELLOW_SOUTH){
        return calculeZone(yellow_south, &yellow_south_index, yellow_south_enable, &xmoy_yellow_south, &ymoy_yellow_south, BORDER_SCAN_MODE_DEN_X);
    }
}


bool_e calculeZone(volatile GEOMETRY_point_t zone[], volatile Uint8 *zone_index, volatile bool_e zone_enable, Sint16 *xmoy_border, Sint16 *ymoy_border, border_scan_mode_den_e mode_den){
    Sint32 xmoy=0, ymoy=0, num=0, den=0;
    Uint8 i,j;
    double a=0;
    Sint16 b=0;
    Uint16 pseudovariance=0;
    Uint8 nb_points_in_zone=0;
    Uint16 distance=0;
    bool_e flag_correction=TRUE;

    if (zone_enable)
        nb_points_in_zone = NB_DATA_POINTS;
    else
        nb_points_in_zone = (*zone_index);

    do{
        j=0;
        xmoy=0;
        ymoy=0;
        num=0;
        den=0;
        pseudovariance=0;

        flag_correction=TRUE;
        if(nb_points_in_zone < NB_POINTS_MINI)
            return FALSE;

        for(i=0;i<NB_DATA_POINTS;i++){
            if((zone[j].x==0)||(zone[j].y==0)){
                j++;
                if((*zone_index)>j){
                    *zone_index--;
                }
            }
            zone[i].x=zone[j].x;
            zone[i].y=zone[j].y;

            xmoy+=(Sint32) zone[i].x;
            ymoy+=(Sint32) zone[i].y;
            j++;
                //it_printf("x=%d\ty=%d\txmoy=%d\tymoy=%d\n",zone[i].x,zone[i].y,xmoy,ymoy);
        }
        xmoy=xmoy/nb_points_in_zone;
        ymoy=ymoy/nb_points_in_zone;

        //it_printf("xmoy=%d\tymoy=%d\n",xmoy,ymoy);

        for(i=0;i<nb_points_in_zone;i++){
            num+=(zone[i].x-xmoy)*(zone[i].y-ymoy);
            //if(mode_den == BORDER_SCAN_MODE_DEN_X)
            den+=(zone[i].x-xmoy)*(zone[i].x-xmoy);
                //else
                //    den+=(zone[i].y-ymoy)*(zone[i].y-ymoy);
        }
        if(den!=0){
            a=(double)num/(double)den;
            b=ymoy-a*xmoy;
            //if(mode_den == BORDER_SCAN_MODE_DEN_X)
                //*moy_border = xmoy;
            //else
            //    *moy_border = ymoy;
            if(mode_den != BORDER_SCAN_MODE_DEN_X){
                for(i=0;i<nb_points_in_zone;i++){
                    distance = absolute(zone[i].y-(a*zone[i].x+b));
                    if(distance > DISTANCE_MAX){
                        zone[i].x = 0;
                        zone[i].y = 0;
                        nb_points_in_zone--;
                        flag_correction=FALSE;
                    }
                    pseudovariance+= distance;
                }
            }else{
                for(i=0;i<nb_points_in_zone;i++){
                    if(a!=0)
                        pseudovariance+=absolute(zone[i].x-(b-zone[i].y)/a);
                    else
                        pseudovariance+=absolute(zone[i].x-xmoy);
                }
            }
        }else{
            if(mode_den != BORDER_SCAN_MODE_DEN_X){
               return FALSE;
            }else{
                for(i=0;i<nb_points_in_zone;i++){
                        pseudovariance+=absolute(zone[i].x-xmoy);
                }
            }
        }
    }while(flag_correction==FALSE);
    pseudovariance=pseudovariance/nb_points_in_zone;

    it_printf("pseudov%d\n",pseudovariance);
    it_printf("a=%f\tb=%d\tnbpoints=%d",a,b,nb_points_in_zone);

    if (pseudovariance<OK_SEUIL){
        *xmoy_border = xmoy;
        *ymoy_border = ymoy;
        //it_printf("xmoy=%d\tymoy=%d\n",xmoy,ymoy);
        return TRUE;
    }else{
        for(i=0;i<NB_DATA_POINTS;i++){
            it_printf("%d\t%d\n",zone[i].x,zone[i].y);
        }
        //it_printf("a=%f\tb=%d\tnbpoints=%d",a,b,nb_points_in_zone);
        return FALSE;
    }
}
