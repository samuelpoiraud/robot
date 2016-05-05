/*
 *  Club Robot ESEO 2009 - 2010
 *
 *  Fichier : scan_laser.c
 *  Package : Propulsion
 *  Description : Scan avec scan_laser
 *  Auteur :
 *  Version 201001
 */

#include "QS/QS_all.h"
#include "QS/QS_outputlog.h"
#include "QS/QS_adc.h"
#include "QS/QS_CANmsgList.h"
#include "QS/QS_can.h"
#include "QS/QS_maths.h"
#include "odometry.h"

#define CONVERSION_LASER_LEFT(x)	((Sint32)(-264*(x)+353500)/1000)
#define CONVERSION_LASER_RIGHT(x)	((Sint32)(-264*(x)+354400)/1000)
#define DISTANCE_SCAN_CENTER		146
#define DISTANCE_SCAN_CENTER_Y		60


//#define FIRST_ANALYSIS
#define SECOND_ANALYSIS


#define ZERO_LINE_MAX	20
#define FIRST_LINE		40, 70
#define SECOND_LINE		110, 130
#define THIRD_LINE		150, 190

#ifdef FIRST_ANALYSIS
#define COLUMN_1	15, 16
#define COLUMN_2	21, 22
#define COLUMN_3	27, 28
#define COLUMN_4	33, 34
#define COLUMN_5	39, 40
#define COLUMN_6	45, 46
#define COLUMN_7	51, 52
#define COLUMN_8	57, 58
#define COLUMN_9	63, 64

static bool_e isInIntervalX2(int index, int index2, int min, int max);
static bool_e duneEmpty();
static bool_e noBloc(int index, int index2, int max);
#endif

#ifdef SECOND_ANALYSIS
#define BLOC_SIZE	60
#define TOLERANCE	10
#define MAX_VARIATION	20

static bool_e importantVariation(int index);
static bool_e isInInterval(int index, int min, int max);
#endif


static Sint16 next_position;
static Sint16 tab_scan[80]={};

static bool_e treatment_scan;
static bool_e scan_dune;

static bool_e total_dune;
static bool_e second_part;
static bool_e nothing;
static bool_e wtf;
static int middle;

void SCAN_init(){
	if(ODOMETRY_get_color()==MAGENTA)
		next_position=1100;
	else
		next_position=1900;
	debug_printf("\n\nnext position init = %d\n\n\n", next_position);
}




void SCAN_process_it(){
	if(ODOMETRY_get_color()==MAGENTA){
		if(scan_dune && (global.position.y+(DISTANCE_SCAN_CENTER_Y) > next_position)){
			if(((next_position-1100)/10) >=0 && ((next_position-1100)/10) < 80){
				tab_scan[(next_position-1100)/10] = -(CONVERSION_LASER_LEFT(ADC_getValue(ADC_SENSOR_LASER_LEFT)))+(global.position.x)-(DISTANCE_SCAN_CENTER);
				next_position = next_position + 10;
			}
			if(next_position >= 1900){
				scan_dune = FALSE;
				treatment_scan = TRUE;
			}
		}
	}else{
		if(scan_dune && ((global.position.y-(DISTANCE_SCAN_CENTER_Y)) < next_position)){
			if(((next_position-1100)/10) >=0 && ((next_position-1100)/10) < 80){
				tab_scan[((next_position-1100)/10) - 1] = -CONVERSION_LASER_RIGHT(ADC_getValue(ADC_SENSOR_LASER_RIGHT))+global.position.x-(DISTANCE_SCAN_CENTER);
				next_position = next_position - 10;
			}
			if(next_position <= 1100){
				scan_dune = FALSE;
				treatment_scan = TRUE;
			}
		}
	}
}

