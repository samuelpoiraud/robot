#include "scan_bloc.h"
#include "QS/QS_adc.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_maths.h"
#include "QS/QS_measure.h"
#include "supervisor.h"
#include "secretary.h"
#include "corrector.h"
#include "roadmap.h"
#include "_Propulsion_config.h"
#include "odometry.h"
#include "config/config_use.h"
#include "QS/QS_watchdog.h"

//------------------------------------------------------------------------------------ Macro

#define square(x) ((float)(x)*(float)(x))
#define CONVERSION_LASER_LEFT(x)	((Sint32)(-264*(x)+353500)/1000)
#define CONVERSION_LASER_RIGHT(x)	((Sint32)(-264*(x)+354400)/1000)


//------------------------------------------------------------------------------------ Define

#define NB_POINT_MAX		250					//Max : 255
#define X_SENSOR_RIGHT		10
#define Y_SENSOR_RIGHT		10
#define X_SENSOR_LEFT		10
#define Y_SENSOR_LEFT		10

#define DISTANCE_SCAN_CENTER		146
#define DISTANCE_SCAN_CENTER_Y		60



//------------------------------------------------------------------------------------ Définition des structures et énumerations

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

static GEOMETRY_point_t ourBloc[NB_POINT_MAX];
static Uint8 nbPointBloc = 0;
static bool_e run_calcul,end_scan;
static bool_e error_scan;
static scan_result_t mesure_en_cours;

static receve_msg_can_e receve_msg_can;
static proporties_scan info_scan;
static watchdog_id_t idWatchdog;
static GEOMETRY_point_t blocPosition;




//------------------------------------------------------------------------------------ Prototype des fonctions local

static void scanOnePoint();

//------------------------------------------------------------------------------------ Fonctions




void SCAN_BLOC_init(void)
{
	ADC_init();				//Initialisation des convertisseurs analogiques numeriques
	run_calcul = FALSE;
}

//---------------------------------------------------------------------------------------------------------------
//												MAE Scan Bloc
//---------------------------------------------------------------------------------------------------------------

void SCAN_BLOC_process_main(){
	if(run_calcul)
		SCAN_BLOC_calculate();
}

void SCAN_BLOC_process_it(){
	typedef enum{
		INIT=0,
		WAIT,
		PROCESS_SCAN,
		SCAN_CALCUL,
		WAIT_CALCULATE,
		SEND_COOR_BLOC,
		END
	}state_e;

	static state_e state = INIT;

	switch(state){

		case INIT :
			receve_msg_can = NO_MSG_CAN;
			//printf("\n\aziaoe\n\n");
			state = WAIT;
			break;

		case WAIT :
			switch(receve_msg_can){
				case MSG_CAN_LAUNCH_SCAN:
					//printf("\n\nhhhhhhhhhhh\n\n");
					idWatchdog = WATCHDOG_create(10, scanOnePoint, TRUE);
					state = PROCESS_SCAN;
					break;
				case NO_MSG_CAN :
				default :
					break;
			}
			break;

		case PROCESS_SCAN:
			if(end_scan == TRUE){
				WATCHDOG_stop(idWatchdog);
				state = SCAN_CALCUL;
			}
			break;

		case SCAN_CALCUL:
			run_calcul = TRUE;
			state = WAIT_CALCULATE;
			break;

		case WAIT_CALCULATE:
			//printf("\n\nsrfgunfgjnkfgdfg\n\n");
			//SCAN_BLOC_calculate();
			if(!run_calcul)
				state = SEND_COOR_BLOC;
			break;

		case SEND_COOR_BLOC:
			SECRETARY_send_bloc_position(error_scan, blocPosition.x, blocPosition.y);
			state=END;
			break;

		case END:
			state = INIT;
			return;
			break;
	}
}

