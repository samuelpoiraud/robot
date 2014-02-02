/*
 * scan_triangle.c
 *
 *  Created on: 11 oct. 2013
 *      Author: Arnaud
 */

#include "scan_triangle.h"
#include "QS/QS_adc.h"
#include "QS/QS_all.h"
#include "QS/QS_outputlog.h"
#include "supervisor.h"
#include "secretary.h"
#include "corrector.h"
#include "roadmap.h"
#include "cos_sin.h"
#include <math.h>

#ifdef SCAN_TRIANGLE

#define square(x) ((float)(x)*(float)(x))
#define conversion_DT10_mm(x) (((((Sint32)(597)*(x))/1024)- 78))
// étalonnage antiérieur : ((((Sint32)(39970UL*(x)))>>16)- 56)
// Rapport sur la courbe DT10 couleur noir  y = 0,667*x - 119
// Rapport sur la courbe DT10 couleur blanc y = 0,583*x - 78,19

#define X_SENSOR_BOT -66	// -45
#define Y_SENSOR_BOT 90		// 100
#define X_SENSOR_MID 78		// 25
#define Y_SENSOR_MID 98		// 100
#define X_SENSOR_TOP 46		// 80
#define Y_SENSOR_TOP 98		// 100

#define DIST_ARETE_MILIEU 47

#define NB_POINTS 90


volatile struct{Sint16 dist[3]; Sint16 x[3]; Sint16 y[3]; position_t pos;} scan[NB_POINTS];
// Variable contenant la distance et les coordonées x y de chaque points du scan

struct{
	Uint8 indice_debut_point, indice_fin_point;
	bool_e active;
	Sint16 longueur;
	struct{Sint16 x; Sint16 y;} point_milieu;
	struct{Sint16 x; Sint16 y;} milieu;
	struct{float x; float y;} vecteur;
	struct{float x; float y;} vecteur_Perpen;
}objet[3][20];
// Variable contenant les informations sur chaque objets différent repérés pendant le scan

struct{Sint16 x; Sint16 y;} vect[3][NB_POINTS-1];
// Variable contenant les vecteurs des entre-points des coordonnées du scan

Sint16 angle_vecteur[3][NB_POINTS-1];
// Variable contenant les angles de chaque vecteurs

Uint8 nb_objet[3] = {0};
// Variable contenant le nombre d'objet par capteur

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
	SEND_MID_TRIANGLE,
	TEST
}state_e;

Uint8 move_completed, run_calcul;
bool_e b_ask_for_scan = FALSE;


static Uint8 EstDansLeCarre(Sint16 lx, Sint16 hx, Sint16 ly, Sint16 hy);
static void Acknowledge_Intern(void);
static void SCAN_TRIANGLE_in_process(Uint8 *n_mesure);
static float racine(float val);
static Sint16 distance(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2);

void SCAN_TRIANGLE_init(void){
	ADC_init();
	run_calcul = 0;
	// Initialisation des convertisseurs analogiques numériques pour les DT10 de détection triangle
}


