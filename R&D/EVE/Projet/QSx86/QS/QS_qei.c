/*
 *	Club Robot ESEO 2008 - 2010
 *	Archi-Tech', PACMAN
 *
 *	Fichier : QS_qei.c
 *	Package : Qualite Software
 *	Description :	Utilise le module QEI pour décoder les
					signaux d'un codeur incrémental
 *	Auteur : Jacen
 *	Licence : CeCILL-C (voir LICENCE.txt)
 *	Version 20100620
 */

#include "QS_qei.h"

//#include <qei.h>

/*-------------------------------------
	Initialisation de l'interface
-------------------------------------*/
void QEI_init() 
{
	static bool_e initialized = FALSE;
	if(initialized)
		return;
	initialized = TRUE;

/*	OpenQEI(QEI_DIR_SEL_QEB &
			QEI_EXT_CLK &
			QEI_INDEX_RESET_DISABLE &
			QEI_GATED_ACC_DISABLE &
			QEI_NORMAL_IO &
			QEI_INPUTS_NOSWAP &
			QEI_MODE_x4_MATCH &
			//QEI_MODE_x4_PULSE &
			QEI_UP_COUNT &
			QEI_IDLE_STOP,
			QEI_QE_CLK_DIVIDE_1_1 &
			QEI_QE_OUT_DISABLE &
			POS_CNT_ERR_INT_DISABLE
			);
	DisableIntQEI;*/
}
/*-------------------------------------
	Recupération de la valeur du codeur
-------------------------------------*/
Sint16 QEI_get_count()
{
	return 0;//(Sint16)ReadQEI();
}

void QEI_set_count(Sint16 count)
{	
	//POSCNT = count;
}
