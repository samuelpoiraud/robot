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

#define CONVERSION_LASER_LEFT(x)  ((Sint32)((x)*5385-18695)/10000)
#define DISTANCE_SCAN_CENTER	150

static Sint16 next_position=1100;
static Sint16 tab_scan[80]={};


// il faut encore gérer le décalage en y entre le centre de gravité et le laser

// et améliorer grandement le remplissage du tableau

// mais je sais pas comment m'y prendre (j'ai pas encore trop réfléchi au sujet)

void SCAN_process_it(){
	if(global.flags.scan_dune&&global.position.x>next_position){
		if(global.position.teta<PI4096&&global.position.teta>0)
			tab_scan[(next_position-1100)/10]=-sin4096(global.position.teta)*(CONVERSION_LASER_LEFT(ADC_getValue(ADC_SENSOR_XUK_LEFT)))+global.position.x+DISTANCE_SCAN_CENTER;
		else
			tab_scan[(next_position-1100)/10]=sin4096(global.position.teta)*CONVERSION_LASER_LEFT(ADC_getValue(ADC_SENSOR_XUK_RIGHT))+global.position.x+DISTANCE_SCAN_CENTER;
		next_position=next_position+10;
		if(next_position==1900){
			global.flags.scan_dune=FALSE;
			global.flags.treatment_scan=TRUE;
		}
	}
}

void SCAN_PROCESS(CAN_msg_t *msg){
	typedef enum{
		INIT,
		LANCER_SCAN,
		TRAITEMENT_SCAN,
		END
	}SCAN_MAE_e;

	static SCAN_MAE_e state=INIT;
	switch (state){
		case INIT:
			if(msg!=NULL)
				state=LANCER_SCAN;
			break;
		case LANCER_SCAN:
			global.flags.scan_dune=TRUE;
			state=TRAITEMENT_SCAN;
			break;
		case TRAITEMENT_SCAN:{
			if(global.flags.treatment_scan){
				Uint8 compteur=0;
				Sint16 sum=0;
				Uint8 shift=0;
				Uint8 i=0;
				for(i=0;i<14;i++){
					if(tab_scan[i]<20&&tab_scan[i]>-20){
						compteur++;
						sum+=tab_scan[i];
					}
				}
				for(i=67;i<80;i++){
					if(tab_scan[i]<20&&tab_scan[i]>-20){
						compteur++;
						sum+=tab_scan[i];
					}
				}
				shift=sum/compteur;
				compteur=0;
				sum=0;
				for(i=14;i<67;i++){
					if(((tab_scan[i]-shift)<78)&&((tab_scan[i]-shift)>38)){
						compteur++;
						sum+=i;
					}
				}
				Uint16 middle=(sum*10+1100)/compteur;
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
			state=END;
			}
		}
		break;
		case END:
			global.flags.scan_dune=FALSE;
			global.flags.treatment_scan=FALSE;
			break;
	}
}

//void SCAN_take_value_left(){
	//CONVERSION_LASER_LEFT(ADC_getValue(ADC_SENSOR_BIG_XUK_LEFT));
//}
