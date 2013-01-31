#define _CODEURS_C

#include "encoders.h"

#ifdef USE_CODEUR_SUR_IT_ET_QE
	#include "QS/QS_qei.h"
	#include "QS/QS_qei_on_it.h"
#else
	#include "cpld.h"
#endif


		
void ENCODERS_init(void)
{
	
	#ifdef USE_CODEUR_SUR_IT_ET_QE
			QEI_ON_IT_init();
			QEI_init();
			IPC4bits.INT1IP = 7;	//On monte au maximum les priorités des codeurs !!!	Ils ne doivent pas être préemptés !
			IPC5bits.INT2IP = 7;
		#else
			CPLD_init();
		#endif
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

		#ifdef USE_CODEUR_SUR_IT_ET_QE
			//Lire les roues codeuses
			count_right=QEI_get_count();
			count_left=QEI_ON_IT_get_count();
		#else
			count_right=CPLD_get_count(CPLD_RIGHT_WHEEL);
			count_left=CPLD_get_count(CPLD_LEFT_WHEEL);
	#endif

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