void SCAN_BLOC_calculate(){
	// TODO : Traitement des points
	Uint8 i, j;
	Sint16 moyx[NB_POINT_MAX], moyy[NB_POINT_MAX];
	Uint16 varx[NB_POINT_MAX], vary[NB_POINT_MAX];
	Uint8 indexMinVarx=0;
	Uint8 indexMinVary=0;
	Uint16 minVarx=65535;
	Uint16 minVary=65535;
	//printf("erhkdfgbhkdg\n");

	for(i=0;i<NB_POINT_MAX;i++){
		moyx[i]=0;
		moyy[i]=0;
		varx[i]=65535;
		vary[i]=65535;
		if(i<NB_POINT_MAX-10){
			for(j=i;j<i+10;j++){
				moyx[i]+=ourBloc[j].x;
				moyy[i]+=ourBloc[j].y;
			}
			moyx[i]/=10;
			moyy[i]/=10;
			if((moyx[i]<174)&&(moyx[i]>58)){
				varx[i]=0;
					for(j=i;j<i+10;j++){
						varx[i]+=(moyx[i]-ourBloc[j].x)*(moyx[i]-ourBloc[j].x);
					}
			}
			//printf("\n%d\n",moyx[i]);
			//printf("\n%d\n",info_scan.is_right_sensor);
			if(((moyy[i]<996)&&(moyy[i]>880)&&(info_scan.is_right_sensor))||((moyy[i]>2004)&&(moyy[i]<2120)&&(!(info_scan.is_right_sensor)))){
				vary[i]=0;
				for(j=i;j<i+10;j++){
					vary[i]+=(moyy[i]-ourBloc[j].y)*(moyy[i]-ourBloc[j].y);
				}
			}
		}
//		debug_printf("[%d;%d]\n", ourBloc[i].x, ourBloc[i].y);
	}
	for(i=0;i<NB_POINT_MAX;i++){
		//printf("\n\n%d<%d\n",varx[i],minVarx);
		if(varx[i]<minVarx){
			minVarx=varx[i];
			indexMinVarx=i;
	//		printf("\n\n%d\t%d\n\n",minVarx,indexMinVarx);
		}
		if(vary[i]<minVary){
			minVary=vary[i];
			indexMinVary=i;
		//	printf("\n\n%d\t%d\n\n",minVary,indexMinVary);
		}
	}
	for(i=0;i<NB_POINT_MAX;i++){
		//printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n",ourBloc[i].x,ourBloc[i].y,moyx[i],moyy[i],varx[i],vary[i],info_scan.is_right_sensor);
	}
	if((varx[indexMinVarx]<500)&&(vary[indexMinVary]<500)){
		blocPosition.x=moyx[indexMinVarx];
		blocPosition.y=moyy[indexMinVary];
	}else{
		error_scan=TRUE;
	}
	//printf("\n\n%d\t%d\n\n",indexMinVary,indexMinVarx);

	printf("\n\n%d\t%d\n\n",blocPosition.x,blocPosition.y);
	run_calcul = FALSE;
}

static void scanOnePoint(){
	mesure_en_cours.robot = global.position;
	if(info_scan.is_right_sensor){	// Magenta
		mesure_en_cours.dist = CONVERSION_LASER_RIGHT(ADC_getValue(ADC_SENSOR_LASER_RIGHT));
	}else{
		mesure_en_cours.dist = CONVERSION_LASER_LEFT(ADC_getValue(ADC_SENSOR_LASER_LEFT));
	}

	// Déterminer la position du point scanné sur le terrain
	if(nbPointBloc < NB_POINT_MAX){
		GEOMETRY_point_t tmp;
		Sint16 cos, sin;
		COS_SIN_4096_get(mesure_en_cours.robot.teta, &cos, &sin);
		Sint32 cos32 = cos, sin32 = sin;
		//printf("%d\n",mesure_en_cours.dist);
		if(info_scan.is_right_sensor){
			tmp.x = mesure_en_cours.robot.x + (mesure_en_cours.dist + DISTANCE_SCAN_CENTER) * sin32/4096 + DISTANCE_SCAN_CENTER_Y * cos32/4096;
			tmp.y = mesure_en_cours.robot.y - (mesure_en_cours.dist + DISTANCE_SCAN_CENTER) * cos32/4096 + DISTANCE_SCAN_CENTER_Y * sin32/4096;
		}else{
			tmp.x = mesure_en_cours.robot.x - (mesure_en_cours.dist + DISTANCE_SCAN_CENTER) * sin32/4096 + DISTANCE_SCAN_CENTER_Y * cos32/4096;
			tmp.y = mesure_en_cours.robot.y + (mesure_en_cours.dist + DISTANCE_SCAN_CENTER) * cos32/4096 + DISTANCE_SCAN_CENTER_Y * sin32/4096;
		}
		//if(info_scan.color == BOT_COLOR){	// Magenta
			//tmp.x = mesure_en_cours.robot.x + mesure_en_cours.dist * cos32/4096 + DISTANCE_SCAN_CENTER;
			//tmp.y = mesure_en_cours.robot.y - mesure_en_cours.dist * sin32/4096 - DISTANCE_SCAN_CENTER_Y;
		//}else{
			//tmp.x = mesure_en_cours.robot.x + mesure_en_cours.dist * cos32/4096 + DISTANCE_SCAN_CENTER;
			//tmp.y = mesure_en_cours.robot.y - mesure_en_cours.dist * sin32/4096 - DISTANCE_SCAN_CENTER_Y;
		//}
		ourBloc[nbPointBloc] = tmp;
		nbPointBloc++;
	}else{
		debug_printf("Dépassement de capacité du tableau de sauvegarde des points de scan\n");
	}
}


//---------------------------------------------------------------------------------------------------------------
//											Traitement messages CAN
//---------------------------------------------------------------------------------------------------------------

void SCAN_BLOC_canMsg(CAN_msg_t *msg){
	if(msg->data.strat_ask_bloc_scan.startScan){
		receve_msg_can = MSG_CAN_LAUNCH_SCAN;
		end_scan = FALSE;
		info_scan.is_right_sensor = msg->data.strat_ask_bloc_scan.isRightSensor;

	}else{
		receve_msg_can = NO_MSG_CAN;
		end_scan = TRUE;
	}
	info_scan.color = msg->data.strat_ask_bloc_scan.color;
}
