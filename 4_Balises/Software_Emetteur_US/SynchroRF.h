#ifndef SYNCHRO_BALISES_H
#define SYNCHRO_BALISES_H

#include "QS/QS_all.h"
#include "QS/QS_rf.h"

#define TIME_BASE_UNIT 100 //en [us]
#define DUREE_STEP     2000	//Dur�e d'un step [us]

#define PERIODE_FLASH	50		//P�riode de r�p�tition des flashs [nombre de step]	//P�riode du flash en �s = PERIODE_FLASH * DUREE_STEP
#define FLASH_CYCLE (PERIODE_FLASH*NOMBRE_BALISES_EMETTRICES) //Un cycle d'�mission des N balises
#define NO_FLASH_TIME 4  //[nb de step] 8ms ou on emet rien au d�but et � la fin, pour avoir donc 8 ms entre la fin de l'�mission d'un balise et le d�but de l'autre

#define TIME_PER_MODULE 50 //en [step]
#define TIME_WHEN_SYNCHRO TIME_PER_MODULE*RF_MODULE  //[nombre de step]
#define TOTAL_STEP_COUNT (RF_MODULE_COUNT*TIME_PER_MODULE)  //[nombre de step] step_ir � [0; TOTAL_STEP_COUNT[

//Deviation obesrv�e entre pic et stm32 apr�s 15min de fonctionnement: 0.1ms/s
//On d�cide que 20 sec sans synchro c'est trop et qu'il faut repasser en �mission continue
#define SYNCHRO_TIMEOUT 10000 //en [nombre de step], 10000 = 20s

void SYNCRF_init();
bool_e SYNCRF_is_synchronized(void);
void SYNCRF_process_main();
void SYNCRF_sendRequest();


#endif // SYNCHRO_BALISES_H
