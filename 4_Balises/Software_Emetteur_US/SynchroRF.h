#ifndef SYNCHRO_BALISES_H
#define SYNCHRO_BALISES_H

#include "QS/QS_all.h"

#define TIME_WHEN_SYNCHRO TIME_PER_MODULE*RF_MODULE/2  //[nombre de step]

void SYNCRF_init();
void SYNCRF_process_main();
void SYNCRF_sendRequest();


#endif // SYNCHRO_BALISES_H
