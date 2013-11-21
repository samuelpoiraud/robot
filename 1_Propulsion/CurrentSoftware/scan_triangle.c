/*
 * scan_triangle.c
 *
 *  Created on: 11 oct. 2013
 *      Author: Arnaud
 */

#include "scan_triangle.h"

#define NB_POINTS 90

volatile struct{Sint16 dist[3]; Sint16 x[3]; Sint16 y[3]; position_t pos;} scan[NB_POINTS];

typedef struct{
	struct{Sint16 teta[3]; position_t pos;} data[90];
}object;

typedef enum{
	INIT=0,
	WAIT,
	PLACEMENT_XY,
	PLACEMENT_WAIT_XY,
	PLACEMENT_TETA,
	PLACEMENT_WAIT_TETA,
	LAUNCH_WARNER,
	IN_PROGRESS,
	WAIT_CALCULATE,
	DISPLAY,
	WAIT_END
}state_c;

Uint8 moveCompleted, runCalcul;
bool_e MESSAGE_CAN_SCAN = FALSE;

static Uint8 EstDansLeCarre(Sint16 lx, Sint16 hx, Sint16 ly, Sint16 hy);
static void Acknowledge_Intern(void);
static Sint32 abs(Sint32 val);
static void SCAN_TRIANGLE_in_process(Uint8 *n_mesure);

//void SUPERVISOR_config_intern_acknowledge(void (*prt)(void));
void SCAN_TRIANGLE_init(void){
	ADC_init();
	// Initialisation des convertisseurs analogiques numériques pour les DT10 de détection triangle
}


void SCAN_TRIANGLE_process_it(void){
	static state_c state = INIT;
	static Uint8 zone;
	static Uint16 temp = 0;
	static Uint8 y = 0;
	static Uint8 n_mesure = 0;


	switch(state){

		case INIT :
			moveCompleted = 0;
			runCalcul = 0;
			state = WAIT;
		break;

		case WAIT :
			if(MESSAGE_CAN_SCAN){
				MESSAGE_CAN_SCAN = FALSE;
				state = PLACEMENT_XY;
			}
		break;

		case PLACEMENT_XY :
			// Analyse de la zone de scan
			if(EstDansLeCarre(1300, 2000, 2300, 3000))		// Foyer haut gauche
				zone = 1;
			else if(EstDansLeCarre(1300, 2000, 0, 700))		// Foyer haut droite
				zone = 2;
			else if(EstDansLeCarre(750, 1250, 1250, 1750))	// Foyer milieu
				zone = 3;

			if(zone == 1){
				SUPERVISOR_config_intern_acknowledge(&Acknowledge_Intern);
				ROADMAP_add_order(TRAJECTORY_TRANSLATION,1600, 2600, 0,
						NOT_RELATIVE, NOW, FORWARD,	NOT_BORDER_MODE, NO_MULTIPOINT,
						FAST, INTERN_ACKNOWLEDGE, CORRECTOR_ENABLE);
			}else if(zone == 2 && !EstDansLeCarre(1300, 2000, 0, 700)){
				//Déplacement
			}else if(zone == 3 && !EstDansLeCarre(750, 1250, 1250, 1750)){
				//Déplacement
			}
			state = PLACEMENT_WAIT_XY;
		break;

		case PLACEMENT_WAIT_XY :
			if(moveCompleted){
				moveCompleted = 0;
				state = PLACEMENT_TETA;
			}
		break;

		case PLACEMENT_TETA :
			if(zone == 1){
				SUPERVISOR_config_intern_acknowledge(Acknowledge_Intern);
				ROADMAP_add_order(TRAJECTORY_ROTATION,	0, 0, 0,
						NOT_RELATIVE, NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT,
						FAST, INTERN_ACKNOWLEDGE, CORRECTOR_ENABLE);
			}else if(zone == 2){
				SUPERVISOR_config_intern_acknowledge(Acknowledge_Intern);
				ROADMAP_add_order(TRAJECTORY_ROTATION,	0, 0, 0,
						NOT_RELATIVE, NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT,
						FAST, INTERN_ACKNOWLEDGE, CORRECTOR_ENABLE);
			}else if(zone == 3){
				//Déplacement
			}
			state = PLACEMENT_WAIT_TETA;
		break;

		case PLACEMENT_WAIT_TETA :
			if(moveCompleted){
				moveCompleted = 0;
				state = LAUNCH_WARNER;
			}
		break;

		case LAUNCH_WARNER :
			if(zone == 1){
				SUPERVISOR_config_intern_acknowledge(Acknowledge_Intern);
				ROADMAP_add_order(TRAJECTORY_ROTATION,	0, 0, -PI4096/2,
						NOT_RELATIVE, NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT,
						SLOW, INTERN_ACKNOWLEDGE, CORRECTOR_ENABLE);
			}else if(zone == 2 ){
				SUPERVISOR_config_intern_acknowledge(Acknowledge_Intern);
				ROADMAP_add_order(TRAJECTORY_ROTATION,	0, 0, PI4096/2,
						NOT_RELATIVE, NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT,
						SLOW, INTERN_ACKNOWLEDGE, CORRECTOR_ENABLE);
			}else if(zone == 3){
				//Déplacement
			}
			state = IN_PROGRESS;
			n_mesure = 0;
			debug_printf("\nNouveau scan : \n");
		break;

		case IN_PROGRESS :
			if(abs(global.position.teta) >= (n_mesure+1)*PI4096/(NB_POINTS*2)){
				SCAN_TRIANGLE_in_process(&n_mesure);
			}
			if(moveCompleted){
				moveCompleted = 0;
				y = 0;
				runCalcul = 1;
				state = WAIT_CALCULATE;
			}
		break;

		case WAIT_CALCULATE :
			if(!runCalcul)
				state = DISPLAY;
		break;

		case DISPLAY :
			if(y < NB_POINTS){
				debug_printf("%d,%d\n", scan[y].x[0], scan[y].y[0]);
				y++;
			}else{
				y = 0;
				state = WAIT_END;
			}
		break;

		case WAIT_END :
			if(temp > 5000){
					temp = 0;
					state = PLACEMENT_TETA;
				}
				else
					temp++;
		break;
	}
}