void SCAN_TRIANGLE_process_it(void){
	static state_e state = INIT;
	static Uint8 zone;
	static Uint16 temp = 0;
	static Uint8 n_mesure = 0;
	static Uint16 time;
	static Uint8 i, j, k;


	switch(state){

		case INIT :
			move_completed = 0;
			run_calcul = 0;
			state = WAIT;
		break;

		case TEST :
			if(temp > 500){
					temp = 0;
					debug_printf("%d,%d,%d\n", (Sint16)ADC_getValue(0), (Sint16)ADC_getValue(1), (Sint16)ADC_getValue(2));
					debug_printf("%d,%d,%d\n\n", (Sint16)conversion_DT10_mm(ADC_getValue(0)), (Sint16)conversion_DT10_mm(ADC_getValue(1)), (Sint16)conversion_DT10_mm(ADC_getValue(2)));
				}
				else
					temp++;
		break;

		case WAIT :
			if(b_ask_for_scan){
				b_ask_for_scan = FALSE;
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
						SLOW, INTERN_ACKNOWLEDGE, CORRECTOR_ENABLE);
			}else if(zone == 2 && !EstDansLeCarre(1300, 2000, 0, 700)){
				SUPERVISOR_config_intern_acknowledge(&Acknowledge_Intern);
				ROADMAP_add_order(TRAJECTORY_TRANSLATION,1600, 400, 0,
						NOT_RELATIVE, NOW, FORWARD,	NOT_BORDER_MODE, NO_MULTIPOINT,
						SLOW, INTERN_ACKNOWLEDGE, CORRECTOR_ENABLE);
			}else if(zone == 3 && !EstDansLeCarre(750, 1250, 1250, 1750)){
				//Déplacement
			}
			state = PLACEMENT_WAIT_XY;
		break;

		case PLACEMENT_WAIT_XY :
			if(move_completed){
				move_completed = 0;
				state = PLACEMENT_TETA;
			}
		break;

		case PLACEMENT_TETA :
			if(zone == 1){
				SUPERVISOR_config_intern_acknowledge(Acknowledge_Intern);
				ROADMAP_add_order(TRAJECTORY_ROTATION,	0, 0, 0,
						NOT_RELATIVE, NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT,
						SLOW, INTERN_ACKNOWLEDGE, CORRECTOR_ENABLE);
			}else if(zone == 2){
				SUPERVISOR_config_intern_acknowledge(Acknowledge_Intern);
				ROADMAP_add_order(TRAJECTORY_ROTATION,	0, 0, -PI4096/2,
						NOT_RELATIVE, NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT,
						SLOW, INTERN_ACKNOWLEDGE, CORRECTOR_ENABLE);
			}else if(zone == 3){
				//Déplacement
			}
			state = PLACEMENT_WAIT_TETA;
		break;

		case PLACEMENT_WAIT_TETA :
			if(move_completed){
				move_completed = 0;
				state = LAUNCH_WARNER;
			}
		break;

		case LAUNCH_WARNER :
			if(zone == 1){
				SUPERVISOR_config_intern_acknowledge(Acknowledge_Intern);
				ROADMAP_add_order(TRAJECTORY_ROTATION,	0, 0, -PI4096/2,
						NOT_RELATIVE, NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT,
						20, INTERN_ACKNOWLEDGE, CORRECTOR_ENABLE);
			}else if(zone == 2 ){
				SUPERVISOR_config_intern_acknowledge(Acknowledge_Intern);
				ROADMAP_add_order(TRAJECTORY_ROTATION,	0, 0, -PI4096,
						NOT_RELATIVE, NOW, FORWARD, NOT_BORDER_MODE, NO_MULTIPOINT,
						20, INTERN_ACKNOWLEDGE, CORRECTOR_ENABLE);
			}else if(zone == 3){
				//Déplacement
			}
			state = IN_PROGRESS;
			n_mesure = 0;
		break;

		case IN_PROGRESS :
			if(zone == 1 && absolute(global.position.teta) >= (n_mesure+1)*PI4096/(NB_POINTS*2)){
				SCAN_TRIANGLE_in_process(&n_mesure);
			}else if(zone == 2 && absolute(global.position.teta + PI4096/2) >= (n_mesure+1)*PI4096/(NB_POINTS*2)){
				SCAN_TRIANGLE_in_process(&n_mesure);
			}
			if(move_completed){
				move_completed = 0;
				run_calcul = 1;
				time = 0;
				state = WAIT_CALCULATE;
			}
		break;

		case WAIT_CALCULATE :
			if(!run_calcul){
				debug_printf("Temps de calculs : %d\n", time);
				state = SEND_MID_TRIANGLE;
			}else
				time += 5;
		break;

		case SEND_MID_TRIANGLE :
			k = 0;
			for(j=0;j<3;j++){
				for(i=0;i<nb_objet[j];i++,k++){
					if(objet[j][i].active)
						SECRETARY_send_triangle_position(FALSE, k, objet[j][i].milieu.x, objet[j][i].milieu.y, (Sint16)(atan2(objet[j][i].vecteur.y, objet[j][i].vecteur.x)*PI4096/3.14));
				}
			}
			SECRETARY_send_triangle_position(TRUE, k, 0, 0, 0);

			state = WAIT;
		break;
	}
}

