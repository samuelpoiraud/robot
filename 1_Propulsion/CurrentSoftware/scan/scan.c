#include "scan.h"
#include "objects_scan.h"
#include "../QS/QS_outputlog.h"
#include "../QS/QS_adc.h"


#define CONVERSION_LASER_LEFT(x)	((Sint32)(-264*(x)+35350)/1000)
#define OFFSET_WIDTH_LASER_LEFT		(-150)
#define OFFSET_LENGTH_LASER_LEFT	(70)

#define CONVERSION_LASER_RIGHT(x)	((Sint32)(-270*(x)+35580)/1000)
#define OFFSET_WIDTH_LASER_RIGHT	(150)
#define OFFSET_LENGTH_LASER_RIGHT	(70)

#define SATURATION_LOW (300)
#define SATURATION_HIGH (1000)

static void SCAN_get_data_left();
static void SCAN_get_data_right();

static volatile scan_data_t laser_left[2*NB_SCAN_DATA];
static volatile scan_data_t laser_right[2*NB_SCAN_DATA];
static volatile Uint16 index = 0;
static volatile bool_e flag_1, flag_2, flag_treatment_too_slow;
static scan_data_t tab_treatment[NB_SCAN_DATA];



void SCAN_init(){
	Uint16 i;

	flag_1 = FALSE;
	flag_2 = FALSE;
	flag_treatment_too_slow = FALSE;

	index = 0;

	for(i=0; i<2*NB_SCAN_DATA; i++){
		laser_left[i].enable = FALSE;
		laser_right[i].enable = FALSE;
	}
}

static void SCAN_get_data_left(){
	GEOMETRY_point_t pos_mesure, pos_laser;
	bool_e enable;
	Sint16 valueADC, value;
	position_t robot;
	Sint16 cosinus = 0, sinus = 0;

	valueADC = ADC_getValue(ADC_SENSOR_LASER_LEFT);
	value = CONVERSION_LASER_LEFT(valueADC);
	robot = global.position; // On récupère la position du robot tout de suite
	robot.teta = GEOMETRY_modulo_angle(robot.teta);
	COS_SIN_4096_get(robot.teta, &cosinus, &sinus);

	// On regarde si il y a des saturations
	enable = TRUE;
	if(valueADC < SATURATION_LOW || valueADC > SATURATION_HIGH){
		enable = FALSE;
	}

	laser_left[index].enable = enable;

	// On calcule et on stocke la valeur même si le capteur entre en saturation
	pos_mesure.x = robot.x + (OFFSET_LENGTH_LASER_LEFT*cosinus + (OFFSET_WIDTH_LASER_LEFT - value)*sinus)/4096.0;
	pos_mesure.y = robot.y + (OFFSET_LENGTH_LASER_LEFT*sinus - (OFFSET_WIDTH_LASER_LEFT - value)*cosinus)/4096.0;
	// On calcule et on stocke la position du laser (c'est à dire de début d'émission du rayon laser)
	pos_laser.x = robot.x + (OFFSET_LENGTH_LASER_LEFT*cosinus + OFFSET_WIDTH_LASER_LEFT*sinus)/4096.0;
	pos_laser.y = robot.y + (OFFSET_LENGTH_LASER_LEFT*sinus - OFFSET_WIDTH_LASER_LEFT*cosinus)/4096.0;

	// On stocke les valeurs
	laser_left[index].pos_mesure = pos_mesure;
	laser_left[index].pos_laser = pos_laser;
}

static void SCAN_get_data_right(){
	GEOMETRY_point_t pos_mesure, pos_laser;
	bool_e enable;
	Sint16 valueADC, value;
	position_t robot;
	Sint16 cosinus = 0, sinus = 0;

	valueADC = ADC_getValue(ADC_SENSOR_LASER_LEFT);
	value = CONVERSION_LASER_RIGHT(valueADC);
	robot = global.position; // On récupère la position du robot tout de suite
	robot.teta = GEOMETRY_modulo_angle(robot.teta);
	COS_SIN_4096_get(robot.teta, &cosinus, &sinus);

	// On regarde si il y a des saturations
	enable = TRUE;
	if(valueADC < SATURATION_LOW || valueADC > SATURATION_HIGH){
		enable = FALSE;
	}

	laser_right[index].enable = enable;

	// On calcule et on stocke la valeur même si le capteur entre en saturation
	pos_mesure.x = (Sint16) (robot.x + (OFFSET_LENGTH_LASER_RIGHT*cosinus + (OFFSET_WIDTH_LASER_RIGHT + value)*sinus))/4096.0;
	pos_mesure.y = (Sint16) (robot.y + (OFFSET_LENGTH_LASER_RIGHT*sinus - (OFFSET_WIDTH_LASER_RIGHT + value)*cosinus))/4096.0;
	// On calcule et on stocke la position du laser (c'est à dire de début d'émission du rayon laser)
	pos_laser.x = (Sint16) (robot.x + (OFFSET_LENGTH_LASER_RIGHT*cosinus + OFFSET_WIDTH_LASER_RIGHT*sinus))/4096.0;
	pos_laser.y = (Sint16) (robot.y + (OFFSET_LENGTH_LASER_RIGHT*sinus - OFFSET_WIDTH_LASER_RIGHT*cosinus))/4096.0;

	// On stocke les valeurs
	laser_right[index].pos_mesure = pos_mesure;
	laser_right[index].pos_laser = pos_laser;
}