void SCAN_PROCESS(CAN_msg_t *msg){
	typedef enum{
		INIT,
		WAIT_MSG,
		LANCER_SCAN,
		TRAITEMENT_SCAN,
		END
	}SCAN_MAE_e;

	static SCAN_MAE_e state = INIT;

	switch (state){
		case INIT:
			scan_dune = FALSE;
			treatment_scan = FALSE;
			total_dune = FALSE;
			second_part = FALSE;
			nothing = FALSE;
			wtf = FALSE;
			middle = 0;
			state = WAIT_MSG;
			break;
		case WAIT_MSG:
			if(msg != NULL)
				state = LANCER_SCAN;
			break;
		case LANCER_SCAN:
			printf("LANCER SCAN\n");
			scan_dune = TRUE;
			state = TRAITEMENT_SCAN;
			break;
		case TRAITEMENT_SCAN:{
			//printf("TRAITEMENT SCAN 1\n");
			if(treatment_scan == TRUE){
				printf("TRAITEMENT SCAN 2\n");
				//Uint8 compteur = 0;
				//Sint16 sum = 0;
				//Uint8 shift = 0;
				Uint8 i=0;
				for(i=0;i<80;i++){
					printf("scan[%d]: %d\n",i,tab_scan[i]);
				}



				#ifdef FIRST_ANALYSIS
				if(isInIntervalX2(COLUMN_4,SECOND_LINE) && isInIntervalX2(COLUMN_5,THIRD_LINE) && isInIntervalX2(COLUMN_6,SECOND_LINE)){
					// Présence de la dune avant on considère que la dune est complète
					debug_printf("La dune est complète\n");

				}else if(isInIntervalX2(COLUMN_4,FIRST_LINE) && isInIntervalX2(COLUMN_5,FIRST_LINE) && isInIntervalX2(COLUMN_6,FIRST_LINE)){
					// La dune avant n'est pas présente
					if(isInIntervalX2(COLUMN_3,FIRST_LINE) && isInIntervalX2(COLUMN_7,FIRST_LINE)){
						// Présence de la dune arrière
						debug_printf("Présence de la dune arrière\n");
					}else{
						debug_printf("C'est la merde pour la dune\n");
					}
				}else if(duneEmpty()){
					// Il n'y a plus de bloc
					debug_printf("La dune n'est plus là\n");
				}else{
					debug_printf("C'est la merde pour la dune 2\n");
				}
				#endif

				#ifdef SECOND_ANALYSIS
				int variation[MAX_VARIATION];
				int nbVariation = 0;
				int previousIndex = 0;

				for(i=2;i<80;i++){
						if(importantVariation(i)){
							//printf("previousIndex=%d\n",previousIndex);
							if((previousIndex + 1) != i){
								if(nbVariation < (MAX_VARIATION - 1)){
									variation[nbVariation] = i;
								}
								//printf("Variation : %d\n", i);
								nbVariation++;
								previousIndex = i;
						}
					}
				}
				if(nbVariation <= 6){
					if(nbVariation >= 4){
						debug_printf("Dune avant présente\n");
						total_dune = TRUE;
						second_part = FALSE;
						nothing = FALSE;
						wtf = FALSE;
						middle = (variation[0] + variation[nbVariation-1]) / 2;
					}else if(nbVariation >= 2){
						// On vérifie que l'on a bien que des cubes sur une seule ligne
						bool_e confirmation = TRUE;
						for(i=0; i<nbVariation - 1; i++){
							int mid = (variation[i] + variation[i+1]) / 2;
							if(mid > 0 && mid < MAX_VARIATION){
								if(!isInInterval(tab_scan[mid],FIRST_LINE)){
									confirmation = FALSE;
								}
							}
						}
						if(confirmation){
							debug_printf("Dune arrière présente\n");
							total_dune = FALSE;
							second_part = TRUE;
							nothing = FALSE;
							wtf = FALSE;
						}else{
							debug_printf("C'est la merde dans la dune 2\n");
							total_dune = FALSE;
							second_part = FALSE;
							nothing = FALSE;
							wtf = TRUE;
						}

					}else{
						debug_printf("La dune n'est pas là\n");
						total_dune = FALSE;
						second_part = FALSE;
						nothing = TRUE;
						wtf = FALSE;
					}
				}else{
					debug_printf("C'est la merde dans la dune\n");
					total_dune = FALSE;
					second_part = FALSE;
					nothing = FALSE;
					wtf = TRUE;
				}

				debug_printf("Nombre de variation : %d\n", nbVariation);

				CAN_msg_t msg;
				msg.sid = STRAT_BACK_SCAN;
				msg.size = SIZE_STRAT_BACK_SCAN;
				msg.data.strat_back_scan.second_part = second_part;
				msg.data.strat_back_scan.nothing = nothing;
				msg.data.strat_back_scan.total_dune = total_dune;
				msg.data.strat_back_scan.wtf = wtf;
				msg.data.strat_back_scan.middle=middle;
				CAN_send(&msg);

				#endif


				/*
				for(i=0;i<14;i++){
					if(tab_scan[i] < 20 && tab_scan[i] > -20){
						compteur++;
						sum+=tab_scan[i];
					}
				}
				for(i=67;i<80;i++){
					if(tab_scan[i]< 20 && tab_scan[i] > -20){
						compteur++;
						sum+=tab_scan[i];
					}
				}
				if(compteur!=0)
					shift = sum/compteur;
				compteur=0;
				sum=0;
				for(i=14;i<67;i++){
					if(((tab_scan[i]-shift)<78) && ((tab_scan[i]-shift)>38)){
						compteur++;
						sum += i;
					}
				}
				Uint16 middle;
				if(compteur!=0)
					middle=(sum*10)/compteur + 1100;
				if(compteur>45){
					CAN_msg_t msg;
					msg.sid = STRAT_BACK_SCAN;
					msg.size = SIZE_STRAT_BACK_SCAN;
					msg.data.strat_back_scan.second_part = TRUE;
					msg.data.strat_back_scan.nothing = FALSE;
					msg.data.strat_back_scan.total_dune = FALSE;
					msg.data.strat_back_scan.wtf = FALSE;
					msg.data.strat_back_scan.middle=middle;
					CAN_send(&msg);
					debug_printf("Il a laissé la deuxième partie de la dune");
				}else{
					compteur = 0;
					for(i=14;i<32;i++){
						if(((tab_scan[i]-shift)<78)&&((tab_scan[i]-shift)>38)){
							compteur++;
						}
					}
					for(i=49;i<67;i++){
						if(((tab_scan[i]-shift)<78)&&((tab_scan[i]-shift)>38)){
							compteur++;
						}
					}
					for(i=32;i<49;i++){
						if(((tab_scan[i]-shift)<96)&&((tab_scan[i]-shift)>194)){
							compteur++;
						}
					}
					if(compteur>45){
						CAN_msg_t msg;
						msg.sid = STRAT_BACK_SCAN;
						msg.size = SIZE_STRAT_BACK_SCAN;
						msg.data.strat_back_scan.total_dune = TRUE;
						msg.data.strat_back_scan.second_part = FALSE;
						msg.data.strat_back_scan.nothing = FALSE;
						msg.data.strat_back_scan.wtf = FALSE;

						CAN_send(&msg);
						debug_printf("Il a tout laissé on s'éclate c'est la fête");
					}else{
						sum=0;
						for(i=14;i<67;i++){
							sum+=tab_scan[i];
						}
						if(sum>3000){
							CAN_msg_t msg;
							msg.sid = STRAT_BACK_SCAN;
							msg.size = SIZE_STRAT_BACK_SCAN;
							msg.data.strat_back_scan.wtf = TRUE;
							msg.data.strat_back_scan.total_dune = FALSE;
							msg.data.strat_back_scan.second_part = FALSE;
							msg.data.strat_back_scan.nothing = FALSE;
							CAN_send(&msg);
							debug_printf("Il reste de la merde à prendre");
						}else{
							CAN_msg_t msg;
							msg.sid = STRAT_BACK_SCAN;
							msg.size = SIZE_STRAT_BACK_SCAN;
							msg.data.strat_back_scan.nothing = TRUE;
							msg.data.strat_back_scan.second_part = FALSE;
							msg.data.strat_back_scan.total_dune = FALSE;
							msg.data.strat_back_scan.wtf = FALSE;
							CAN_send(&msg);
							debug_printf("Il reste plus rien il nous reste plus qu'à dégager");
						}
					}
				}
				*/

				state=END;
			}

		}
		break;
		case END:
			scan_dune=FALSE;
			treatment_scan=FALSE;
			break;
	}
}

