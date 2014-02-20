#define _CODEURS_C

#include "encoders.h"
#include "QS/QS_qei.h"


void ENCODERS_init(void)
{
	QEI_init();
}

/*
Fonction qui modifie delta_G et delta_D :
distance parcouru par la roue Gauche et par la roue droite
depuis le dernier appel à cette fonction.
*/
void ENCODERS_get(Sint32 * left, Sint32 * right)
{
	//variables locales (car on fait les calculs en Sint16 et le retour est en Sint32...
	Sint32 delta_left;
	Sint32 delta_right;
	Sint16 count_left;
	Sint16 count_right;
	static Sint16 count_left_prec = 0;
	static Sint16 count_right_prec = 0;

	
	//Lire les roues codeuses
	count_right=-QEI1_get_count();
	count_left=QEI2_get_count();
	

	//Mise a jour des deltas des roues...
	delta_left=count_left-count_left_prec;
	delta_right=count_right-count_right_prec;

	if(delta_left > 512)
		delta_left -= 65536;
	else if(delta_left < -512)
		delta_left += 65536;
	if(delta_right > 512)
		delta_right -= 65536;
	else if(delta_right < -512)
		delta_right += 65536;
	
	*left = (Sint32)delta_left;
	*right = (Sint32)delta_right;
		
	//Sauvegarde de l'état actuel pour le prochain appel.
	count_left_prec=count_left;
	count_right_prec=count_right;
}