static void SCAN_TRIANGLE_in_process(Uint8 *n_mesure){
	Uint8 i;
	if((*n_mesure) < NB_POINTS){
		for(i=0;i<3;i++){
			scan[(*n_mesure)].dist[i] = conversion_DT10_mm(ADC_getValue(i));
		}
		scan[*n_mesure].pos = global.position;
		(*n_mesure)++;
	}

}

void SCAN_TRIANGLE_calculate(void){
	if(run_calcul){
		debug_printf("Calcul\n");
		Uint8 k;
		Sint16 i, j;
		Sint16 cos, sin;

		// Initialisation
		for(i=0;i<20;i++)
			for(j=0;j<3;j++){
				objet[j][i].indice_debut_point = 0;
				objet[j][i].indice_fin_point = 0;
				objet[j][i].active = FALSE;
				objet[j][i].longueur = 0;
				objet[j][i].milieu.x = 0;
				objet[j][i].milieu.y = 0;
				objet[j][i].vecteur.x = 0;
				objet[j][i].vecteur.y = 0;
			}

		// Calcul point x et y
		for(i=0;i<NB_POINTS;i++){									// Validé
				COS_SIN_4096_get(scan[i].pos.teta, &cos, &sin);
				Sint32 cos32 = cos, sin32 = sin;
				scan[i].x[0] = scan[i].pos.x + X_SENSOR_BOT*cos32/4096 - (Y_SENSOR_BOT + scan[i].dist[0])*sin32/4096;
				scan[i].y[0] = scan[i].pos.y + X_SENSOR_BOT*sin32/4096 + (Y_SENSOR_BOT + scan[i].dist[0])*cos32/4096;
				scan[i].x[1] = scan[i].pos.x + X_SENSOR_MID*cos32/4096 - (Y_SENSOR_MID + scan[i].dist[1])*sin32/4096;
				scan[i].y[1] = scan[i].pos.y + X_SENSOR_MID*sin32/4096 + (Y_SENSOR_MID + scan[i].dist[1])*cos32/4096;
				scan[i].x[2] = scan[i].pos.x + X_SENSOR_TOP*cos32/4096 - (Y_SENSOR_TOP + scan[i].dist[2])*sin32/4096;
				scan[i].y[2] = scan[i].pos.y + X_SENSOR_TOP*sin32/4096 + (Y_SENSOR_TOP + scan[i].dist[2])*cos32/4096;
		}

		// Calcul vecteur de chaque entre points
		for(i=0;i<NB_POINTS-1;i++){						// Validé
			for(j=0;j<3;j++){
				vect[j][i].x = scan[i+1].x[j] - scan[i].x[j];
				vect[j][i].y = scan[i+1].y[j] - scan[i].y[j];
				if(vect[j][i].x == 0 && vect[j][i].y == 0){
					if(i == 0){
						vect[j][i].x = 1;
						vect[j][i].y = 1;
					}else{
						vect[j][i].x = vect[j][i-1].x;
						vect[j][i].y = vect[j][i-1].y;
					}
				}
			}
		}

		// Calcul de l'angle entre deux vecteurs
		for(i=0;i<NB_POINTS-1;i++){						// Validé
			for(j=0;j<3;j++){
				angle_vecteur[j][i] = (Sint16)(atan2(vect[j][i].y, vect[j][i].x)*180/3.14);
			}
		}


		// filtrer temp futur
		for(i=1;i<NB_POINTS-1;i++){						// Validé
			for(j=0;j<3;j++){
				if(i==NB_POINTS-3){
					angle_vecteur[j][i] = (angle_vecteur[j][i] + angle_vecteur[j][i+1])/2;
				}else if(i==NB_POINTS-4){
					angle_vecteur[j][i] = (angle_vecteur[j][i] + angle_vecteur[j][i+1] + angle_vecteur[j][i+2])/3;
				}else if (i!=NB_POINTS-2){
					angle_vecteur[j][i] = (angle_vecteur[j][i] + angle_vecteur[j][i+1] + angle_vecteur[j][i+2] +  angle_vecteur[j][i+3])/4;
				}
			}
		}



		// filtrer reverse temp futur
		for(i=NB_POINTS-3;i>=0;i--){					// Validé
			for(j=0;j<3;j++){
				if(i==1){
					angle_vecteur[j][i] = (angle_vecteur[j][i] + angle_vecteur[j][i-1])/2;
				}else if(i==2){
					angle_vecteur[j][i] = (angle_vecteur[j][i] + angle_vecteur[j][i-1] + angle_vecteur[j][i-2])/3;
				}else if(i!=0){
					angle_vecteur[j][i] = (angle_vecteur[j][i] + angle_vecteur[j][i-1] + angle_vecteur[j][i-2] +  angle_vecteur[j][i-3])/4;
				}
			}
		}

		// dérivée
		for(i=0;i<NB_POINTS-2;i++){						// Validé
			for(j=0;j<3;j++){
				angle_vecteur[j][i] -= angle_vecteur[j][i+1];
			}
		}

		// seuil changement brutal
		for(i=0;i<NB_POINTS-1;i++){						// Validé
			for(j=0;j<3;j++){
				if((int)(absolute(angle_vecteur[j][i])) > 10){
					angle_vecteur[j][i] = 1;
				}else{
					angle_vecteur[j][i] = 0;
				}
			}
		}

		// Detection d'objet
		for(i=0;i<NB_POINTS-1;i++){						// Validé
			for(j=0;j<3;j++){
				if(nb_objet[j] < 20){
					if(i==0)
						objet[j][nb_objet[j]].indice_debut_point = i;
					else if(i!=0 && angle_vecteur[j][i-1] == 0 && angle_vecteur[j][i] == 1){
						objet[j][nb_objet[j]].indice_fin_point = i;
						nb_objet[j]++;
					}
					else if(i!=0 && angle_vecteur[j][i-1] == 1 && angle_vecteur[j][i] == 0)
						objet[j][nb_objet[j]].indice_debut_point = i;
				}
			}
			if(objet[j][nb_objet[j]].indice_fin_point == 0){
				objet[j][nb_objet[j]].indice_fin_point = i;
				nb_objet[j]++;
			}
		}

		// Séparation des objets trop éloigné
		for(j=0;j<3;j++){
			for(k=0;k<nb_objet[j];k++){
				if(nb_objet[j] < 20){
					for(i=objet[j][k].indice_debut_point;i<=objet[j][k].indice_fin_point;i++){
						if(distance(scan[j].x[i], scan[j].y[i], scan[j].x[i+1], scan[j].y[i+1]) > 30){
							objet[j][nb_objet[j]].indice_fin_point = objet[j][k].indice_fin_point;
							objet[j][nb_objet[j]].indice_debut_point = i+1;
							objet[j][k].indice_fin_point = i;
							nb_objet[j]++;
						}
					}
				}
			}
		}

		// Retrait des objets où il ne faut pas faire de traitement
		for(j=0;j<3;j++){
			for(k=0;k<nb_objet[j];k++){
				objet[j][k].active = FALSE;

				for(i=objet[j][k].indice_debut_point;i<=objet[j][k].indice_fin_point;i++)
					if((j == 0 &&
							((scan[i].y[j] < 2850 && scan[i].x[j] > 0 && scan[i].x[j] < 1625)
							|| (scan[i].y[j] > 150 && scan[i].x[j] > 0 && scan[i].x[j] < 1625)
							|| (scan[i].x[j] < 1850 && scan[i].y[j] > 375 && scan[i].y[j] < 2652)
							|| (scan[i].y[j] < -scan[i].x[j] + 4625)// Vrai si x y dans le terrain
							|| (scan[i].y[j] > scan[i].x[j] + 375))) // Vrai si x y dans le terrain
						|| (j == 1 && scan[i].x[j] > 250 && scan[i].x[j] < 1850 && scan[i].y[j] > 250 && scan[i].y[j] < 2850)
						|| (j == 2 && scan[i].x[j] > 250 && scan[i].x[j] < 1850 && scan[i].y[j] > 250 && scan[i].y[j] < 2850))
						objet[j][k].active = TRUE;

				// autre idée (x-x')²-(y-y')² = sqrt(rayon) avec x' = 2000 et y' = 3000 et rayon = 250
				// droite partant de 1625/3000 à 2000/2625
				// droite d'équation : y = -x + 4625
				// droite partant de 1625/0 à 2000/375
				// droite d'équation : y = x + 375
			}
		}


		// Calcul de la longueur de chaques objets
		// Et test des objets probablement des triangles
		for(j=0;j<3;j++){
			for(i=0;i<nb_objet[j];i++){
				if(1){
					objet[j][i].longueur = distance(scan[objet[j][i].indice_debut_point].x[j], scan[objet[j][i].indice_debut_point].y[j],
							scan[objet[j][i].indice_fin_point].x[j], scan[objet[j][i].indice_fin_point].x[j]);
					if(objet[j][i].longueur < 80 || objet[j][i].longueur > 160)
						objet[j][i].active = FALSE;
				}
			}
		}

		// vecteurs unitaire d'un objet avec moyennage des vecteurs de l'objet
		for(j=0;j<3;j++){
			for(i=0;i<nb_objet[j];i++){
				if(objet[j][i].longueur){
					for(i=objet[j][k].indice_debut_point;i<=objet[j][k].indice_fin_point-1;i++){
						objet[j][i].vecteur.x += vect[j][i].x;
						objet[j][i].vecteur.x += vect[j][i].x;
					}
					objet[j][i].vecteur.x /= (float)(objet[j][i].indice_fin_point-objet[j][i].indice_debut_point);
					objet[j][i].vecteur.y /= (float)(objet[j][i].indice_fin_point-objet[j][i].indice_debut_point);
					objet[j][i].vecteur.x /= (float)(objet[j][i].longueur);
					objet[j][i].vecteur.y /= (float)(objet[j][i].longueur);
				}
			}
		}

		// Calcul du point milieu de chaques objets
		for(j=0;j<3;j++){								// Validé
			for(i=0;i<nb_objet[j];i++){
				objet[j][i].point_milieu.x = (scan[objet[j][i].indice_debut_point].x[j] + scan[objet[j][i].indice_fin_point].x[j])/2;
				objet[j][i].point_milieu.y = (scan[objet[j][i].indice_debut_point].y[j] + scan[objet[j][i].indice_fin_point].y[j])/2;
			}
		}


		// vecteur unitaire perpendiculaire
		for(j=0;j<3;j++){								// Validé
			for(i=0;i<nb_objet[j];i++){
				objet[j][i].vecteur_Perpen.y = objet[j][i].vecteur.x;
				objet[j][i].vecteur_Perpen.x = -objet[j][i].vecteur.y;
			}
		}

		// Calcul milieu triangle
		for(j=0;j<3;j++){								// Validé
			for(i=0;i<nb_objet[j];i++){
				objet[j][i].milieu.x = objet[j][i].point_milieu.x + DIST_ARETE_MILIEU * objet[j][i].vecteur_Perpen.x;
				objet[j][i].milieu.y = objet[j][i].point_milieu.y + DIST_ARETE_MILIEU * objet[j][i].vecteur_Perpen.y;
			}
		}

		/*debug_printf("#Actif\n");
		for(i=0;i<nb_objet[0];i++){
			debug_printf("0;%d\n", i);
			debug_printf("%d\n", objet[0][i].active);
		}

		debug_printf("#Longueur\n");
		for(i=0;i<nb_objet[0];i++){
			debug_printf("0;%d\n", i);
			debug_printf("%d\n", objet[0][i].longueur);
		}*/


	#ifdef SOFT_SCAN_TRIANGLE
		debug_printf("###-DEBUT_SCAN-###\n");
		/*for(k=0;k<3;k++){
			debug_printf("#Point\n");
			debug_printf("%d\n", k);
			for(i=0;i<90;i++){
				debug_printf("%d;%d\n", scan[i].x[k], scan[i].y[k]);
			}
			debug_printf("#End_Point\n");
		}*/
		for(k=0;k<1;k++){
			for(i=0;i<nb_objet[k];i++){
				debug_printf("#Objet\n");
				debug_printf("%d;%d\n", k, i);
				for(j=objet[k][i].indice_debut_point;j<=objet[k][i].indice_fin_point;j++){
					debug_printf("%d;%d\n", scan[j].x[k], scan[j].y[k]);
				}
				debug_printf("#End_Objet\n");
			}

			for(i=0;i<nb_objet[k];i++){
				if(objet[k][i].active){
					debug_printf("#PointMilieu\n");
					debug_printf("%d;%d\n", k, i);
					debug_printf("%d;%d\n", objet[k][i].point_milieu.x, objet[k][i].point_milieu.y);
					debug_printf("#End_PointMilieu\n");
				}
			}

			for(i=0;i<nb_objet[k];i++){
				if(objet[k][i].active){
					debug_printf("#Milieu\n");
					debug_printf("%d;%d\n", k, i);
					debug_printf("%d;%d\n", objet[k][i].milieu.x, objet[k][i].milieu.y);
					debug_printf("#End_Milieu\n");
				}
			}
		}
		debug_printf("#Robot\n");
		debug_printf("%d;%d\n", global.position.x, global.position.y);
		debug_printf("###-FIN_SCAN-###\n");
	#endif
		run_calcul = 0;
	}
}

