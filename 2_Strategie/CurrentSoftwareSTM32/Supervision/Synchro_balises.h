#ifndef SYNCHRO_BALISES_H
#define SYNCHRO_BALISES_H

#include "../QS/QS_all.h"

//Les balises sont elles là ? Informatif, si elles sont là, implique que la RF fonctionne en reception pour ce robot, et en emission pour les balises
extern bool_e balise_here[2];

void SYNCHRO_init();
void SYNCHRO_process_main();


#endif // SYNCHRO_BALISES_H