static void SCAN_TRIANGLE_in_process(Uint8 *n_mesure){
	Uint8 i;
	if((*n_mesure) < NB_POINTS){
		debug_printf("%ld\n", ((Sint32)(39970UL * ADC_getValue(0)) >> 16) - 56);
		for(i=0;i<3;i++){
			scan[(*n_mesure)].dist[i] = ((Sint32)(39970UL * ADC_getValue(i)) >> 16) - 56;
		}
		scan[*n_mesure].pos = global.position;
		(*n_mesure)++;
	}

}

void SCAN_TRIANGLE_calculate(void){
	if(runCalcul){
		Uint8 j, i;
		Sint16 cos, sin;
		for(j=0;j<NB_POINTS;j++){
			for(i=0;i<3;i++){
				COS_SIN_4096_get(scan[j].pos.teta, &cos, &sin);
				scan[j].x[i] = scan[j].pos.x + 90*cos/4096 - (130 + scan[j].dist[i])*sin/4096;
				scan[j].y[i] = scan[j].pos.y + 90*sin/4096 + (130 + scan[j].dist[i])*cos/4096;
			}
		}
		runCalcul = 0;
	}
}

void SCAN_TRIANGLE_canMsg(void){
	MESSAGE_CAN_SCAN = TRUE;
}

static Uint8 EstDansLeCarre(Sint16 lx, Sint16 hx, Sint16 ly, Sint16 hy){
	return global.position.x > lx && global.position.x < hx
			&& global.position.y > ly && global.position.y < hy;
}

static void Acknowledge_Intern(void){
	moveCompleted = 1;
}

static Sint32 abs(Sint32 val){
	if(val > 0)
		return val;
	else
		return -val;
}
