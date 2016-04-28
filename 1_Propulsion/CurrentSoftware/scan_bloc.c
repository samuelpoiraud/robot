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
#define conversion_capteur_LEFT(x) ((Sint32)(550)*(x)/1000 - 69)
#define conversion_capteur_RIGHT(x) ((Sint32)(551)*(x)/1000 - 63)


//------------------------------------------------------------------------------------ Define

#define NB_POINT_MAX		250					//Max : 255
#define X_SENSOR_RIGHT		10
#define Y_SENSOR_RIGHT		10
#define X_SENSOR_LEFT		10
#define Y_SENSOR_LEFT		10



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
			state = WAIT;
			break;

		case WAIT :
			switch(receve_msg_can){
				case MSG_CAN_LAUNCH_SCAN:
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
			SCAN_BLOC_calculate();
			if(!run_calcul)
				state = SEND_COOR_BLOC;
			break;

		case SEND_COOR_BLOC:
			SECRETARY_send_bloc_position(error_scan, blocPosition.x, blocPosition.y);
			break;

		case END:
			state = INIT;
			return;
			break;
	}
}

void SCAN_BLOC_calculate(){
	// TODO : Traitement des points

	run_calcul = FALSE;
}

static void scanOnePoint(){
	mesure_en_cours.robot = global.position;
	if(info_scan.color == BOT_COLOR){	// Magenta
		mesure_en_cours.dist = conversion_capteur_RIGHT(ADC_getValue(ADC_SCAN_BLOC_SENSOR_RIGHT));
	}else{
		mesure_en_cours.dist = conversion_capteur_LEFT(ADC_getValue(ADC_SCAN_BLOC_SENSOR_LEFT));
	}

	// Déterminer la position du point scanné sur le terrain
	if(nbPointBloc < NB_POINT_MAX){
		GEOMETRY_point_t tmp;
		Sint16 cos, sin;
		COS_SIN_4096_get(mesure_en_cours.robot.teta, &cos, &sin);
		Sint32 cos32 = cos, sin32 = sin;
		if(info_scan.color == BOT_COLOR){	// Magenta
			tmp.x = mesure_en_cours.robot.x + mesure_en_cours.dist * cos32/4096 + X_SENSOR_RIGHT;
			tmp.y = mesure_en_cours.robot.y - mesure_en_cours.dist * sin32/4096 - Y_SENSOR_RIGHT;
		}else{
			tmp.x = mesure_en_cours.robot.x + mesure_en_cours.dist * cos32/4096 + X_SENSOR_RIGHT;
			tmp.y = mesure_en_cours.robot.y - mesure_en_cours.dist * sin32/4096 - Y_SENSOR_RIGHT;
		}
		ourBloc[nbPointBloc] = tmp;
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
	}else{
		receve_msg_can = NO_MSG_CAN;
		end_scan = TRUE;
	}
	info_scan.color = msg->data.strat_ask_bloc_scan.color;
}
