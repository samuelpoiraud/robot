#define _CPLD_C

#include "cpld.h"

#ifdef USE_CPLD


void CPLD_init(void)
{
	RAZ_CPLD=1;
	//6 nop  - ils sont là pour qu'on attende bien que le CPLD soit prêt...
	nop();
	nop();
	nop();
	nop();
	nop();
	nop();
	RAZ_CPLD=0;
}


Sint16 CPLD_get_count(CPLD_wheel_e wheel)
{
	Sint16 wheel_info = 0;	//raz compteur.
	Uint16 tmp;
			// roue Gauche 
	if(wheel==CPLD_LEFT_WHEEL)
		SEL_ROUE_CPLD=0;
	else	// roue droite
		SEL_ROUE_CPLD=1;

	SEL_OCTET_CPLD=1;	//On demande au CPLD les bits de poids forts.

	nop();
	nop();
	nop();
	nop();
	nop();
	nop();
	nop();
	nop();
	nop();
	nop();
	nop();
	nop();
	nop();
	nop();
	nop();

	tmp = HIGHINT(PORT_COMPT);
	
	wheel_info = 	((tmp & 0b0000000010000000)>>7) + 
						((tmp & 0b0000000001000000)>>5) +
			((tmp & 0b0000000000100000)>>3) +
			((tmp & 0b0000000000010000)>>1) +
			((tmp & 0b0000000000001000)<<1) +
			((tmp & 0b0000000000000100)<<3) +
			((tmp & 0b0000000000000010)<<5) +
						((tmp & 0b0000000000000001)<<7) ;
	wheel_info = wheel_info << 8; 
	
	// Sur la carte 2010 les pins du port étaient dans le bon sens ...
	//stockage des bits de poids forts à leur place
	//wheel_info = PORT_COMPT & 0xFF00;
	
	SEL_OCTET_CPLD=0;	//On demande au CPLD les bits de poids faible.

	nop();
	nop();
	nop();
	nop();
	nop();
	nop();
	nop();
	nop();
	nop();
	nop();
	nop();
	nop();

	tmp = HIGHINT(PORT_COMPT);
		
	wheel_info += 	((tmp & 0b0000000010000000)>>7) + 
			((tmp & 0b0000000001000000)>>5) +
			((tmp & 0b0000000000100000)>>3) +
			((tmp & 0b0000000000010000)>>1) +
			((tmp & 0b0000000000001000)<<1) +
			((tmp & 0b0000000000000100)<<3) +
			((tmp & 0b0000000000000010)<<5) +
			((tmp & 0b0000000000000001)<<7) ;
					
	// Sur la carte 2010 les pins du port étaient dans le bon sens ...
	//on ajoute les bits de poids faible en bas du compteur.
	//wheel_info += HIGHINT(PORT_COMPT);	

	return wheel_info;
}
		
#endif