#ifdef FIRST_ANALYSIS
static bool_e isInIntervalX2(int index, int index2, int min, int max){
	if(tab_scan[index] >= min && tab_scan[index] <= max && tab_scan[index2] >= min && tab_scan[index2] <= max){
		return FALSE;
	}else{
		return TRUE;
	}
}

static bool_e duneEmpty(){
	if(noBloc(COLUMN_1,ZERO_LINE_MAX) && noBloc(COLUMN_2,ZERO_LINE_MAX) && noBloc(COLUMN_3,ZERO_LINE_MAX) &&
			noBloc(COLUMN_4,ZERO_LINE_MAX) && noBloc(COLUMN_5,ZERO_LINE_MAX) && noBloc(COLUMN_6,ZERO_LINE_MAX) &&
			noBloc(COLUMN_7,ZERO_LINE_MAX) && noBloc(COLUMN_8,ZERO_LINE_MAX) && noBloc(COLUMN_9,ZERO_LINE_MAX)){
		return TRUE;
	}else{
		return FALSE;
	}
}

static bool_e noBloc(int index, int index2, int max){
	if(tab_scan[index] <= max && tab_scan[index2] <= max){
		return TRUE;
	}else{
		return FALSE;
	}
}
#endif

#ifdef SECOND_ANALYSIS
static bool_e importantVariation(int index){
	if(absolute(tab_scan[index-2]-tab_scan[index]) >= (BLOC_SIZE - TOLERANCE)){
		return TRUE;
	}else{
		return FALSE;
	}
}

static bool_e isInInterval(int index, int min, int max){
	if(tab_scan[index] >= min && tab_scan[index] <= max){
		return FALSE;
	}else{
		return TRUE;
	}
}

#endif
