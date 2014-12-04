/* ------------------- TODO -------------------
 * Ajouter la fonction pour envoyer le message CAN
 * Mettre en place la trame du message CAN
*/

#include "scan_cup.h"
#include "QS/QS_adc.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_who_am_i.h"
#include "QS/QS_maths.h"
#include "supervisor.h"
#include "secretary.h"
#include "corrector.h"
#include "roadmap.h"
#include "_Propulsion_config.h"

#ifdef SCAN_CUP

//------------------------------------------------------------------------------------ Macro

#define square(x) ((float)(x)*(float)(x))
#define conversion_capteur(x) (((((Sint32)(597)*(x))/1024)- 78))  //A d�finir

//------------------------------------------------------------------------------------ Define

#define RADIUS_MIN_CUP		50
#define RADIUS_MAX_CUP		100
#define NB_POINT_MAX		200					//Max : 255
#define Y1					0
#define Y2					400
#define Y3					2600
#define Y4					3000
#define X1					400
#define X2					800
#define X3					1200
#define X4					1600
#define QUANTUM_MESURE		5					//Distance entre deux mesures
#define SENSOR_NAME			ADC_SENSOR_BIG


//------------------------------------------------------------------------------------ Prototype des fonctions local

static void inArea(scan_result objet);
static GEOMETRY_point_t determine_center(GEOMETRY_point_t tab[], Uint8 nb_points);


//------------------------------------------------------------------------------------ D�finition des structures et �numerations

typedef struct{
	GEOMETRY_point_t robot;
	Uint8 dist;
}scan_result;

typedef enum{
	INIT=0,
	WAIT,
	SCAN_LINEAR,
	SCAN_CALCUL,
	WAIT_CALCULATE,
	SEND_COOR_CUP
}state_e;

typedef enum{
	NO_MSG_CAN,
	MSG_CAN_SCAN_LINEAR
}receve_msg_can_e;


//------------------------------------------------------------------------------------ Variables Globales

static GEOMETRY_point_t salleH[NB_POINT_MAX];
static GEOMETRY_point_t salleB[NB_POINT_MAX];
static Uint8 nbPointH = 0;
static Uint8 nbPointB = 0;
static Uint32 old_measure = 0;
static Uint8 nb_mesure = 0;
static bool_e run_calcul,end_scan;
color_e color = ODOMETRY_get_color();

//------------------------------------------------------------------------------------ Fonctions

void SCAN_CUP_init(void){
	ADC_init();				//Initialisation des convertisseurs analogiques numeriques
	run_calcul = FALSE;
}

void SCAN_CUP_process_it(){
	static state_e state = INIT;
	scan_result mesure_en_cours;
	switch(state){

		case INIT :
			receve_msg_can = NO_MSG_CAN;
			state = WAIT;
			break;

		case WAIT :
			switch(receve_msg_can){
				case MSG_CAN_SCAN_LINEAR:
					old_measure = global.position.x;
					state = SCAN_LINEAR;
					break;
				case NO_MSG_CAN :
				default :
					break;
			}
			break;

		case SCAN_LINEAR:
			if(abs(old_measure-global.position.x) >= PAS_DE_MESURE){
				mesure_en_cours.robot.x = global.position.x;
				mesure_en_cours.robot.y = global.position.y;
				mesure_en_cours.dist = conversion_capteur(ADC_getValue(SENSOR_NAME));
				inArea(mesure_en_cours);
			}
			if(end_scan == TRUE){		//On passe dans la phase de calcul
				state = SCAN_CALCUL;
			}
			break;

		case SCAN_CALCUL:
			run_calcul = TRUE;
			state = WAIT_CALCULATE;
			break;

		case WAIT_CALCULATE:
			if(!run_calcul)
				state = SEND_COOR_CUP;
			break;

		case SEND_COOR_CUP:
			//data = (Uint8)(val >> 8);
			//data = (Uint8)(val & 0x00FF)
			SECRETARY_send_cup_position();
			break;


	}
}

