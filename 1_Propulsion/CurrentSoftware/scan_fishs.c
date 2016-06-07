
#include "scan_fishs.h"
#include "QS/QS_maths.h"
#include "odometry.h"
#include "QS/QS_can.h"
#include "QS/QS_adc.h"
#include "QS/QS_outputlog.h"

#define DISTANCE_CENTRE_ROBOT 20
#define MARGIN_BAC 5
#define NB_FISHS 4
#define NB_POINTS 1000
#define CONVERSION_FISHS(x)	((Sint32)(4845*(x)+221900)/10000)

static volatile GEOMETRY_point_t tab_points[NB_POINTS];
static Uint16 tab_index;
static GEOMETRY_point_t pos_fishs[NB_FISHS];
static Uint8 nb_fishs_detected;
static bool_e flag_start, flag_finish;
static color_e color;


static void SCAN_FISHS_treatment();
static void SCAN_FISHS_send_infos();

void SCAN_FISHS_init(){
	color = ODOMETRY_get_color();
	flag_start = FALSE;
	flag_finish = FALSE;
	tab_index = 0;
	nb_fishs_detected = 0;
}


void SCAN_FISHS_process_main(){
	typedef enum{INIT, IDLE, SCAN, TREATMENT, SEND_INFOS, DONE} SCAN_FISHS_e;
	static SCAN_FISHS_e state = INIT;
	static SCAN_FISHS_e last_state = DONE;

	if(last_state != state){
		debug_printf("state = %d\n", state);
		last_state = state;
	}

	switch(state){
		 case INIT:
			 SCAN_FISHS_init();
			 state = IDLE;
			 break;

		case IDLE:
			 if(flag_start)
				 state = SCAN;
			 break;

		case SCAN:
			 if(flag_finish)
				 state = TREATMENT;
			 break;

		case TREATMENT:
			 SCAN_FISHS_treatment();
			 state = SEND_INFOS;
			 break;

		case SEND_INFOS:
			 SCAN_FISHS_send_infos();
			 state = DONE;
			 break;

		case DONE:
			 state = INIT;
			 break;
	 }
}

void SCAN_FISHS_process_it(){
	if(flag_start){
		Uint16 distance_scan = CONVERSION_FISHS(ADC_getValue(ADC_SENSOR_FISHS)) + DISTANCE_CENTRE_ROBOT;
		GEOMETRY_point_t point;
		point.x = global.position.x + distance_scan*cos4096(global.position.teta);
		point.y = global.position.y + distance_scan*sin4096(global.position.teta);
		debug_printf("%4d ; %4d",point.x, point.y);
		if(tab_index < NB_POINTS){
			if(color == MAGENTA && is_in_square(2027 + MARGIN_BAC , 2227 - MARGIN_BAC, 503 + MARGIN_BAC, 903 - MARGIN_BAC, point)){
				tab_points[tab_index].x = point.x;
				tab_points[tab_index].y = point.y;
				tab_index++;
				debug_printf(" X\n");
			}else if(color == GREEN && is_in_square(2027 + MARGIN_BAC , 2227 - MARGIN_BAC, 2097 + MARGIN_BAC, 2497 - MARGIN_BAC, point)){
				tab_points[tab_index].x = point.x;
				tab_points[tab_index].y = point.y;
				tab_index++;
				debug_printf("\n");
			}
		}
	}
}

static void SCAN_FISHS_treatment(){
	Uint16 i = 0;
	Uint16 moyX, moyY;
	Uint16 index_begin_detection = 0;

	debug_printf("tab_index=%d", tab_index);
	//PS: Si on ne rentre pas dans le for, alors aucun poisson n'a été détecté
	for(i = 0; i < tab_index && nb_fishs_detected < NB_FISHS; i++){
		debug_printf("i=%3d  %4d %4d\n", i, tab_points[i].x, tab_points[i].y);
		if(i == 0){
			moyX = tab_points[i].x;
			moyY = tab_points[i].y;
		}else if(GEOMETRY_squared_distance(tab_points[i],tab_points[index_begin_detection]) < 20*20){ //Ce point correspond au même poisson qu'avant
			moyX += tab_points[i].x;
			moyY += tab_points[i].y;
		}else{ //un nouveau poisson a été détecté
			pos_fishs[nb_fishs_detected].x = moyX/(i - index_begin_detection);
			pos_fishs[nb_fishs_detected].y = moyY/(i - index_begin_detection);
			debug_printf("fishs_detected in x=%d  y=%d\n", pos_fishs[nb_fishs_detected].x, pos_fishs[nb_fishs_detected].y);
			nb_fishs_detected++;
			index_begin_detection = i;
			moyX = tab_points[i].x;
			moyY = tab_points[i].y;
		}
	}
	debug_printf("Traitement fini\n");
}

static void SCAN_FISHS_send_infos(){
	CAN_msg_t msg;
	msg.sid = PROP_RESULT_FISHS_SCAN;
	msg.size = SIZE_PROP_RESULT_FISHS_SCAN;
	Uint8 i;
	for(i = 0; i < nb_fishs_detected; i++){
		msg.data.prop_result_fishs_scan.y_pos[i] = pos_fishs[i].y;
	}
	for(i = nb_fishs_detected; i < NB_FISHS; i++){
		msg.data.prop_result_fishs_scan.y_pos[i] = 0;
	}
	CAN_send(&msg);
}

void SCAN_FISHS_set_start(bool_e value){
	debug_printf("start affected\n");
	color = ODOMETRY_get_color();
	debug_printf("I am color=%d\n", color);
	flag_start = value;
}


void SCAN_FISHS_set_finish(bool_e value){
	debug_printf("finish affected\n");
	flag_finish = value;
}