void SCAN_process_it(){

	if((index < NB_SCAN_DATA && !flag_1) || (index >= NB_SCAN_DATA && !flag_2)){
		SCAN_get_data_left();
		SCAN_get_data_right();
	}else{
		flag_treatment_too_slow = TRUE;
	}

	index = (index + 1) % (2*NB_SCAN_DATA);

	if(index == 0)
		flag_2 = TRUE;
	else if(index == NB_SCAN_DATA)
		flag_1 = TRUE;

}

void SCAN_process_main(){
	Uint16 i;
	scan_data_t tab_treatment_left[NB_SCAN_DATA], tab_treatment_right[NB_SCAN_DATA];

	if(flag_treatment_too_slow){
		debug_printf("problème d'accés conccurents\n");
	}

	// Copie de la première partie du tableau pendant que la première partie est en train de se remplir
	if(flag_1){
		debug_printf("\n\n VIDAGE FLAG_1\n");
		for(i=0; i<NB_SCAN_DATA; i++){
			tab_treatment_left[i].pos_mesure = laser_left[i].pos_mesure;
			tab_treatment_left[i].pos_laser = laser_left[i].pos_laser;
			tab_treatment_left[i].enable = laser_left[i].enable;
			tab_treatment_right[i].pos_mesure = laser_right[i].pos_mesure;
			tab_treatment_right[i].pos_laser = laser_right[i].pos_laser;
			tab_treatment_right[i].enable = laser_right[i].enable;
			//debug_printf("i1=%d\n", i);
			//debug_printf("i1=%d l(%4d ; %4d) e= %1d  r(%4d ; %4d) e= %1d\n", i, laser_left[i].pos_mesure.x, laser_left[i].pos_mesure.y,laser_left[i].enable, laser_right[i].pos_mesure.x, laser_right[i].pos_mesure.y, laser_right[i].enable);
		}
		flag_1 = FALSE;

		// Appel de fonctions de scan objets
		OBJECTS_SCAN_treatment(tab_treatment_left);
		OBJECTS_SCAN_treatment(tab_treatment_right);
	}

	// Copie de la seconde partie du tableau pendant que la première partie est en train de se remplir
	if(flag_2){
		debug_printf("\n\n VIDAGE FLAG_2\n");
		for(i=0; i<NB_SCAN_DATA; i++){
			tab_treatment_left[i].pos_mesure = laser_left[NB_SCAN_DATA + i].pos_mesure;
			tab_treatment_left[i].pos_laser = laser_left[NB_SCAN_DATA + i].pos_laser;
			tab_treatment_left[i].enable = laser_left[NB_SCAN_DATA + i].enable;
			tab_treatment_right[i].pos_mesure = laser_right[NB_SCAN_DATA + i].pos_mesure;
			tab_treatment_right[i].pos_laser = laser_right[NB_SCAN_DATA + i].pos_laser;
			tab_treatment_right[i].enable = laser_right[NB_SCAN_DATA + i].enable;
			//debug_printf("i2=%d\n", i);
			//debug_printf("i2=%d l(%4d ; %4d) e= %1d  r(%4d ; %4d) e= %1d\n", i, laser_left[i].pos_mesure.x, laser_left[i].pos_mesure.y,laser_left[i].enable, laser_right[i].pos_mesure.x, laser_right[i].pos_mesure.y, laser_right[i].enable);
		}
		flag_2 = FALSE;

		// Appel des fonctions de scans objets
		OBJECTS_SCAN_treatment(tab_treatment_left);
		OBJECTS_SCAN_treatment(tab_treatment_right);
	}

}