static void inArea(scan_result objet){
	GEOMETRY_point_t cup;
	cup.x = objet.robot.x;				//+ constante suivant o� est plac� le capteur
	cup.y = objet.robot.y+objet.dist;	//Modifier la distance suivant emplacement capteur (+constante)
	if(color){
		if(cup.x>=X1 && cup.x<=X2 && cup.y>=Y3 && cup.y<=Y4){ //Salle de cinema du haut
			if(nbPointH>=NB_POINT_MAX){
				salleH[nbPointH] = cup;
				nbPointH++;
			}
		}
		if(cup.x>=X3 && cup.x<=X4 && cup.y>=Y3 && cup.y<=Y4){ //Salle de cinema du bas
			if(nbPointB>=NB_POINT_MAX){
				salleB[nbPointB] = cup;
				nbPointB++;
			}
		}
	}else{
		if(cup.x>=X1 && cup.x<=X2 && cup.y>=Y1 && cup.y<=Y2){ //Salle de cinema du haut
			if(nbPointH>=NB_POINT_MAX){
				salleH[nbPointH] = cup;
				nbPointH++;
			}
		}
		if(cup.x>=X3 && cup.x<=X4 && cup.y>=Y1 && cup.y<=Y2){ //Salle de cinema du bas
			if(nbPointB>=NB_POINT_MAX){
				salleB[nbPointB] = cup;
				nbPointB++;
			}
		}
	}
}

static GEOMETRY_point_t determine_center(GEOMETRY_point_t tab[], Uint8 nb_points){
	Sint64 sumX=0, sumY=0, sumX2=0, sumY2=0, sumX3=0, sumY3=0, sumXY=0, sumX2Y=0, sumXY2=0; //les sommes de 0 � nb_points-1 des coordonn�es. ex: sumX=somme de toutes les abscisses
	Sint64 c11=0, c20=0, c30=0, c21=0, c02=0, c03=0, c12=0; //des coefficients interm�diaires de calculs
	Uint8 i;
	GEOMETRY_point_t centre;

	//Calcul des sommes
	for(i=0;i<nb_points; i++){
		sumX=sumX+tab[i].x;
		sumY=sumY+tab[i].y;
		sumX2=sumX2+puissance(tab[i].x,2);
		sumY2=sumY2+puissance(tab[i].y,2);
		sumX3=sumX3+puissance(tab[i].x,3);
		sumY3=sumY3+puissance(tab[i].y,3);
		sumXY=sumXY+tab[i].x*tab[i].y;
		sumX2Y=sumX2Y+puissance(tab[i].x,2)*tab[i].y;
		sumXY2=sumXY2+tab[i].x*puissance(tab[i].y,2);
	}

	//Calcul des coefficients
	c11=nb_points*sumXY-sumX*sumY;
	c20=nb_points*sumX2-sumX*sumX;
	c30=nb_points*sumX3-sumX2*sumX;
	c21=nb_points*sumX2Y-sumX2*sumY;
	c02=nb_points*sumY2-sumY*sumY;
	c03=nb_points*sumY3-sumY2*sumY;
	c12=nb_points*sumXY2-sumX*sumY2;

	//calcul du centre de la balise
	centre.x = ((c30+c12)*c02-(c03+c21)*c11)/(2.*(c20*c02-c11*c11));
	centre.y = ((c03+c21)*c20-(c30+c12)*c11)/(2.*(c20*c02-c11*c11));
}

void SCAN_CUP_canMsg(CAN_msg_t *msg){
	if(msg->data[0] & 0x01)
		end_scan = TRUE;
	else
		end_scan = FALSE;
}

void SCAN_TRIANGLE_calculate(void){
	if(run_calcul){
		debug_printf("Calcul\n");
		//D�termination des centres des goblets
	}
}

#endif
