#ifndef SYNCHRO_BALISES_H
#define SYNCHRO_BALISES_H

#include "QS/QS_all.h"
#include "QS/QS_rf.h"

#define TIME_BASE_UNIT 100 //en [us]
#define DUREE_STEP     2000	//Durée d'un step [us]

#define TIME_PER_MODULE 50 //en [step]
#define TIME_WHEN_SYNCHRO TIME_PER_MODULE*RF_MODULE  //[nombre de step]

void SYNCRF_init();
void SYNCRF_process_main();
void SYNCRF_sendRequest();


#endif // SYNCHRO_BALISES_H
