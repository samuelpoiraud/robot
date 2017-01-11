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



void BORDERS_SCAN_treatment(scan_data_t tab[]){
    Uint32 index,i,j;
    Sint64 xmoy=0;
    Sint64 ymoy=0;
    Sint64 num=0;
    Sint64 den=0;
    double pente=0;
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
            middle_north[middle_north_index]=tab[index].pos_mesure;
            middle_north_index++;
            if(middle_north_index==NB_DATA_POINTS){
                middle_north_index=0;
                middle_north_enable=TRUE;
            }
            if((middle_north_index%NB_POINTS_ANALYSYS)==0){
                if(middle_north_enable){
                    xmoy=0;
                    ymoy=0;
                    num=0;
                    den=0;
                    for(i=0;i<NB_DATA_POINTS;i++){
                        xmoy+=middle_north[i].x;
                        ymoy+=middle_north[i].y;
                    }
                    xmoy=xmoy/NB_DATA_POINTS;
                    ymoy=ymoy/NB_DATA_POINTS;
                    for(i=0;i<NB_DATA_POINTS;i++){
                        num+=(middle_north[i].x-xmoy)*(middle_north[i].y-ymoy);
                        den+=(middle_north[i].y-ymoy)*(middle_north[i].y-ymoy); //attention on inverse pour ne pas avoir une pente dementielle
                    }
                    pente=num/den;
                    angle_middle_north=atan4096(pente);
                    xmoy_middle_north=xmoy;
                }
                else{
                    for(i=0;i<middle_north_index;i++){
                        xmoy+=middle_north[i].x;
                        ymoy+=middle_north[i].y;
                    }
                    xmoy=xmoy/middle_north_index;
                    ymoy=ymoy/middle_north_index;
                    for(i=0;i<middle_north_index;i++){
                        num+=(middle_north[i].x-xmoy)*(middle_north[i].y-ymoy);
                        den+=(middle_north[i].y-ymoy)*(middle_north[i].y-ymoy);//attention inversion
                    }
                    pente=num/den;
                    angle_middle_north=atan4096(pente);
                    xmoy_middle_north=xmoy;
                }
            }
        }
        else if ((tab[index].pos_mesure.y<20)&&(tab[index].pos_mesure.x<650)&&(tab[index].pos_mesure.x>400)){
            blue_start[blue_start_index]=tab[index].pos_mesure;
            blue_start_index++;
            if(blue_start_index==NB_DATA_POINTS){
                blue_start_index=0;
                blue_start_enable=TRUE;
            }
            if((blue_start_index%NB_POINTS_ANALYSYS)==0){
                if(blue_start_enable){
                    xmoy=0;
                    ymoy=0;
                    num=0;
                    den=0;
                    for(i=0;i<NB_DATA_POINTS;i++){
                        xmoy+=blue_start[i].x;
                        ymoy+=blue_start[i].y;
                    }
                    xmoy=xmoy/NB_DATA_POINTS;
                    ymoy=ymoy/NB_DATA_POINTS;
                    for(i=0;i<NB_DATA_POINTS;i++){
                        num+=(blue_start[i].x-xmoy)*(blue_start[i].y-ymoy);
                        den+=(blue_start[i].x-xmoy)*(blue_start[i].x-xmoy);
                    }
                    angle_blue_start=atan4096(num/den);
                    ymoy_blue_start=ymoy;
                }
                else{
                    for(i=0;i<blue_start_index;i++){
                        xmoy+=blue_start[i].x;
                        ymoy+=blue_start[i].y;
                    }
                    xmoy=xmoy/blue_start_index;
                    ymoy=ymoy/blue_start_index;
                    for(i=0;i<blue_start_index;i++){
                        num+=(blue_start[i].x-xmoy)*(blue_start[i].y-ymoy);
                        den+=(blue_start[i].x-xmoy)*(blue_start[i].x-xmoy);
                    }
                    angle_blue_start=atan4096(num/den);
                    ymoy_blue_start=ymoy;
                }
            }
        }
        else if ((tab[index].pos_mesure.y<20)&&(tab[index].pos_mesure.x<1980)&&(tab[index].pos_mesure.x>1450)){
            blue_corner[blue_corner_index]=tab[index].pos_mesure;
            blue_corner_index++;
            if(blue_corner_index==NB_DATA_POINTS){
                blue_corner_index=0;
                blue_corner_enable=TRUE;
            }
            if((blue_corner_index%NB_POINTS_ANALYSYS)==0){
                if(blue_corner_enable){
                    xmoy=0;
                    ymoy=0;
                    num=0;
                    den=0;
                    for(i=0;i<NB_DATA_POINTS;i++){
                        xmoy+=blue_corner[i].x;
                        ymoy+=blue_corner[i].y;
                    }
                    xmoy=xmoy/NB_DATA_POINTS;
                    ymoy=ymoy/NB_DATA_POINTS;
                    for(i=0;i<NB_DATA_POINTS;i++){
                        num+=(blue_corner[i].x-xmoy)*(blue_corner[i].y-ymoy);
                        den+=(blue_corner[i].x-xmoy)*(blue_corner[i].x-xmoy);
                    }
                    angle_blue_corner=atan4096(num/den);
                    ymoy_blue_corner=ymoy;
                }
                else{
                    for(i=0;i<blue_corner_index;i++){
                        xmoy+=blue_corner[i].x;
                        ymoy+=blue_corner[i].y;
                    }
                    xmoy=xmoy/blue_corner_index;
                    ymoy=ymoy/blue_corner_index;
                    for(i=0;i<blue_corner_index;i++){
                        num+=(blue_corner[i].x-xmoy)*(blue_corner[i].y-ymoy);
                        den+=(blue_corner[i].x-xmoy)*(blue_corner[i].x-xmoy);
                    }
                    angle_blue_corner=atan4096(num/den);
                    ymoy_blue_corner=ymoy;
                }
            }
        }
        else if ((tab[index].pos_mesure.y>2980)&&(tab[index].pos_mesure.x<650)&&(tab[index].pos_mesure.x>400)){
            yellow_start[yellow_start_index]=tab[index].pos_mesure;
            yellow_start_index++;
            if(yellow_start_index==NB_DATA_POINTS){
                yellow_start_index=0;
                yellow_start_enable=TRUE;
            }
            if((yellow_start_index%NB_POINTS_ANALYSYS)==0){
                if(yellow_start_enable){
                    xmoy=0;
                    ymoy=0;
                    num=0;
                    den=0;
                    for(i=0;i<NB_DATA_POINTS;i++){
                        xmoy+=yellow_start[i].x;
                        ymoy+=yellow_start[i].y;
                    }
                    xmoy=xmoy/NB_DATA_POINTS;
                    ymoy=ymoy/NB_DATA_POINTS;
                    for(i=0;i<NB_DATA_POINTS;i++){
                        num+=(yellow_start[i].x-xmoy)*(yellow_start[i].y-ymoy);
                        den+=(yellow_start[i].x-xmoy)*(yellow_start[i].x-xmoy);
                    }
                    angle_yellow_start=atan4096(num/den);
                    ymoy_yellow_start=ymoy;
                }
                else{
                    for(i=0;i<yellow_start_index;i++){
                        xmoy+=yellow_start[i].x;
                        ymoy+=yellow_start[i].y;
                    }
                    xmoy=xmoy/yellow_start_index;
                    ymoy=ymoy/yellow_start_index;
                    for(i=0;i<yellow_start_index;i++){
                        num+=(yellow_start[i].x-xmoy)*(yellow_start[i].y-ymoy);
                        den+=(yellow_start[i].x-xmoy)*(yellow_start[i].x-xmoy);
                    }
                    angle_yellow_start=atan4096(num/den);
                    ymoy_yellow_start=ymoy;
                }
            }
        }
        else if ((tab[index].pos_mesure.y>2980)&&(tab[index].pos_mesure.x<1980)&&(tab[index].pos_mesure.x>1450)){
            yellow_corner[yellow_corner_index]=tab[index].pos_mesure;
            yellow_corner_index++;
            if(yellow_corner_index==NB_DATA_POINTS){
                yellow_corner_index=0;
                yellow_corner_enable=TRUE;
            }
            if((yellow_corner_index%NB_POINTS_ANALYSYS)==0){
                if(yellow_corner_enable){
                    xmoy=0;
                    ymoy=0;
                    num=0;
                    den=0;
                    for(i=0;i<NB_DATA_POINTS;i++){
                        xmoy+=yellow_corner[i].x;
                        ymoy+=yellow_corner[i].y;
                    }
                    xmoy=xmoy/NB_DATA_POINTS;
                    ymoy=ymoy/NB_DATA_POINTS;
                    for(i=0;i<NB_DATA_POINTS;i++){
                        num+=(yellow_corner[i].x-xmoy)*(yellow_corner[i].y-ymoy);
                        den+=(yellow_corner[i].x-xmoy)*(yellow_corner[i].x-xmoy);
                    }
                    angle_yellow_corner=atan4096(num/den);
                    ymoy_yellow_corner=ymoy;
                }
                else{
                    for(i=0;i<yellow_corner_index;i++){
                        xmoy+=yellow_corner[i].x;
                        ymoy+=yellow_corner[i].y;
                    }
                    xmoy=xmoy/yellow_corner_index;
                    ymoy=ymoy/yellow_corner_index;
                    for(i=0;i<yellow_corner_index;i++){
                        num+=(yellow_corner[i].x-xmoy)*(yellow_corner[i].y-ymoy);
                        den+=(yellow_corner[i].x-xmoy)*(yellow_corner[i].x-xmoy);
                    }
                    angle_yellow_corner=atan4096(num/den);
                    ymoy_yellow_corner=ymoy;
                }
            }
        }
        else if ((tab[index].pos_mesure.x>1980)&&(tab[index].pos_mesure.y<1250)&&(tab[index].pos_mesure.y>20)){
            blue_south[blue_south_index]=tab[index].pos_mesure;
            blue_south_index++;
            if(blue_south_index==NB_DATA_POINTS){
                blue_south_index=0;
                blue_south_enable=TRUE;
            }
            if((blue_south_index%NB_POINTS_ANALYSYS)==0){
                if(blue_south_enable){
                    xmoy=0;
                    ymoy=0;
                    num=0;
                    den=0;
                    for(i=0;i<NB_DATA_POINTS;i++){
                        xmoy+=blue_south[i].x;
                        ymoy+=blue_south[i].y;
                    }
                    xmoy=xmoy/NB_DATA_POINTS;
                    ymoy=ymoy/NB_DATA_POINTS;
                    for(i=0;i<NB_DATA_POINTS;i++){
                        num+=(blue_south[i].x-xmoy)*(blue_south[i].y-ymoy);
                        den+=(blue_south[i].y-ymoy)*(blue_south[i].y-ymoy); //attention inversion
                    }
                    angle_blue_south=atan4096(num/den);
                    xmoy_blue_south=xmoy;
                }
                else{
                    for(i=0;i<blue_south_index;i++){
                        xmoy+=blue_south[i].x;
                        ymoy+=blue_south[i].y;
                    }
                    xmoy=xmoy/blue_south_index;
                    ymoy=ymoy/blue_south_index;
                    for(i=0;i<blue_south_index;i++){
                        num+=(blue_south[i].x-xmoy)*(blue_south[i].y-ymoy);
                        den+=(blue_south[i].y-ymoy)*(blue_south[i].y-ymoy);//attention inversion
                    }
                    angle_blue_south=atan4096(num/den);
                    xmoy_blue_south=xmoy;
                }
            }
        }
        else if ((tab[index].pos_mesure.x>1980)&&(tab[index].pos_mesure.y<2980)&&(tab[index].pos_mesure.y>1750)){
            yellow_south[yellow_south_index]=tab[index].pos_mesure;
            yellow_south_index=(yellow_south_index+1)%NB_DATA_POINTS;
            yellow_south_index++;
            if(yellow_south_index==NB_DATA_POINTS){
                yellow_south_index=0;
                yellow_south_enable=TRUE;
            }
            if((yellow_south_index%NB_POINTS_ANALYSYS)==0){
                if(yellow_south_enable){
                    xmoy=0;
                    ymoy=0;
                    num=0;
                    den=0;
                    for(i=0;i<NB_DATA_POINTS;i++){
                        xmoy+=yellow_south[i].x;
                        ymoy+=yellow_south[i].y;
                    }
                    xmoy=xmoy/NB_DATA_POINTS;
                    ymoy=ymoy/NB_DATA_POINTS;
                    for(i=0;i<NB_DATA_POINTS;i++){
                        num+=(yellow_south[i].x-xmoy)*(yellow_south[i].y-ymoy);
                        den+=(yellow_south[i].y-ymoy)*(yellow_south[i].y-ymoy);//attention inversion
                    }
                    angle_yellow_south=atan4096(num/den);
                    xmoy_yellow_south=xmoy;
                }
                else{
                    for(i=0;i<yellow_south_index;i++){
                        xmoy+=yellow_south[i].x;
                        ymoy+=yellow_south[i].y;
                    }
                    xmoy=xmoy/yellow_south_index;
                    ymoy=ymoy/yellow_south_index;
                    for(i=0;i<yellow_south_index;i++){
                        num+=(yellow_south[i].x-xmoy)*(yellow_south[i].y-ymoy);
                        den+=(yellow_south[i].y-ymoy)*(yellow_south[i].y-ymoy);//attention inversion
                    }
                    angle_yellow_south=atan4096(num/den);
                    xmoy_yellow_south=xmoy;
                }
            }
        }
    }
}

