#include "borders_scan.h"
#include "scan.h"
#include "../QS/QS_objects.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_maths.h"
#include "../QS/QS_types.h"

#define NB_DATA_POINTS      30
#define OK_SEUIL 10
#define NB_POINTS_MINI 5

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


GEOMETRY_point_t* BORDERS_SCAN_treatment(GEOMETRY_point_t pos_mesure){


    if ((pos_mesure.x<20)&&(pos_mesure.y<1050)&&(pos_mesure.y>50)){
        addPointZone(blue_north, &blue_north_index, &blue_north_enable, pos_mesure);
        //calculeZone(middle_north, &middle_north_index, middle_north_enable, &angle_blue_north, &xmoy_blue_north, BORDER_SCAN_MODE_DEN_X);
        return blue_north;
    }
    else if ((pos_mesure.x<20)&&(pos_mesure.y<1750)&&(pos_mesure.y>1250)){
        addPointZone(middle_north, &middle_north_index, &middle_north_enable, pos_mesure);
        //calculeZone(middle_north, &middle_north_index, middle_north_enable, &angle_middle_north, &xmoy_middle_north, BORDER_SCAN_MODE_DEN_X);
        return middle_north;
    }
    else if ((pos_mesure.x<20)&&(pos_mesure.y>1950)&&(pos_mesure.y<2950)){
        addPointZone(yellow_north, &yellow_north_index, &yellow_north_enable, pos_mesure);
        //calculeZone(yellow_north, &yellow_north_index, yellow_north_enable, &angle_yellow_north, &xmoy_yellow_north, BORDER_SCAN_MODE_DEN_X);
        return yellow_north;
    }
    else if ((pos_mesure.y<20)&&(pos_mesure.x<580)&&(pos_mesure.x>50)){
        addPointZone(blue_start, &blue_start_index, &blue_start_enable, pos_mesure);
        //calculeZone(blue_start, &blue_start_index, blue_start_enable, &angle_blue_start, &ymoy_blue_start, BORDER_SCAN_MODE_DEN_Y);
        return blue_start;
    }
    else if ((pos_mesure.y<20)&&(pos_mesure.x<1950)&&(pos_mesure.x>1450)){
        addPointZone(blue_corner, &blue_corner_index, &blue_corner_enable, pos_mesure);
        //calculeZone(blue_corner, &blue_corner_index, blue_corner_enable, &angle_blue_corner, &ymoy_blue_corner, BORDER_SCAN_MODE_DEN_Y);
        return blue_corner;
    }
    else if ((pos_mesure.y>2980)&&(pos_mesure.x<580)&&(pos_mesure.x>50)){
        addPointZone(yellow_start, &yellow_start_index, &yellow_start_enable, pos_mesure);
        //calculeZone(yellow_start, &yellow_start_index, yellow_start_enable, &angle_yellow_start, &ymoy_yellow_start, BORDER_SCAN_MODE_DEN_Y);
        return yellow_start;
    }
    else if ((pos_mesure.y>2980)&&(pos_mesure.x<1950)&&(pos_mesure.x>1450)){
        addPointZone(yellow_corner, &yellow_corner_index, &yellow_corner_enable, pos_mesure);
        //calculeZone(yellow_corner, &yellow_corner_index, yellow_corner_enable, &angle_yellow_corner, &ymoy_yellow_corner, BORDER_SCAN_MODE_DEN_Y);
        return yellow_corner;
    }
    else if ((pos_mesure.x>1980)&&(pos_mesure.y<1200)&&(pos_mesure.y>50)){
        addPointZone(blue_south, &blue_south_index, &blue_south_enable, pos_mesure);
        //calculeZone(blue_south, &blue_south_index, blue_south_enable, &angle_blue_south, &xmoy_blue_south, BORDER_SCAN_MODE_DEN_X);
        return blue_south;
    }
    else if ((pos_mesure.x>1980)&&(pos_mesure.y<2950)&&(pos_mesure.y>1800)){
        addPointZone(yellow_south, &yellow_south_index, &yellow_south_enable, pos_mesure);
        //calculeZone(yellow_south, &yellow_south_index, yellow_south_enable, &angle_yellow_south, &xmoy_yellow_south, BORDER_SCAN_MODE_DEN_X);
        return yellow_south;
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


bool_e calculeZonePublic(GEOMETRY_point_t* zone){

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



    if (zone == blue_north){
        return calculeZone(middle_north, &middle_north_index, middle_north_enable, &xmoy_blue_north, &ymoy_blue_north, BORDER_SCAN_MODE_DEN_X);
    }
    else if (zone == middle_north){
        return calculeZone(middle_north, &middle_north_index, middle_north_enable, &xmoy_middle_north, &ymoy_middle_north, BORDER_SCAN_MODE_DEN_X);
    }
    else if (zone == yellow_north){
        return calculeZone(yellow_north, &yellow_north_index, yellow_north_enable, &xmoy_yellow_north, &ymoy_yellow_north, BORDER_SCAN_MODE_DEN_X);
    }
    else if (zone == blue_start){
        return calculeZone(blue_start, &blue_start_index, blue_start_enable, &xmoy_blue_start, &ymoy_blue_start, BORDER_SCAN_MODE_DEN_Y);
    }
    else if (zone == blue_corner){
        return calculeZone(blue_corner, &blue_corner_index, blue_corner_enable, &xmoy_blue_corner, &ymoy_blue_corner, BORDER_SCAN_MODE_DEN_Y);
    }
    else if (zone == yellow_start){
        return calculeZone(yellow_start, &yellow_start_index, yellow_start_enable, &xmoy_yellow_start, &ymoy_yellow_start, BORDER_SCAN_MODE_DEN_Y);
    }
    else if (zone == yellow_corner){
        return calculeZone(yellow_corner, &yellow_corner_index, yellow_corner_enable, &xmoy_yellow_corner, &ymoy_yellow_corner, BORDER_SCAN_MODE_DEN_Y);
    }
    else if (zone == blue_south){
        return calculeZone(blue_south, &blue_south_index, blue_south_enable, &xmoy_blue_south, &ymoy_blue_south, BORDER_SCAN_MODE_DEN_X);
    }
    else if (zone == yellow_south){
        return calculeZone(yellow_south, &yellow_south_index, yellow_south_enable, &xmoy_yellow_south, &ymoy_yellow_south, BORDER_SCAN_MODE_DEN_X);
    }
}


bool_e calculeZone(volatile GEOMETRY_point_t zone[], volatile Uint8 *zone_index, volatile bool_e zone_enable, Sint16 *xmoy_border, Sint16 *ymoy_border, border_scan_mode_den_e mode_den){
    Sint64 xmoy=0, ymoy=0, num=0, den=0;
    Uint32 i;
    double a=0;
    Sint16 b=0;
    Uint16 pseudovariance=0;

    if(zone_enable){
        for(i=0;i<NB_DATA_POINTS;i++){
            xmoy+=zone[i].x;
            ymoy+=zone[i].y;
        }
        xmoy=xmoy/NB_DATA_POINTS;
        ymoy=ymoy/NB_DATA_POINTS;
        for(i=0;i<NB_DATA_POINTS;i++){
            num+=(zone[i].x-xmoy)*(zone[i].y-ymoy);
            //if(mode_den == BORDER_SCAN_MODE_DEN_X)
                den+=(zone[i].x-xmoy)*(zone[i].x-xmoy);
            //else
            //    den+=(zone[i].y-ymoy)*(zone[i].y-ymoy);
        }
        a=(double)num/(double)den;
      //  *angle_zone=atan4096(a);
        b=ymoy-a*xmoy;
        //if(mode_den == BORDER_SCAN_MODE_DEN_X)
            //*moy_border = xmoy;
        //else
        //    *moy_border = ymoy;
        if(mode_den == BORDER_SCAN_MODE_DEN_X){
            for(i=0;i<NB_DATA_POINTS;i++){
                pseudovariance+=absolute(zone[i].y-(a*zone[i].x+b));
            }
        }else{
            for(i=0;i<NB_DATA_POINTS;i++){
                if(a!=0)
                    pseudovariance+=absolute(zone[i].x-(b-zone[i].y)/a);
                else
                    pseudovariance+=absolute(zone[i].x-xmoy);
            }
        }
        pseudovariance=pseudovariance/NB_DATA_POINTS;
    }
    else{
        if(*zone_index<NB_POINTS_MINI)
            return FALSE;
        for(i=0;i<(*zone_index);i++){
            xmoy+=zone[i].x;
            ymoy+=zone[i].y;
        }
        xmoy=xmoy/(*zone_index);
        ymoy=ymoy/(*zone_index);
        for(i=0;i<(*zone_index);i++){
            num+=(zone[i].x-xmoy)*(zone[i].y-ymoy);
            //if(mode_den == BORDER_SCAN_MODE_DEN_X)
                den+=(zone[i].x-xmoy)*(zone[i].x-xmoy);
            //else
            //    den+=(zone[i].y-ymoy)*(zone[i].y-ymoy);
        }
        a = (float)num/((float)den);
      //  *angle_zone=atan4096(a);
        b=ymoy-a*xmoy;

        //if(mode_den == BORDER_SCAN_MODE_DEN_X)
           // *moy_border = xmoy;
        //else
        //    *moy_border = ymoy;
        if(mode_den == BORDER_SCAN_MODE_DEN_X){
            for(i=0;i<*zone_index;i++){
                pseudovariance+=absolute(zone[i].y-(a*zone[i].x+b));
            }
        }else{
            for(i=0;i<*zone_index;i++){
                if(a!=0)
                    pseudovariance+=absolute(zone[i].x-(b-zone[i].y)/a);
                else
                    pseudovariance+=absolute(zone[i].x-xmoy);
            }
        }
        pseudovariance=pseudovariance/(*zone_index);
    }
    if (pseudovariance<OK_SEUIL){
        *xmoy_border = xmoy;
        *ymoy_border = ymoy;
        it_printf("%d\t%d\n",xmoy,ymoy);
        return TRUE;
    }else
        return FALSE;
}

