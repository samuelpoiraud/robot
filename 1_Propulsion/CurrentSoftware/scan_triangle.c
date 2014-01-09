/*
 * scan_triangle.c
 *
 *  Created on: 11 oct. 2013
 *      Author: Arnaud
 */

#include "scan_triangle.h"
#include <math.h>

#ifdef SCAN_TRIANGLE
#define NB_POINTS 90
volatile struct{Sint16 dist[3]; Sint16 x[3]; Sint16 y[3]; position_t pos;} scan[NB_POINTS];

typedef struct{
	struct{Sint16 dist; Sint16 x; Sint16 y; position_t pos;} data[NB_POINTS];
	Uint8 nbData;
	Uint8 active;
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
static void SCAN_TRIANGLE_in_process(Uint8 *n_mesure);
static void printf_float(float f);

void SCAN_TRIANGLE_init(void){
	ADC_init();
	runCalcul = 0;
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
			if(absolute(global.position.teta) >= (n_mesure+1)*PI4096/(NB_POINTS*2)){
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
			/*if(y < NB_POINTS){
				debug_printf("%d,%d;\n", scan[y].x[0], scan[y].y[0]);//, scan[y].x[1], scan[y].y[1], scan[y].x[2], scan[y].y[2]);
				y++;
			}else{
				y = 0;
				state = WAIT_END;
			}*/
		break;

		case WAIT_END :
			if(temp > 200){
					temp = 0;
					debug_printf("%ld,%ld,%ld\n", ((Sint32)(39970UL * ADC_getValue(0)) >> 16) - 56,((Sint32)(39970UL * ADC_getValue(1)) >> 16) - 56,((Sint32)(39970UL * ADC_getValue(2)) >> 16) - 56);
				}
				else
					temp++;
		break;
	}
}

static void SCAN_TRIANGLE_in_process(Uint8 *n_mesure){
	Uint8 i;
	if((*n_mesure) < NB_POINTS){
		debug_printf("%ld,%ld,%ld\n", ((Sint32)(39970UL * ADC_getValue(0)) >> 16) - 56,((Sint32)(39970UL * ADC_getValue(1)) >> 16) - 56,((Sint32)(39970UL * ADC_getValue(2)) >> 16) - 56);
		for(i=0;i<3;i++){
			scan[(*n_mesure)].dist[i] = ((Sint32)(39970UL * ADC_getValue(i)) >> 16) - 56;
		}
		scan[*n_mesure].pos = global.position;
		(*n_mesure)++;
	}

}

void SCAN_TRIANGLE_calculate(void){
	if(runCalcul){
		debug_printf("Calcul\n");
		Uint8 j, i, nb_objet[3] = {0};
		Uint16 nb_point = NB_POINTS;
		Sint16 cos, sin;
		Sint16 coef[3][NB_POINTS-1];
		static object objet[3][20];
		struct{Sint16 x; Sint16 y;} vect[3][NB_POINTS-1]; //Va recupérer les vect directeurs des 3 lasers, recupere le vecteur entre 2 points donc NB_POINTS-1



		// Initialisation
		for(i=0;i<20;i++)
			for(j=0;j<3;j++){
				objet[j][i].nbData = 0;
				objet[j][i].active = 1;
			}

		// Calcul point x et y
		for(i=0;i<NB_POINTS;i++){
				COS_SIN_4096_get(scan[i].pos.teta, &cos, &sin);
				Sint32 cos32 = cos, sin32 = sin;
				scan[i].x[0] = scan[i].pos.x - 45*cos32/4096 - (100 + scan[i].dist[0])*sin32/4096;
				scan[i].y[0] = scan[i].pos.y - 45*sin32/4096 + (100 + scan[i].dist[0])*cos32/4096;
				scan[i].x[1] = scan[i].pos.x + 25*cos32/4096 - (100 + scan[i].dist[1])*sin32/4096;
				scan[i].y[1] = scan[i].pos.y + 25*sin32/4096 + (100 + scan[i].dist[1])*cos32/4096;
				scan[i].x[2] = scan[i].pos.x + 80*cos32/4096 - (100 + scan[i].dist[2])*sin32/4096;
				scan[i].y[2] = scan[i].pos.y + 80*sin32/4096 + (100 + scan[i].dist[2])*cos32/4096;
		}

		// Calcul vecteur de chaque entre points
		for(i=0;i<NB_POINTS-1;i++){						// Validé
			for(j=0;j<3;j++){
				vect[j][i].x = scan[i+1].x[j] - scan[i].x[j];
				vect[j][i].y = scan[i+1].y[j] - scan[i].y[j];
			}
		}

		/*
		  u.v = xx' + yy'  = ||u|| * ||v|| cos(u,v)
		  -> cos(u,v) = (xx' + yy')/(||u|| * ||v||)
		  ||u|| = sqrt(x²+y²)
		  ||v|| = sqrt(x'²+y'²)
		*/
		debug_printf("valeur des scalaires : \n");
		for(i=0;i<NB_POINTS-1;i++){
			for(j=0;j<3;j++){
				printf_float(((float)vect[j][i].x*(float)vect[j][i+1].x + (float)vect[j][i].y*(float)vect[j][i+1].y)/
						(sqrt((float)vect[j][i].x*(float)vect[j][i].x + (float)vect[j][i].y*(float)vect[j][i].y) +
						 sqrt((float)vect[j][i+1].x*(float)vect[j][i+1].x + (float)vect[j][i+1].y*(float)vect[j][i+1].y)));
				if(50/100 < ((float)vect[j][i].x*(float)vect[j][i+1].x + (float)vect[j][i].y*(float)vect[j][i+1].y)/
						(sqrt((float)vect[j][i].x*(float)vect[j][i].x + (float)vect[j][i].y*(float)vect[j][i].y) +
						 sqrt((float)vect[j][i+1].x*(float)vect[j][i+1].x + (float)vect[j][i+1].y*(float)vect[j][i+1].y))){
					debug_printf("Objet détécté\n");
					nb_objet[j]++;
				}
				/*objet[j][nb_objet].data[objet[j][nb_objet].nbData].dist = scan[i].dist[j];
				objet[j][nb_objet].data[objet[j][nb_objet].nbData].x = scan[i].x[j];
				objet[j][nb_objet].data[objet[j][nb_objet].nbData].y = scan[i].y[j];
				objet[j][nb_objet].data[objet[j][nb_objet].nbData].pos = scan[i].pos;
				objet[j][nb_objet].nbData++;	*/
			}
		}
		debug_printf("fin valeur scalaires\n");
		debug_printf("Nombre d'objet pour la DT10 du bas : %d\n", nb_objet[0]);

		/*// Calcul angle
		for(i=0;i<NB_POINTS-1;i++){
			for(j=0;j<3;j++){
				if(i==0 && j==0){
					debug_printf("petit cal %f \n",(float){((vect_direc[j][i].x*vect_direc[j][i+1].x + vect_direc[j][i].y*vect_direc[j][i+1].y)/
													 (sqrt(vect_direc[j][i].x*vect_direc[j][i].x + vect_direc[j][i].y*vect_direc[j][i].y) +
													  sqrt(vect_direc[j][i+1].x*vect_direc[j][i+1].x + vect_direc[j][i+1].y*vect_direc[j][i+1].y)))});
					debug_printf("valP1 x= %d, y=%d\nval2 x= %d, y= %d\n",vect_direc[j][i].x,vect_direc[j][i].y,vect_direc[j][i+1].x,vect_direc[j][i+1].y);
				}


				if( 0,50 < (float){((vect_direc[j][i].x*vect_direc[j][i+1].x + vect_direc[j][i].y*vect_direc[j][i+1].y)/
						(sqrt(vect_direc[j][i].x*vect_direc[j][i].x + vect_direc[j][i].y*vect_direc[j][i].y) +
						 sqrt(vect_direc[j][i+1].x*vect_direc[j][i+1].x + vect_direc[j][i+1].y*vect_direc[j][i+1].y)))} &&

						(float){((vect_direc[j][i].x*vect_direc[j][i+1].x + vect_direc[j][i].y*vect_direc[j][i+1].y)/
						 (sqrt(vect_direc[j][i].x*vect_direc[j][i].x + vect_direc[j][i].y*vect_direc[j][i].y) +
						  sqrt(vect_direc[j][i+1].x*vect_direc[j][i+1].x + vect_direc[j][i+1].y*vect_direc[j][i+1].y)))} < 1){

					debug_printf("Objet Appartient i : %d\n",i);

				}
			}
		}*/


			/*for(j=0;j<3;j++){
				if((scan[i+1].x[j]-scan[i].x[j])
				  != 0)
					coef[j][i] = (10*(10*scan[i+1].y[j]-10*scan[i].y[j]))/(10*scan[i+1].x[j]-10*scan[i].x[j]);
				else if(i-1 >= 0){
					debug_printf("\nici");
					coef[j][i] = coef[j][i-1];
				}
				else{
					debug_printf("connard dégage d'ici");
					coef[j][i] = 0;
				}
			}*/


/*
		debug_printf("\nCoefficient directeur : \n");
		for(i=0;i<NB_POINTS-1;i++)
			debug_printf("%d\n", coef[0][i]);

		// Détection des différents objets
		for(i=0;i<NB_POINTS-1;i++){
			for(j=0;j<3;j++){
				if(nb_objet <20){
					if(absolute(100*coef[j][i+1] - 100*coef[j][i]) > 1)
						nb_objet++;
					objet[j][nb_objet].data[objet[j][nb_objet].nbData].dist = scan[i].dist[j];
					objet[j][nb_objet].data[objet[j][nb_objet].nbData].x = scan[i].x[j];
					objet[j][nb_objet].data[objet[j][nb_objet].nbData].y = scan[i].y[j];
					objet[j][nb_objet].data[objet[j][nb_objet].nbData].pos = scan[i].pos;
					objet[j][nb_objet].nbData++;
				}
			}
		}

		debug_printf("\nNombre d'objet : %d\n", nb_objet);*/

		// Calcul de la longueur de chaques objets
		// Et test des objets probablement des triangles
		/*Sint16 longueur[3][20];
		for(i=0;i<20;i++){
			for(j=0;j<3;j++){
				longueur[j][i] = sqrt(((objet[j][i].data[0].y - objet[j][i].data[objet[j][i].nbData].y)
						*(objet[j][i].data[0].y - objet[j][i].data[objet[j][i].nbData].y))
						+((objet[j][i].data[0].y - objet[j][i].data[objet[j][i].nbData].x)
						*(objet[j][i].data[0].y - objet[j][i].data[objet[j][i].nbData].x)));
				if(longueur[j][i] > 100 && longueur[j][i] < 130)
					objet[j][i].active = 0;

			}
		}

		debug_printf("Longueur : \n");
		for(i=0;i<nb_objet+1;i++)
			debug_printf("%d\n",longueur[0][i]);


		// Calcul du point milieu de chaques objets
		position_t point_milieu[3][20];
		for(i=0;i<20;i++){
			for(j=0;j<3;j++){
				point_milieu[j][i].x = (objet[j][i].data[0].x + objet[j][i].data[objet[j][i].nbData].x)/2;
				point_milieu[j][i].y = (objet[j][i].data[0].y + objet[j][i].data[objet[j][i].nbData].y)/2;
			}
		}

		debug_printf("Point milieu : \n");
		for(i=0;i<nb_objet;i++)
			debug_printf("%d %d\n",point_milieu[0][i].x,point_milieu[0][i].y);*/

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

static void printf_float(float f){
	printf("%d.%03d", (int)f/1000, absolute((Sint16)f)%1000);
}
#endif