void SCAN_TRIANGLE_canMsg(void){
	b_ask_for_scan = TRUE;
}

static Uint8 EstDansLeCarre(Sint16 lx, Sint16 hx, Sint16 ly, Sint16 hy){
	return global.position.x > lx && global.position.x < hx
			&& global.position.y > ly && global.position.y < hy;
}

float racine(float val){
	if(val >= 0)
		return sqrt(val);
	else{
		debug_printf("######Racine d'une valeur négative ! val = %d######\n", (int)val);
		return 0;
	}

}

static void Acknowledge_Intern(void){
	move_completed = 1;
}

static Sint16 distance(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2){
	return racine(square((Sint32)(y1 - y2)) + square(((Sint32)(x1 - x2))));
}

#endif

/*
  u.v = xx' + yy'  = ||u|| * ||v|| cos(u,v) = 1/2(||u + v||² - ||u||² - ||v||²)
  -> cos(u,v) = (xx' + yy')/(||u|| * ||v||)
  ||u|| = sqrt(x²+y²)
  ||v|| = sqrt(x'²+y'²)
*/
/*debug_printf("valeur des scalaires : \n");
for(i=0;i<NB_POINTS-1;i++){
	for(j=0;j<3;j++){
		if(i != 0)
			debug_printf("%d\n", (int)(absolute((1000*((vect[j][i-1].x)*(vect[j][i].x) + (vect[j][i-1].y)*(vect[j][i].y)))/
						 (racine(square(vect[j][i-1].x) + square(vect[j][i-1].y))*
						  racine(square(vect[j][i].x) + square(vect[j][i].y))))));
		objet[j][nb_objet[j]].data[objet[j][nb_objet[j]].nbData].x = scan[i].x[j];
		objet[j][nb_objet[j]].data[objet[j][nb_objet[j]].nbData].y = scan[i].y[j];
		objet[j][nb_objet[j]].data[objet[j][nb_objet[j]].nbData].dist = scan[i].dist[j];
		objet[j][nb_objet[j]].data[objet[j][nb_objet[j]].nbData].pos = scan[i].pos;
		objet[j][nb_objet[j]].nbData++;
		if(i != 0 && (500 < (int)(absolute((1000*((vect[j][i-1].x)*(vect[j][i].x) + (vect[j][i-1].y)*(vect[j][i].y)))/
					(racine(square(vect[j][i-1].x) + square(vect[j][i-1].y))*
					 racine(square(vect[j][i].x) + square(vect[j][i].y))))))
					|| racine(square((Sint32)(scan[i-1].y[j] - scan[i].y[j]))
					   + square(((Sint32)(scan[i-1].x[j] - scan[i].x[j])))) < 50)
				&& nb_objet[j] < 20)
			nb_objet[j]++;
		if((int)(absolute((1000*((vect[j][i-1].x)*(vect[j][i].x) + (vect[j][i-1].y)*(vect[j][i].y)))/
			   (racine(square(vect[j][i-1].x) + square(vect[j][i-1].y))*
				racine(square(vect[j][i].x) + square(vect[j][i].y)))))> 1000)
			printf("V1 %d %d\nV2 %d %d\nnorme V1 %d\nnorme V2 %d\n produit scalaire %d\n", (int)vect[j][i-1].x, (int)vect[j][i-1].y, (int)vect[j][i].x, (int)vect[j][i].y,
					(int)(racine(square(vect[j][i-1].x) + square(vect[j][i-1].y))), (int)(racine(square(vect[j][i].x) + square(vect[j][i].y))),
					(int)(1000*((vect[j][i-1].x)*(vect[j][i].x) + (vect[j][i-1].y)*(vect[j][i].y))));
	}
}
nb_objet[0]++;
nb_objet[1]++;
nb_objet[2]++;*/


/*
// filtrer temp passé
for(i=1;i<NB_POINTS-1;i++){
	for(j=0;j<3;j++){
		if(i==1){
			angle_vecteur_temp[j][i] = (angle_vecteur[j][i] + angle_vecteur[j][i-1])/2;
		}else if(i==2){
			angle_vecteur_temp[j][i] = (angle_vecteur[j][i] + angle_vecteur[j][i-1] + angle_vecteur[j][i-2])/3;
		}else{
			angle_vecteur_temp[j][i] = (angle_vecteur[j][i] + angle_vecteur[j][i-1] + angle_vecteur[j][i-2] +  angle_vecteur[j][i-3])/4;
		}
	}
}

// filtrer reverse temp passé
for(i=NB_POINTS-3;i>=0;i--){
	for(j=0;j<3;j++){
		if(i==NB_POINTS-3){
			angle_vecteur[j][i] = (angle_vecteur_temp[j][i] + angle_vecteur_temp[j][i+1])/2;
		}else if(i==NB_POINTS-4){
			angle_vecteur[j][i] = (angle_vecteur_temp[j][i] + angle_vecteur_temp[j][i+1] + angle_vecteur_temp[j][i+2])/3;
		}else{
			angle_vecteur[j][i] = (angle_vecteur_temp[j][i] + angle_vecteur_temp[j][i+1] + angle_vecteur_temp[j][i+2] +  angle_vecteur_temp[j][i+3])/4;
		}
	}
}*/

/*// filtrage des coordonées des objets
for(j=0;j<3;j++){
	for(k=0;k<nb_objet[j];k++){
		for(i=objet[j][nb_objet[j]].indice_debut_point;i<=objet[j][nb_objet[j]].indice_fin_point;i++){
			if(i != objet[j][nb_objet[j]].indice_fin_point){
				objet[j][k].data[i].x = (objet[j][k].data[i].x + objet[j][k].data[i+1].x)/2;
				objet[j][k].data[i].y = (objet[j][k].data[i].y + objet[j][k].data[i+1].y)/2;
			}
		}
	}
}

// filtrage reverse des coordonées des objets
for(j=0;j<3;j++){
	for(k=0;k<nb_objet[j];k++){
		for(i=objet[j][nb_objet[j]].indice_debut_point;i<=objet[j][nb_objet[j]].indice_fin_point;i++){
			if(i != objet[j][nb_objet[j]].indice_debut_point){
				objet[j][k].data[i].x = (objet[j][k].data[i].x + objet[j][k].data[i-1].x)/2;
				objet[j][k].data[i].y = (objet[j][k].data[i].y + objet[j][k].data[i-1].y)/2;
			}
		}
	}
}*/
